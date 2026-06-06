#!/usr/bin/env python3
"""Prepare LDBC SNB Interactive v1 data for the recursive/graph benchmark.

One command can now do everything: download the official dataset, normalize it,
and (optionally) load it into Neo4j.

    python prepare_ldbc.py --sf 1 --download                 # fetch + normalize
    python prepare_ldbc.py --sf 1 --raw-dir <social_network> # normalize local data
    python prepare_ldbc.py --sf 1 --neo4j-load --neo4j-import-dir <neo4j>/import

All three engines (DuckDB, BumbleBee, Neo4j) then read one canonical CSV set:

    downloads/ldbc/sf<N>/{person,knows,message,place}.csv

mirroring the upstream LDBC DuckDB loader (duckdb/ddl/{schema,load}.sql):
  * `message` = post UNION ALL comment (m_c_replyof = coalesce(replyof_post, replyof_comment))
  * `knows`   is materialized BOTH directions (so directed `k_person1id = :id` filters
              behave like Neo4j's undirected KNOWS)
Temporal columns are exported as epoch-second BIGINTs and birthday as YYYYMMDD INT,
so ordering/equality is identical across engines with no timestamp-format drift.
Date parsing is robust to BOTH the LongDateFormatter (epoch-millisecond integers)
and the ISO-string datagen variants.

--download fetches the SF<N> 'CsvCompositeMergeForeign' archive from the LDBC
Cloudflare R2 host (datasets.ldbcouncil.org) and extracts the .tar.zst. Needs curl
and zstd (`brew install zstd`).
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

QUERY_DIRS = ["files/ldbc/input_duckdb", "files/ldbc/input_bumblebee", "files/ldbc/input_neo4j"]
PARAMS_JSON = "files/ldbc/params.json"

DOWNLOAD_DIR = Path("downloads/raw")
# LDBC datasets moved to Cloudflare R2 (2026); direct download, no staging.
R2_BASE = "https://datasets.ldbcouncil.org/snb-interactive-v1"
ARCHIVE_TMPL = "social_network-sf{sf}-CsvCompositeMergeForeign-LongDateFormatter.tar.zst"

# format-agnostic conversions (raw date columns are staged as VARCHAR):
#   epoch-ms integer  -> /1000 ;  ISO string -> strptime -> epoch
EPOCH_SECS = (
    "CAST(coalesce("
    "TRY_CAST({c} AS BIGINT)/1000, "
    "epoch(TRY_STRPTIME({c}, '%Y-%m-%dT%H:%M:%S.%g+0000')), "
    "epoch(TRY_STRPTIME({c}, '%Y-%m-%d %H:%M:%S'))"
    ") AS BIGINT)"
)
BIRTHDAY_INT = (
    "CAST(strftime(coalesce("
    "to_timestamp(TRY_CAST({c} AS BIGINT)/1000), "
    "TRY_STRPTIME({c}, '%Y-%m-%d')"
    "), '%Y%m%d') AS INT)"
)


def build_sql(raw, out):
    e = lambda c: EPOCH_SECS.format(c=c)  # noqa: E731
    return f"""
CREATE TABLE post (
    m_messageid bigint, m_ps_imagefile varchar, m_creationdate varchar,
    m_locationip varchar, m_browserused varchar, m_ps_language varchar,
    m_content varchar, m_length int, m_creatorid bigint, m_ps_forumid bigint,
    m_locationid bigint
);
CREATE TABLE comment (
    m_messageid bigint, m_creationdate varchar, m_locationip varchar,
    m_browserused varchar, m_content varchar, m_length int, m_creatorid bigint,
    m_locationid bigint, m_replyof_post bigint, m_replyof_comment bigint
);
CREATE TABLE person (
    p_personid bigint, p_firstname varchar, p_lastname varchar, p_gender varchar,
    p_birthday varchar, p_creationdate varchar, p_locationip varchar,
    p_browserused varchar, p_placeid bigint,
    p_language varchar, p_email varchar  -- inline semicolon-lists in composite-merged-fk
);
CREATE TABLE knows (k_person1id bigint, k_person2id bigint, k_creationdate varchar);
CREATE TABLE place (
    pl_placeid bigint, pl_name varchar, pl_url varchar, pl_type varchar,
    pl_containerplaceid bigint
);

COPY post    FROM '{raw}/dynamic/post_0_0.csv'                 (DELIMITER '|', HEADER, NULLSTR '');
COPY comment FROM '{raw}/dynamic/comment_0_0.csv'             (DELIMITER '|', HEADER, NULLSTR '');
COPY person  FROM '{raw}/dynamic/person_0_0.csv'              (DELIMITER '|', HEADER, NULLSTR '');
COPY place   FROM '{raw}/static/place_0_0.csv'                (DELIMITER '|', HEADER, NULLSTR '');
COPY knows   FROM '{raw}/dynamic/person_knows_person_0_0.csv' (DELIMITER '|', HEADER, NULLSTR '');
INSERT INTO knows SELECT k_person2id, k_person1id, k_creationdate FROM knows;  -- both directions

CREATE TABLE message AS
    SELECT m_messageid, m_creationdate, m_creatorid, m_locationid, CAST(NULL AS bigint) AS m_c_replyof FROM post
  UNION ALL
    SELECT m_messageid, m_creationdate, m_creatorid, m_locationid,
           coalesce(m_replyof_post, m_replyof_comment) AS m_c_replyof FROM comment;

COPY (
  SELECT p_personid, p_firstname, p_lastname, p_gender,
         {BIRTHDAY_INT.format(c='p_birthday')}     AS p_birthday,
         {e('p_creationdate')}                     AS p_creationdate,
         p_locationip, p_browserused, p_placeid
  FROM person
) TO '{out}/person.csv' (HEADER, DELIMITER ',');

COPY (
  SELECT k_person1id, k_person2id, {e('k_creationdate')} AS k_creationdate FROM knows
) TO '{out}/knows.csv' (HEADER, DELIMITER ',');

COPY (
  SELECT m_messageid, {e('m_creationdate')} AS m_creationdate,
         m_creatorid, m_locationid, m_c_replyof
  FROM message
) TO '{out}/message.csv' (HEADER, DELIMITER ',');

COPY (
  SELECT pl_placeid, pl_name, pl_type, pl_containerplaceid FROM place
) TO '{out}/place.csv' (HEADER, DELIMITER ',');
"""


# --- normalized CSV -> Neo4j graph --------------------------------------------
NEO4J_LOAD = r"""
CREATE CONSTRAINT person_id IF NOT EXISTS FOR (p:Person)  REQUIRE p.id IS UNIQUE;
CREATE CONSTRAINT msg_id    IF NOT EXISTS FOR (m:Message) REQUIRE m.id IS UNIQUE;
CREATE CONSTRAINT place_id  IF NOT EXISTS FOR (pl:Place)  REQUIRE pl.id IS UNIQUE;

LOAD CSV WITH HEADERS FROM 'file:///place.csv' AS row
CALL { WITH row CREATE (:Place {id: toInteger(row.pl_placeid), name: row.pl_name,
       type: row.pl_type, containerId: toInteger(row.pl_containerplaceid)}) } IN TRANSACTIONS OF 10000 ROWS;

MATCH (city:Place) WHERE city.containerId IS NOT NULL
CALL { WITH city MATCH (country:Place {id: city.containerId})
       MERGE (city)-[:IS_PART_OF]->(country) } IN TRANSACTIONS OF 10000 ROWS;

LOAD CSV WITH HEADERS FROM 'file:///person.csv' AS row
CALL { WITH row CREATE (:Person {id: toInteger(row.p_personid), firstName: row.p_firstname,
       lastName: row.p_lastname, gender: row.p_gender, birthday: toInteger(row.p_birthday),
       creationDate: toInteger(row.p_creationdate), locationIP: row.p_locationip,
       browserUsed: row.p_browserused, placeId: toInteger(row.p_placeid)}) } IN TRANSACTIONS OF 10000 ROWS;

MATCH (p:Person)
CALL { WITH p MATCH (city:Place {id: p.placeId}) MERGE (p)-[:IS_LOCATED_IN]->(city) } IN TRANSACTIONS OF 10000 ROWS;

LOAD CSV WITH HEADERS FROM 'file:///message.csv' AS row
CALL { WITH row CREATE (:Message {id: toInteger(row.m_messageid),
       creationDate: toInteger(row.m_creationdate), creatorId: toInteger(row.m_creatorid),
       locationId: toInteger(row.m_locationid),
       replyOf: CASE row.m_c_replyof WHEN '' THEN null ELSE toInteger(row.m_c_replyof) END}) } IN TRANSACTIONS OF 10000 ROWS;

MATCH (m:Message)
CALL { WITH m MATCH (p:Person {id: m.creatorId}) MERGE (m)-[:HAS_CREATOR]->(p) } IN TRANSACTIONS OF 10000 ROWS;
MATCH (m:Message)
CALL { WITH m MATCH (pl:Place {id: m.locationId}) MERGE (m)-[:IS_LOCATED_IN]->(pl) } IN TRANSACTIONS OF 10000 ROWS;
MATCH (c:Message) WHERE c.replyOf IS NOT NULL
CALL { WITH c MATCH (m:Message {id: c.replyOf}) MERGE (c)-[:REPLY_OF]->(m) } IN TRANSACTIONS OF 10000 ROWS;

LOAD CSV WITH HEADERS FROM 'file:///knows.csv' AS row
WITH row WHERE toInteger(row.k_person1id) < toInteger(row.k_person2id)
CALL { WITH row MATCH (a:Person {id: toInteger(row.k_person1id)}), (b:Person {id: toInteger(row.k_person2id)})
       MERGE (a)-[:KNOWS {creationDate: toInteger(row.k_creationdate)}]->(b) } IN TRANSACTIONS OF 10000 ROWS;
"""


def _require(binary, hint):
    if shutil.which(binary) is None:
        sys.exit(f"Error: '{binary}' not found on PATH. {hint}")


def download_dataset(sf):
    """Fetch + extract the SF<sf> archive from the LDBC R2 host."""
    _require("curl", "")
    _require("zstd", "Install it with `brew install zstd`.")
    DOWNLOAD_DIR.mkdir(parents=True, exist_ok=True)
    archive = ARCHIVE_TMPL.format(sf=sf)
    url = f"{R2_BASE}/{archive}"
    dest = DOWNLOAD_DIR / archive

    if not dest.exists():
        print(f"[download] fetching {url}")
        # -C - resumes a partial file; --retry survives intermittent failures
        rc = subprocess.run(["curl", "-L", "--fail", "-C", "-",
                             "--retry", "5", "--retry-delay", "5",
                             "-o", str(dest), url]).returncode
        if rc != 0:
            sys.exit(f"Download failed (curl exit {rc}). Check the SF<{sf}> archive exists at:\n  {url}\n"
                     f"or download it manually and use --raw-dir.")
    else:
        print(f"[download] reusing {dest}")

    extract_dir = DOWNLOAD_DIR / f"sf{sf}_extracted"
    extract_dir.mkdir(exist_ok=True)
    print(f"[extract] {dest} -> {extract_dir}")
    # zstd -dc archive.tar.zst | tar -xf - -C extract_dir
    zstd = subprocess.Popen(["zstd", "-dc", str(dest)], stdout=subprocess.PIPE)
    subprocess.run(["tar", "-xf", "-", "-C", str(extract_dir)], stdin=zstd.stdout, check=True)
    zstd.wait()
    return find_social_network(extract_dir)


def find_social_network(root):
    """Locate the directory containing both dynamic/ and static/ subfolders."""
    root = Path(root)
    for d in [root, *root.rglob("*")]:
        if d.is_dir() and (d / "dynamic").is_dir() and (d / "static").is_dir():
            return str(d)
    sys.exit(f"Could not find a 'social_network' folder (with dynamic/ + static/) under {root}")


def run_duckdb(sql):
    duckdb = os.environ.get("DUCKDB", "duckdb")
    _require(duckdb, "Install DuckDB or set $DUCKDB to its path.")
    if subprocess.run([duckdb, "-c", sql], text=True).returncode != 0:
        sys.exit("DuckDB normalization failed")


def normalize(raw_dir, out_dir):
    out_dir.mkdir(parents=True, exist_ok=True)
    raw = str(Path(raw_dir).resolve())
    print(f"[normalize] {raw} -> {out_dir}")
    run_duckdb(build_sql(raw, str(out_dir.resolve())))
    for f in ("person.csv", "knows.csv", "message.csv", "place.csv"):
        print(f"  {f}: {'ok' if (out_dir / f).exists() else 'MISSING'}")


def neo4j_load(out_dir, import_dir):
    _require("cypher-shell", "Install it with `brew install cypher-shell`.")
    if not import_dir:
        sys.exit("--neo4j-import-dir is required with --neo4j-load "
                 "(Neo4j only reads file:/// CSVs from its import directory)")
    import_dir = Path(import_dir)
    import_dir.mkdir(parents=True, exist_ok=True)
    for f in ("person.csv", "knows.csv", "message.csv", "place.csv"):
        shutil.copy(out_dir / f, import_dir / f)
        print(f"[neo4j] copied {f} -> {import_dir}")
    uri = os.environ.get("NEO4J_URI", "neo4j://localhost:7687")
    user = os.environ.get("NEO4J_USER", "neo4j")
    pw = os.environ.get("NEO4J_PASSWORD", "neo4j")
    print("[neo4j] loading graph via cypher-shell ...")
    if subprocess.run(["cypher-shell", "-a", uri, "-u", user, "-p", pw],
                      input=NEO4J_LOAD, text=True).returncode != 0:
        sys.exit("Neo4j load failed")
    print("[neo4j] done")


def _duckdb_rows(sql):
    duckdb = os.environ.get("DUCKDB", "duckdb")
    out = subprocess.run([duckdb, "-csv", "-noheader", "-c", sql],
                         capture_output=True, text=True)
    if out.returncode != 0:
        sys.exit(f"DuckDB query failed:\n{out.stderr}")
    return [line.split(",") for line in out.stdout.splitlines() if line.strip()]


def pick_and_apply(sf):
    """Pick valid query literals from the normalized SF<sf> data and substitute
    them into every query file + params.json (idempotent: old values come from
    params.json, so re-running re-picks from the current literals)."""
    data = (Path("downloads/ldbc") / f"sf{sf}").resolve()
    msg = str(data / "message.csv")
    knows = str(data / "knows.csv")
    place = str(data / "place.csv")
    if not (data / "person.csv").exists():
        sys.exit(f"No normalized data at {data}; run --download or --raw-dir first.")

    # a message that HAS replies and whose creator HAS friends -> non-empty IS7/IC8/IS3/IC2
    cand = _duckdb_rows(f"""
        SELECT m.m_messageid, m.m_creatorid
        FROM read_csv_auto('{msg}') m
        WHERE m.m_messageid IN (
                SELECT TRY_CAST(m_c_replyof AS BIGINT) FROM read_csv_auto('{msg}') WHERE m_c_replyof IS NOT NULL)
          AND m.m_creatorid IN (SELECT k_person1id FROM read_csv_auto('{knows}'))
        ORDER BY m.m_messageid LIMIT 1;""")
    if not cand:
        sys.exit("Could not find a message-with-replies whose author has friends; data may be incomplete.")
    new_message_id, new_person_id = cand[0][0], cand[0][1]

    # two countries with the most messages (IC3 may still be empty if no friend posted in both)
    countries = _duckdb_rows(f"""
        SELECT pl.pl_name
        FROM read_csv_auto('{place}') pl, read_csv_auto('{msg}') m
        WHERE m.m_locationid = pl.pl_placeid AND pl.pl_type = 'country'
        GROUP BY pl.pl_name ORDER BY count(*) DESC LIMIT 2;""")
    new_cx = countries[0][0] if countries else "China"
    new_cy = countries[1][0] if len(countries) > 1 else "India"

    # current (old) literals come from params.json
    params = json.load(open(PARAMS_JSON))
    old_person_id = str(params["IS3"]["personId"])
    old_message_id = str(params["IS7"]["messageId"])
    old_cx = params["IC3"]["countryXName"]
    old_cy = params["IC3"]["countryYName"]

    # single-pass, word-bounded substitution (longer tokens first; no cascade because
    # new values never equal an old token)
    repl = {old_message_id: new_message_id, old_person_id: new_person_id,
            old_cx: new_cx, old_cy: new_cy}
    toks = sorted((re.escape(k) for k in repl), key=len, reverse=True)
    pattern = re.compile(r"\b(" + "|".join(toks) + r")\b")

    changed = 0
    for d in QUERY_DIRS:
        for f in sorted(Path(d).glob("*")):
            text = f.read_text()
            new = pattern.sub(lambda m: repl[m.group(0)], text)
            if new != text:
                f.write_text(new)
                changed += 1

    for q in params:
        if isinstance(params[q], dict):
            if "personId" in params[q]:
                params[q]["personId"] = int(new_person_id)
            if "messageId" in params[q]:
                params[q]["messageId"] = int(new_message_id)
    params["IC3"]["countryXName"] = new_cx
    params["IC3"]["countryYName"] = new_cy
    json.dump(params, open(PARAMS_JSON, "w"), indent=2)

    print(f"[pick-params] personId {old_person_id} -> {new_person_id}, "
          f"messageId {old_message_id} -> {new_message_id}, "
          f"countries '{old_cx}','{old_cy}' -> '{new_cx}','{new_cy}'  ({changed} files updated)")


def set_active(sf):
    """Point downloads/ldbc/active -> sf<sf> so the query files (which read
    downloads/ldbc/active/) use whichever scale factor was just prepared."""
    link = Path("downloads/ldbc") / "active"
    target = f"sf{sf}"
    if link.is_symlink() or link.exists():
        link.unlink()
    link.symlink_to(target)  # relative symlink inside downloads/ldbc/
    print(f"[active] downloads/ldbc/active -> {target}")


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--sf", default="1", help="scale factor (URL + output dir downloads/ldbc/sf<SF>)")
    ap.add_argument("--download", action="store_true",
                    help="download + extract the SF<SF> dataset from the LDBC SURF repo")
    ap.add_argument("--raw-dir", help="path to an extracted social_network/ folder (dynamic/ + static/)")
    ap.add_argument("--pick-params", action="store_true",
                    help="pick valid query literals from the data and write them into the query files + params.json")
    ap.add_argument("--neo4j-load", action="store_true", help="load normalized CSVs into local Neo4j")
    ap.add_argument("--neo4j-import-dir", help="Neo4j import/ directory (CSVs are copied here)")
    args = ap.parse_args()

    out_dir = Path("downloads/ldbc") / f"sf{args.sf}"

    raw_dir = args.raw_dir
    if args.download:
        raw_dir = download_dataset(args.sf)

    if raw_dir:
        normalize(raw_dir, out_dir)
    elif not (out_dir / "person.csv").exists():
        sys.exit(f"No normalized data at {out_dir}. Use --download to fetch it, or "
                 f"--raw-dir to point at an extracted social_network/ folder.")
    else:
        print(f"[normalize] reusing existing normalized CSVs in {out_dir}")

    set_active(args.sf)  # queries read downloads/ldbc/active/ -> this SF

    if args.pick_params:
        pick_and_apply(args.sf)

    if args.neo4j_load:
        neo4j_load(out_dir, args.neo4j_import_dir)


if __name__ == "__main__":
    main()