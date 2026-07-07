#include "utf8proc_wrapper.hpp"
#include "utf8proc.hpp"

using namespace std;

namespace bumblebee {

// This function efficiently checks if a string is valid UTF8.
// It was originally written by Sjoerd Mullender.

// Here is the table that makes it work:

// B 		= Number of Bytes in UTF8 encoding
// C_MIN 	= First Unicode code point
// C_MAX 	= Last Unicode code point
// B1 		= First Byte Prefix

// 	B	C_MIN		C_MAX		B1
//	1	U+000000	U+00007F		0xxxxxxx
//	2	U+000080	U+0007FF		110xxxxx
//	3	U+000800	U+00FFFF		1110xxxx
//	4	U+010000	U+10FFFF		11110xxx

static void AssignInvalidUTF8Reason(UnicodeInvalidReason *invalid_reason, size_t *invalid_pos, size_t pos, UnicodeInvalidReason reason) {
	if (invalid_reason) {
		*invalid_reason = reason;
	}
	if (invalid_pos) {
		*invalid_pos = pos;
	}
}

UnicodeType Utf8Proc::Analyze(const char *s, size_t len, UnicodeInvalidReason *invalid_reason, size_t *invalid_pos) {
	UnicodeType type = UnicodeType::ASCII;
	// Word-at-a-time fast path: most strings are pure ASCII, so validate 8 bytes
	// per step. A word is a run of plain ASCII iff no byte has the high bit set and
	// no byte is NUL. The SWAR NUL test is the classic (w - 0x01..) & ~w & 0x80..
	// trick. Any word failing either test falls through to the byte-precise loop
	// below (which also handles all multi-byte sequences), so behaviour - including
	// the exact invalid reason and position - is identical to the scalar scan.
	constexpr uint64_t HIGH_BITS = 0x8080808080808080ULL;
	constexpr uint64_t LOW_ONES = 0x0101010101010101ULL;
	size_t i = 0;
	for (;;) {
		while (i + 8 <= len) {
			uint64_t w;
			memcpy(&w, s + i, 8);
			if ((w & HIGH_BITS) || ((((w - LOW_ONES) & ~w) & HIGH_BITS))) {
				break;
			}
			i += 8;
		}
		if (i >= len) {
			break;
		}
		char c = s[i];
		if (c == '\0') {
			AssignInvalidUTF8Reason(invalid_reason, invalid_pos, i, UnicodeInvalidReason::NULL_BYTE);
			return UnicodeType::INVALID;
		}
		// 1 Byte / ASCII
		if ((c & 0x80) == 0) {
			i++;
			continue;
		}
		type = UnicodeType::UNICODE;
		if ((s[++i] & 0xC0) != 0x80) {
			AssignInvalidUTF8Reason(invalid_reason, invalid_pos, i, UnicodeInvalidReason::BYTE_MISMATCH);
			return UnicodeType::INVALID;
		}
		if ((c & 0xE0) == 0xC0) {
			i++;
			continue;
		}
		if ((s[++i] & 0xC0) != 0x80) {
			AssignInvalidUTF8Reason(invalid_reason, invalid_pos, i, UnicodeInvalidReason::BYTE_MISMATCH);
			return UnicodeType::INVALID;
		}
		if ((c & 0xF0) == 0xE0) {
			i++;
			continue;
		}
		if ((s[++i] & 0xC0) != 0x80) {
			AssignInvalidUTF8Reason(invalid_reason, invalid_pos, i, UnicodeInvalidReason::BYTE_MISMATCH);
			return UnicodeType::INVALID;
		}
		if ((c & 0xF8) == 0xF0) {
			i++;
			continue;
		}
		AssignInvalidUTF8Reason(invalid_reason, invalid_pos, i, UnicodeInvalidReason::BYTE_MISMATCH);
		return UnicodeType::INVALID;
	}

	return type;
}


char* Utf8Proc::Normalize(const char *s, size_t len) {
	assert(s);
	assert(Utf8Proc::Analyze(s, len) != UnicodeType::INVALID);
	return (char*) utf8proc_NFC((const utf8proc_uint8_t*) s, len);
}

bool Utf8Proc::IsValid(const char *s, size_t len) {
	return Utf8Proc::Analyze(s, len) != UnicodeType::INVALID;
}

size_t Utf8Proc::NextGraphemeCluster(const char *s, size_t len, size_t cpos) {
	return utf8proc_next_grapheme(s, len, cpos);
}

size_t Utf8Proc::PreviousGraphemeCluster(const char *s, size_t len, size_t cpos) {
	if (!Utf8Proc::IsValid(s, len)) {
		return cpos - 1;
	}
	size_t current_pos = 0;
	while(true) {
		size_t new_pos = NextGraphemeCluster(s, len, current_pos);
		if (new_pos <= current_pos || new_pos >= cpos) {
			return current_pos;
		}
		current_pos = new_pos;
	}
}

bool Utf8Proc::CodepointToUtf8(int cp, int &sz, char *c) {
	return utf8proc_codepoint_to_utf8(cp, sz, c);
}

int Utf8Proc::CodepointLength(int cp) {
	return utf8proc_codepoint_length(cp);
}

int32_t Utf8Proc::UTF8ToCodepoint(const char *c, int &sz) {
	return utf8proc_codepoint(c, sz);
}

size_t Utf8Proc::RenderWidth(const char *s, size_t len, size_t pos) {
    int sz;
    auto codepoint = bumblebee::utf8proc_codepoint(s + pos, sz);
    auto properties = bumblebee::utf8proc_get_property(codepoint);
    return properties->charwidth;
}

}
