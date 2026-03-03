/*
 * Copyright (C) 2025 Davide Fuscà
 *
 * This file is part of BumbleBee.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "bumblebee/common/StringUtils.hpp"

#include <sstream>
#include <uuid/uuid.h>

namespace bumblebee{

vector<string> StringUtils::split(const string &str, char delimiter) {
    std::stringstream ss(str);
    vector<string> lines;
    string temp;
    while (getline(ss, temp, delimiter)) {
        lines.push_back(temp);
    }
    return lines;
}

vector<string> StringUtils::split(const string &input, const string &split) {
    vector<string> splits;

    idx_t last = 0;
    idx_t input_len = input.size();
    idx_t split_len = split.size();
    while (last <= input_len) {
        idx_t next = input.find(split, last);
        if (next == string::npos) {
            next = input_len;
        }

        // Push the substring [last, next) on to splits
        string substr = input.substr(last, next - last);
        if (substr.empty() == false) {
            splits.push_back(substr);
        }
        last = next + split_len;
    }
    return splits;
}

bool StringUtils::contains(const string &input, const string &split) {
    return (input.find(split) != string::npos);
}

bool StringUtils::glob(const string &str, const string &pattern) {
	return glob(str.c_str(), str.size(), pattern.c_str(), pattern.size());
}

bool StringUtils::hasGlob(const string &str) {
	for (idx_t i = 0; i < str.size(); i++) {
		switch (str[i]) {
			case '*':
			case '?':
			case '[':
				return true;
			default:
				break;
		}
	}
	return false;
}

bool StringUtils::glob(const char *string, idx_t slen, const char *pattern, idx_t plen) {
	idx_t sidx = 0;
	idx_t pidx = 0;
main_loop : {
	// main matching loop
	while (sidx < slen && pidx < plen) {
		char s = string[sidx];
		char p = pattern[pidx];
		switch (p) {
		case '*': {
			// asterisk: match any set of characters
			// skip any subsequent asterisks
			pidx++;
			while (pidx < plen && pattern[pidx] == '*') {
				pidx++;
			}
			// if the asterisk is the last character, the pattern always matches
			if (pidx == plen) {
				return true;
			}
			// recursively match the remainder of the pattern
			for (; sidx < slen; sidx++) {
				if (StringUtils::glob(string + sidx, slen - sidx, pattern + pidx, plen - pidx)) {
					return true;
				}
			}
			return false;
		}
		case '?':
			// wildcard: matches anything but null
			break;
		case '[':
			pidx++;
			goto parse_bracket;
		case '\\':
			// escape character, next character needs to match literally
			pidx++;
			// check that we still have a character remaining
			if (pidx == plen) {
				return false;
			}
			p = pattern[pidx];
			if (s != p) {
				return false;
			}
			break;
		default:
			// not a control character: characters need to match literally
			if (s != p) {
				return false;
			}
			break;
		}
		sidx++;
		pidx++;
	}
	while (pidx < plen && pattern[pidx] == '*') {
		pidx++;
	}
	// we are finished only if we have consumed the full pattern
	return pidx == plen && sidx == slen;
}
parse_bracket : {
	// inside a bracket
	if (pidx == plen) {
		return false;
	}
	// check the first character
	// if it is an exclamation mark we need to invert our logic
	char p = pattern[pidx];
	char s = string[sidx];
	bool invert = false;
	if (p == '!') {
		invert = true;
		pidx++;
	}
	bool found_match = invert;
	idx_t start_pos = pidx;
	bool found_closing_bracket = false;
	// now check the remainder of the pattern
	while (pidx < plen) {
		p = pattern[pidx];
		// if the first character is a closing bracket, we match it literally
		// otherwise it indicates an end of bracket
		if (p == ']' && pidx > start_pos) {
			// end of bracket found: we are done
			found_closing_bracket = true;
			pidx++;
			break;
		}
		// we either match a range (a-b) or a single character (a)
		// check if the next character is a dash
		if (pidx + 1 == plen) {
			// no next character!
			break;
		}
		bool matches;
		if (pattern[pidx + 1] == '-') {
			// range! find the next character in the range
			if (pidx + 2 == plen) {
				break;
			}
			char next_char = pattern[pidx + 2];
			// check if the current character is within the range
			matches = s >= p && s <= next_char;
			// shift the pattern forward past the range
			pidx += 3;
		} else {
			// no range! perform a direct match
			matches = p == s;
			// shift the pattern forward past the character
			pidx++;
		}
		if (found_match == invert && matches) {
			// found a match! set the found_matches flag
			// we keep on pattern matching after this until we reach the end bracket
			// however, we don't need to update the found_match flag anymore
			found_match = !invert;
		}
	}
	if (!found_closing_bracket) {
		// no end of bracket: invalid pattern
		return false;
	}
	if (!found_match) {
		// did not match the bracket: return false;
		return false;
	}
	// finished the bracket matching: move forward
	sidx++;
	goto main_loop;
}
}


bool StringUtils::characterIsSpace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

bool StringUtils::characterIsDigit(char c) {
	return c >= '0' && c <= '9';
}

bool StringUtils::characterIsNewline(char c) {
	return c == '\n' || c == '\r';
}

string StringUtils::upper(const string &str) {
	string copy(str);
	transform(copy.begin(), copy.end(), copy.begin(), [](unsigned char c) { return std::toupper(c); });
	return (copy);
}

string StringUtils::lower(const string &str) {
	string copy(str);
	transform(copy.begin(), copy.end(), copy.begin(), [](unsigned char c) { return std::tolower(c); });
	return (copy);
}

void StringUtils::removeQuote(string &str) {
	if (str.back() == '"' || str.back() == '\'')
		str.pop_back();
	if (str.front() == '"' || str.front() == '\'')
		str.erase(0,1);
}

bool StringUtils::startsWith(string str, string prefix) {
	if (prefix.size() > str.size()) {
		return false;
	}
	return equal(prefix.begin(), prefix.end(), str.begin());
}

string StringUtils::trim(const string& str) {
	if (str.empty()) {
		return str;
	}

	idx_t start = 0;
	idx_t end = str.size() - 1;

	// Move start forward while characters are whitespace
	while (start < str.size() && characterIsSpace(str[start])) {
		start++;
	}

	// Move end backward while characters are whitespace
	while (end > start && characterIsSpace(str[end])) {
		end--;
	}

	// Extract the trimmed substring
	return str.substr(start, end - start + 1);
}


// Helper function for UTF-8 aware space trimming
string trimWhitespace(const string &col_name) {
	utf8proc_int32_t codepoint;
	auto str = reinterpret_cast<const utf8proc_uint8_t *>(col_name.c_str());
	idx_t size = col_name.size();
	// Find the first character that is not left trimmed
	idx_t begin = 0;
	while (begin < size) {
		auto bytes = utf8proc_iterate(str + begin, size - begin, &codepoint);
		BB_ASSERT(bytes > 0);
		if (utf8proc_category(codepoint) != UTF8PROC_CATEGORY_ZS) {
			break;
		}
		begin += bytes;
	}

	// Find the last character that is not right trimmed
	idx_t end;
	end = begin;
	for (auto next = begin; next < col_name.size();) {
		auto bytes = utf8proc_iterate(str + next, size - next, &codepoint);
		BB_ASSERT(bytes > 0);
		next += bytes;
		if (utf8proc_category(codepoint) != UTF8PROC_CATEGORY_ZS) {
			end = next;
		}
	}

	// return the trimmed string
	return col_name.substr(begin, end - begin);
}

string StringUtils::normalizeColumnName(const string &col_name) {
	// normalized columns names: UPPERCASE and without " " ("col 1" -> "COL_1")
	// normalize UTF8 characters to NFKD
	auto nfkd = utf8proc_NFKD((const utf8proc_uint8_t *)col_name.c_str(), col_name.size());
	const string col_name_nfkd = string((const char *)nfkd, strlen((const char *)nfkd));
	free(nfkd);

	// only keep ASCII characters 0-9 a-z A-Z and replace spaces with regular whitespace
	string col_name_ascii = "";
	for (idx_t i = 0; i < col_name_nfkd.size(); i++) {
		if (col_name_nfkd[i] == '_' || (col_name_nfkd[i] >= '0' && col_name_nfkd[i] <= '9') ||
		    (col_name_nfkd[i] >= 'A' && col_name_nfkd[i] <= 'Z') ||
		    (col_name_nfkd[i] >= 'a' && col_name_nfkd[i] <= 'z')) {
			col_name_ascii += col_name_nfkd[i];
		} else if (StringUtils::characterIsSpace(col_name_nfkd[i])) {
			col_name_ascii += " ";
		}
	}

	// trim whitespace and replace remaining whitespace by _
	string col_name_trimmed = trimWhitespace(col_name_ascii);
	string col_name_cleaned = "";
	bool in_whitespace = false;
	for (idx_t i = 0; i < col_name_trimmed.size(); i++) {
		if (col_name_trimmed[i] == ' ') {
			if (!in_whitespace) {
				col_name_cleaned += "_";
				in_whitespace = true;
			}
		} else {
			col_name_cleaned += col_name_trimmed[i];
			in_whitespace = false;
		}
	}

	// if does not start with letter then append a prefix
	if (col_name_cleaned[0] == '_' || (col_name_cleaned[0] >= '0' && col_name_cleaned[0] <= '9')) {
		col_name_cleaned = "COLUMN_" + col_name_cleaned;
	}

	// don't leave string empty; if not empty, make lowercase
	if (col_name_cleaned.empty()) {
		col_name_cleaned = "_";
	}
	// make upper case
	col_name_cleaned = StringUtils::upper(col_name_cleaned);

	return col_name_cleaned;
}

char StringUtils::characterToLower(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c - ('A' - 'a');
	}
	return c;
}

std::unordered_map<string,string> StringUtils::parseColMapping(const string &columns, const vector<string> &varNames) {
	std::unordered_map<string,string> realColumnNames;

	if (columns.empty()) {
		for (auto &var: varNames)
			realColumnNames[var] = var;
		return realColumnNames;
	}

	std::stringstream ss(columns);
	string token;

	while (getline(ss, token, ';')) { // Split by ';'
		if (token.empty()) continue;

		auto pos = token.find(':');
		if (pos != string::npos) {
			string varName = token.substr(0, pos);
			string realName = token.substr(pos + 1);

			varName = trim(varName);
			realName = trim(realName);

			realColumnNames[varName] = realName;
		}
	}
	for (auto& var : varNames) {
		// skip anonymous variables (underscore)
		if (var == "_") continue;
		if (!realColumnNames.contains(var))
			ErrorHandler::errorParsing("Error variable "+var+" not specified in the column mapping.");
	}
	return realColumnNames;
}


string StringUtils::getUUID() {
	uuid_t uuid;
	uuid_generate(uuid); // Generate UUID

	char uuid_str[37]; // 36 chars + null terminator
	uuid_unparse(uuid, uuid_str);

	string uuid_string(uuid_str);
	return uuid_string;
}

}
