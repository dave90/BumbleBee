#!/usr/bin/env python3
"""
Generate a synthetic ClickBench-"hits" dataset.

The real ClickBench dataset (datasets.clickhouse.com / huggingface) is not
reachable from this environment, so we synthesize a dataset with the SAME
105-column schema and column NAMES, giving the ~15 query-relevant columns
realistic cardinalities / skew and the other ~90 columns cheap values. This
reproduces ClickBench's defining shape (very wide table, queries touch a few
columns) so projection-pushdown, parallel scan, high-cardinality GROUP BY and
string filtering are all exercised the same way the real benchmark exercises
them. Both DuckDB and BumbleBee read the identical file, so the engine-vs-engine
gap is measured apples-to-apples.

Usage: gen_synthetic_hits.py <rows> <out.parquet> [threads] [row_group_size]
"""
import sys, duckdb

rows = int(sys.argv[1])
out = sys.argv[2]
threads = int(sys.argv[3]) if len(sys.argv) > 3 else 4
rgs = int(sys.argv[4]) if len(sys.argv) > 4 else 122880

# Query-relevant columns get realistic generators; everything else is cheap.
# URL: 30% drawn from a small "popular" head (so top-N GROUP BY is meaningful),
# 70% a high-cardinality long tail. ~20% of tail domains contain 'google'
# (Q20/Q21 filter URL LIKE '%google%').
expr = {
    # near-unique hit id -> Q32 GROUP BY WatchID,ClientIP makes ~N groups
    '"WatchID"': "(random()*9e18)::BIGINT",
    '"ClientIP"': "(random()*2000000)::INTEGER",          # ~2M distinct
    '"RegionID"': "(random()*9000)::INTEGER",
    # UserID: mostly random (Q03 AVG), with the Q19 probe value injected sparsely
    '"UserID"': "CASE WHEN random() < 0.00001 THEN 435090932899640449 ELSE (random()*9e18)::BIGINT END",
    '"CounterID"': "(random()*5000)::INTEGER",            # Q36 CounterID=62 exists
    '"EventDate"': "DATE '2013-07-01' + (random()*30)::INTEGER",
    '"EventTime"': "TIMESTAMP '2013-07-01' + ((random()*2592000)::INTEGER * INTERVAL 1 SECOND)",
    '"ResolutionWidth"': "(random()*2500)::SMALLINT",     # Q02/Q30/Q32 AVG
    '"SearchEngineID"': "(random()*30)::SMALLINT",        # low card
    '"AdvEngineID"': "CASE WHEN random() < 0.02 THEN (random()*40)::SMALLINT ELSE 0 END",  # ~98% zero
    '"IsRefresh"': "CASE WHEN random() < 0.15 THEN 1 ELSE 0 END",
    '"DontCountHits"': "CASE WHEN random() < 0.05 THEN 1 ELSE 0 END",
    # SearchPhrase: ~85% empty; non-empty drawn with skew from a phrase pool
    '"SearchPhrase"': (
        "CASE WHEN random() < 0.85 THEN '' "
        "ELSE 'phrase ' || (pow(random(),2)*20000)::INTEGER::VARCHAR END"
    ),
    # URL with realistic head/tail + google matches
    '"URL"': (
        "CASE WHEN random() < 0.3 "
        "THEN (['http://google.com/', 'http://mail.google.com/inbox', "
        "'http://example.com/', 'http://yandex.ru/news', 'http://shop.site/cart', "
        "'http://news.site/top', 'http://video.site/watch', 'http://maps.google.com/', "
        "'http://forum.site/thread', 'http://blog.site/post'])[(random()*9)::INTEGER + 1] "
        "ELSE 'http://' || "
        "(['google.com','mail.google.com','maps.google.com','example.com','yandex.ru',"
        "'news.site','shop.site','video.site','forum.site','blog.site'])[(random()*9)::INTEGER + 1] "
        "|| '/p' || (random()*1000000)::INTEGER::VARCHAR END"
    ),
}

# Build column list from the canonical schema (name, type) in declaration order.
schema = [
 ('WatchID','BIGINT'),('JavaEnable','SMALLINT'),('Title','TEXT'),('GoodEvent','SMALLINT'),
 ('EventTime','TIMESTAMP'),('EventDate','DATE'),('CounterID','INTEGER'),('ClientIP','INTEGER'),
 ('RegionID','INTEGER'),('UserID','BIGINT'),('CounterClass','SMALLINT'),('OS','SMALLINT'),
 ('UserAgent','SMALLINT'),('URL','TEXT'),('Referer','TEXT'),('IsRefresh','SMALLINT'),
 ('RefererCategoryID','SMALLINT'),('RefererRegionID','INTEGER'),('URLCategoryID','SMALLINT'),
 ('URLRegionID','INTEGER'),('ResolutionWidth','SMALLINT'),('ResolutionHeight','SMALLINT'),
 ('ResolutionDepth','SMALLINT'),('FlashMajor','SMALLINT'),('FlashMinor','SMALLINT'),
 ('FlashMinor2','TEXT'),('NetMajor','SMALLINT'),('NetMinor','SMALLINT'),('UserAgentMajor','SMALLINT'),
 ('UserAgentMinor','VARCHAR'),('CookieEnable','SMALLINT'),('JavascriptEnable','SMALLINT'),
 ('IsMobile','SMALLINT'),('MobilePhone','SMALLINT'),('MobilePhoneModel','TEXT'),('Params','TEXT'),
 ('IPNetworkID','INTEGER'),('TraficSourceID','SMALLINT'),('SearchEngineID','SMALLINT'),
 ('SearchPhrase','TEXT'),('AdvEngineID','SMALLINT'),('IsArtifical','SMALLINT'),
 ('WindowClientWidth','SMALLINT'),('WindowClientHeight','SMALLINT'),('ClientTimeZone','SMALLINT'),
 ('ClientEventTime','TIMESTAMP'),('SilverlightVersion1','SMALLINT'),('SilverlightVersion2','SMALLINT'),
 ('SilverlightVersion3','INTEGER'),('SilverlightVersion4','SMALLINT'),('PageCharset','TEXT'),
 ('CodeVersion','INTEGER'),('IsLink','SMALLINT'),('IsDownload','SMALLINT'),('IsNotBounce','SMALLINT'),
 ('FUniqID','BIGINT'),('OriginalURL','TEXT'),('HID','INTEGER'),('IsOldCounter','SMALLINT'),
 ('IsEvent','SMALLINT'),('IsParameter','SMALLINT'),('DontCountHits','SMALLINT'),('WithHash','SMALLINT'),
 ('HitColor','VARCHAR'),('LocalEventTime','TIMESTAMP'),('Age','SMALLINT'),('Sex','SMALLINT'),
 ('Income','SMALLINT'),('Interests','SMALLINT'),('Robotness','SMALLINT'),('RemoteIP','INTEGER'),
 ('WindowName','INTEGER'),('OpenerName','INTEGER'),('HistoryLength','SMALLINT'),('BrowserLanguage','TEXT'),
 ('BrowserCountry','TEXT'),('SocialNetwork','TEXT'),('SocialAction','TEXT'),('HTTPError','SMALLINT'),
 ('SendTiming','INTEGER'),('DNSTiming','INTEGER'),('ConnectTiming','INTEGER'),
 ('ResponseStartTiming','INTEGER'),('ResponseEndTiming','INTEGER'),('FetchTiming','INTEGER'),
 ('SocialSourceNetworkID','SMALLINT'),('SocialSourcePage','TEXT'),('ParamPrice','BIGINT'),
 ('ParamOrderID','TEXT'),('ParamCurrency','TEXT'),('ParamCurrencyID','SMALLINT'),
 ('OpenstatServiceName','TEXT'),('OpenstatCampaignID','TEXT'),('OpenstatAdID','TEXT'),
 ('OpenstatSourceID','TEXT'),('UTMSource','TEXT'),('UTMMedium','TEXT'),('UTMCampaign','TEXT'),
 ('UTMContent','TEXT'),('UTMTerm','TEXT'),('FromTag','TEXT'),('HasGCLID','SMALLINT'),
 ('RefererHash','BIGINT'),('URLHash','BIGINT'),('CLID','INTEGER'),
]

def default_expr(t):
    if t in ('TEXT','VARCHAR'):
        return "''"
    if t == 'BIGINT':
        return "(random()*1000000)::BIGINT"
    if t == 'DATE':
        return "DATE '2013-07-15'"
    if t == 'TIMESTAMP':
        return "TIMESTAMP '2013-07-15'"
    if t == 'INTEGER':
        return "(random()*1000)::INTEGER"
    # SMALLINT
    return "(random()*100)::SMALLINT"

cols = []
for name, t in schema:
    key = f'"{name}"'
    e = expr.get(key, default_expr(t))
    cols.append(f'{e} AS "{name}"')

sql = (
    "COPY (SELECT\n  " + ",\n  ".join(cols) +
    f"\nFROM range({rows})) TO '{out}' (FORMAT parquet, ROW_GROUP_SIZE {rgs}, COMPRESSION snappy)"
)

con = duckdb.connect()
con.execute(f"PRAGMA threads={threads}")
con.execute("PRAGMA enable_progress_bar")
print(f"Generating {rows:,} rows -> {out} (threads={threads}, row_group_size={rgs})", flush=True)
con.execute(sql)
n = con.execute(f"SELECT count(*) FROM '{out}'").fetchone()[0]
print(f"Done: {n:,} rows", flush=True)
