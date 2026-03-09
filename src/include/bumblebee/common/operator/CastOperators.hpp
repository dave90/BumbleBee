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
#pragma once
#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/common/Limits.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include <type_traits>
#include <limits>
#include <cmath>

#include "bumblebee/common/NumericUtils.hpp"
#include "bumblebee/common/types/Date.hpp"

namespace bumblebee{


//===--------------------------------------------------------------------===//
// Numeric -> Numeric Cast Operations
//===--------------------------------------------------------------------===//
struct Cast {
private:
    template <class T>
    static constexpr bool is_arith_v = std::is_arithmetic_v<T>;

    template <class T>
    static constexpr bool is_int_v = std::is_integral_v<T>;

    template <class T>
    static constexpr bool is_float_v = std::is_floating_point_v<T>;


public:
    // Generic arithmetic → arithmetic cast with safety checks.
    template <class SRC, class DST>
    static inline DST operation(SRC input) {
        if constexpr (is_arith_v<SRC> && is_arith_v<DST>) {
            return static_cast<DST>(input);
        }
        // Non-arithmetic (e.g., string) lands here
        ErrorHandler::errorNotImplemented("Unimplemented type for cast :(");
        return static_cast<DST>(input); // never returned
    }
};

struct NumericTryCast {
    template <class SRC, class DST>
    static inline bool operation(SRC& value, DST& result) {
        if (NumericLimits<SRC>::isSigned() != NumericLimits<DST>::isSigned()) {
            if (NumericLimits<SRC>::isSigned()) {
                // signed to unsigned conversion
                if (NumericLimits<SRC>::digits() > NumericLimits<DST>::digits()) {
                    if (value < 0 || value > (SRC)NumericLimits<DST>::maximum()) {
                        return false;
                    }
                } else {
                    if (value < 0) {
                        return false;
                    }
                }
                result = (DST)value;
                return true;
            } else {
                // unsigned to signed conversion
                if (NumericLimits<SRC>::digits() >= NumericLimits<DST>::digits()) {
                    if (value <= (SRC)NumericLimits<DST>::maximum()) {
                        result = (DST)value;
                        return true;
                    }
                    return false;
                } else {
                    result = (DST)value;
                    return true;
                }
            }
        } else {
            // same sign conversion
            if (NumericLimits<DST>::digits() >= NumericLimits<SRC>::digits()) {
                result = (DST)value;
                return true;
            } else {
                if (value < SRC(NumericLimits<DST>::minimum()) || value > SRC(NumericLimits<DST>::maximum())) {
                    return false;
                }
                result = (DST)value;
                return true;
            }
        }
    }
};

//===--------------------------------------------------------------------===//
// Cast String -> Numeric
//===--------------------------------------------------------------------===//

struct IntegerCastOperation {
    template <class T, bool NEGATIVE>
    static bool handleDigit(T &result, uint8_t digit) {
        if (NEGATIVE) {
            if (result < (NumericLimits<T>::minimum() + digit) / 10) {
                return false;
            }
            result = result * 10 - digit;
        } else {
            if (result > (NumericLimits<T>::maximum() - digit) / 10) {
                return false;
            }
            result = result * 10 + digit;
        }
        return true;
    }

    template <class T, bool NEGATIVE>
    static bool handleExponent(T &result, int64_t exponent) {
        double dbl_res = result * std::pow(10.0L, exponent);
        if (dbl_res < NumericLimits<T>::minimum() || dbl_res > NumericLimits<T>::maximum()) {
            return false;
        }
        result = (T)dbl_res;
        return true;
    }

    template <class T, bool NEGATIVE>
    static bool handleDecimal(T &result, uint8_t digit) {
        return true;
    }

    template <class T>
    static bool finalize(T &result) {
        return true;
    }
};



template <class T, bool NEGATIVE, bool ALLOW_EXPONENT, class OP = IntegerCastOperation>
static bool integerCastLoop(const char *buf, idx_t len, T &result) {
	idx_t start_pos = NEGATIVE || *buf == '+' ? 1 : 0;
	idx_t pos = start_pos;
	while (pos < len) {
		if (!StringUtils::characterIsDigit(buf[pos])) {
			// not a digit!
			if (buf[pos] == '.') {
			    // is not an integer
                return false;
			    // uncomment if we want to cast removing the decimal part
				// bool number_before_period = pos > start_pos;
				// // decimal point: we accept decimal values for integers as well
				// // we just truncate them
				// // make sure everything after the period is a number
				// pos++;
				// idx_t start_digit = pos;
				// while (pos < len) {
				// 	if (!StringUtils::characterIsDigit(buf[pos])) {
				// 		break;
				// 	}
				// 	if (!OP::template handleDecimal<T, NEGATIVE>(result, buf[pos] - '0')) {
				// 		return false;
				// 	}
				// 	pos++;
				// }
				// // make sure there is either (1) one number after the period, or (2) one number before the period
				// // i.e. we accept "1." and ".1" as valid numbers, but not "."
				// if (!(number_before_period || pos > start_digit)) {
				// 	return false;
				// }
				// if (pos >= len) {
				// 	break;
				// }
			}
			if (StringUtils::characterIsSpace(buf[pos])) {
				// skip any trailing spaces
				while (++pos < len) {
					if (!StringUtils::characterIsSpace(buf[pos])) {
						return false;
					}
				}
				break;
			}
			if (ALLOW_EXPONENT) {
				if (buf[pos] == 'e' || buf[pos] == 'E') {
					if (pos == start_pos) {
						return false;
					}
					pos++;
					if (pos >= len) {
						return false;
					}
					int64_t exponent = 0;
					int negative = buf[pos] == '-';
					if (negative) {
						if (!integerCastLoop<int64_t, true, false>(buf + pos, len - pos, exponent)) {
							return false;
						}
					} else {
						if (!integerCastLoop<int64_t, false, false>(buf + pos, len - pos, exponent)) {
							return false;
						}
					}
					return OP::template handleExponent<T, NEGATIVE>(result, exponent);
				}
			}
			return false;
		}
		uint8_t digit = buf[pos++] - '0';
		if (!OP::template handleDigit<T, NEGATIVE>(result, digit)) {
			return false;
		}
	}
	if (!OP::template finalize<T>(result)) {
		return false;
	}
	return pos > start_pos;
}

struct TryIntegerCast {

	// Define the INPUT_TYPE parameter so it can be reused in the cast loop (cast functions usually take two parameters)
	template <class string_t, class T, bool IS_SIGNED = true, bool ALLOW_EXPONENT = true, class OP = IntegerCastOperation, bool ZERO_INITIALIZE = true>
	static inline bool operation(string_t& val, T &result) {
		const char *buf = val.getDataUnsafe();
		idx_t len = val.length();
		auto res = TryIntegerCast::operation<string_t, T, IS_SIGNED, ALLOW_EXPONENT, OP, ZERO_INITIALIZE>(buf, len, result);
		return res;
	}

    // Define the INPUT_TYPE parameter so it can be reused in the cast loop (cast functions usually take two parameters)
    template <class string_t, class T, bool IS_SIGNED = true, bool ALLOW_EXPONENT = true, class OP = IntegerCastOperation, bool ZERO_INITIALIZE = true>
    static inline bool operation(const char *buf, idx_t len, T &result) {
        // skip any spaces at the start
        while (len > 0 && StringUtils::characterIsSpace(*buf)) {
            buf++;
            len--;
        }
        if (len == 0) {
            return false;
        }
        int negative = *buf == '-';

        if (ZERO_INITIALIZE) {
            memset(&result, 0, sizeof(T));
        }
        if (!negative) {
            return integerCastLoop<T, false, ALLOW_EXPONENT, OP>(buf, len, result);
        } else {
            if (!IS_SIGNED) {
                // Need to check if its not -0
                idx_t pos = 1;
                while (pos < len) {
                    if (buf[pos++] != '0') {
                        return false;
                    }
                }
            }
            return integerCastLoop<T, true, ALLOW_EXPONENT, OP>(buf, len, result);
        }
    }
};


template <class T, bool NEGATIVE>
static void computeDoubleResult(T &result, idx_t decimal, idx_t decimal_factor) {
	if (decimal_factor > 1) {
		if (NEGATIVE) {
			result -= (T)decimal / (T)decimal_factor;
		} else {
			result += (T)decimal / (T)decimal_factor;
		}
	}
}

template <class T, bool NEGATIVE>
static bool doubleCastLoop(const char *buf, idx_t len, T &result) {
	idx_t start_pos = NEGATIVE || *buf == '+' ? 1 : 0;
	idx_t pos = start_pos;
	idx_t decimal = 0;
	idx_t decimal_factor = 0;
	while (pos < len) {
		if (!StringUtils::characterIsDigit(buf[pos])) {
			// not a digit!
			if (buf[pos] == '.') {
				// decimal point
				if (decimal_factor != 0) {
					// nested periods
					return false;
				}
				decimal_factor = 1;
				pos++;
				continue;
			} else if (StringUtils::characterIsSpace(buf[pos])) {
				// skip any trailing spaces
				while (++pos < len) {
					if (!StringUtils::characterIsSpace(buf[pos])) {
						return false;
					}
				}
				computeDoubleResult<T, NEGATIVE>(result, decimal, decimal_factor);
				return true;
			} else if (buf[pos] == 'e' || buf[pos] == 'E') {
				if (pos == start_pos) {
					return false;
				}
				// E power
				// parse an integer, this time not allowing another exponent
				pos++;
				int64_t exponent;
				if (!TryIntegerCast::operation<string_t, int64_t, true, false>(buf + pos, len - pos, exponent)) {
					return false;
				}
				computeDoubleResult<T, NEGATIVE>(result, decimal, decimal_factor);
				if (result > NumericLimits<T>::maximum() / std::pow(10.0L, exponent)) {
					return false;
				}
				result = result * std::pow(10.0L, exponent);

				return true;
			} else {
				return false;
			}
		}
		T digit = buf[pos++] - '0';
		if (decimal_factor == 0) {
			result = result * 10 + (NEGATIVE ? -digit : digit);
		} else {
			if (decimal_factor >= 1000000000000000000) {
				// decimal value will overflow if we parse more, ignore any subsequent numbers
				continue;
			}
			decimal = decimal * 10 + digit;
			decimal_factor *= 10;
		}
	}
	computeDoubleResult<T, NEGATIVE>(result, decimal, decimal_factor);
	return pos > start_pos;
}

template <class T>
bool checkDoubleValidity(T value);

template <>
bool checkDoubleValidity(float value) {
	return !(std::isnan(value) || std::isinf(value));
}

template <>
bool checkDoubleValidity(double value) {
	return !(std::isnan(value) || std::isinf(value));
}

struct TryDoubleCast{

	template <class string_t, class T>
	static bool operation(string_t& val, T &result) {
		const char *buf = val.getDataUnsafe();
		idx_t len = val.length();

	    // skip any spaces at the start
	    while (len > 0 && StringUtils::characterIsSpace(*buf)) {
	        buf++;
	        len--;
	    }
	    if (len == 0) {
	        return false;
	    }
	    int negative = *buf == '-';

	    result = 0;
	    if (!negative) {
	        if (!doubleCastLoop<T, false>(buf, len, result)) {
	            return false;
	        }
	    } else {
	        if (!doubleCastLoop<T, true>(buf, len, result)) {
	            return false;
	        }
	    }
	    if (!checkDoubleValidity<T>(result)) {
	        return false;
	    }
	    return true;
	}
};

//===--------------------------------------------------------------------===//
// Cast Numeric -> String
//===--------------------------------------------------------------------===//


struct StringCast {
    template <class T>
    static inline string_t operation(T value, Vector& vector) {
        ErrorHandler::errorNotImplemented("Unimplemented type for string cast!");
    }
};

struct StringTryCast {
	template <class T>
	static inline string_t operation(T value, Vector& vector) {
		if (NumericLimits<T>::maximum() == value) return string_t("");
		return StringCast::operation(value, vector);
	}
};


template<>
string_t inline StringCast::operation(uint8_t value, Vector& vector) {
    return NumericHelper::formatSigned<uint8_t,uint8_t>(value, vector);
}


template<>
string_t inline StringCast::operation(uint16_t value, Vector& vector) {
    return NumericHelper::formatSigned<uint16_t,uint16_t>(value, vector);
}

template<>
string_t inline StringCast::operation(uint32_t value, Vector& vector) {
    return NumericHelper::formatSigned<uint32_t,uint32_t>(value, vector);
}


template<>
string_t inline StringCast::operation(uint64_t value, Vector& vector) {
    return NumericHelper::formatSigned<uint64_t,uint64_t>(value, vector);
}


template<>
string_t inline StringCast::operation(int8_t value, Vector& vector) {
    return NumericHelper::formatSigned<int8_t,uint8_t>(value, vector);
}

template<>
string_t inline StringCast::operation(int16_t value, Vector& vector) {
    return NumericHelper::formatSigned<int16_t,uint16_t>(value, vector);
}

template<>
string_t inline StringCast::operation(int32_t value, Vector& vector) {
    return NumericHelper::formatSigned<int32_t,uint32_t>(value, vector);
}

template<>
string_t inline StringCast::operation(int64_t value, Vector& vector) {
    return NumericHelper::formatSigned<int64_t,uint64_t>(value, vector);
}


template<>
string_t inline StringCast::operation(float value, Vector& vector) {
    string s = std::to_string(value);
    return StringVector::addString(vector, s);
}

template<>
string_t inline StringCast::operation(double value, Vector& vector) {
    string s = std::to_string(value);
    return StringVector::addString(vector, s);
}


struct StringCastFromDecimal {
	template <class SRC>
	static inline string_t operation(SRC input, uint8_t width, uint8_t scale, Vector &result) {
        ErrorHandler::errorNotImplemented("Unimplemented type for StringCastFromDecimal cast!");
		return {};
	}
};

struct StringTryCastFromDecimal {
	template <class SRC>
	static inline string_t operation(SRC input, uint8_t width, uint8_t scale, Vector &result) {
		if (NumericLimits<SRC>::maximum() == input) return string_t("");
		return StringCastFromDecimal::operation(input, width, scale, result);
	}
};


template <>
inline string_t StringCastFromDecimal::operation(int16_t input, uint8_t width, uint8_t scale, Vector &result) {
	return DecimalToString::format<int16_t, uint16_t>(input, scale, result);
}
template <>
inline string_t StringCastFromDecimal::operation(int32_t input, uint8_t width, uint8_t scale, Vector &result) {
	return DecimalToString::format<int32_t, uint32_t>(input, scale, result);
}
template <>
inline string_t StringCastFromDecimal::operation(int64_t input, uint8_t width, uint8_t scale, Vector &result) {
	return DecimalToString::format<int64_t, uint64_t>(input, scale, result);
}

struct StringCastFromDate {
	template <class SRC>
	static inline string_t operation(SRC input, Vector &result) {
		ErrorHandler::errorNotImplemented("Unimplemented type for StringCastFromDate cast!");
		return {};
	}
};

template <>
inline string_t StringCastFromDate::operation(date_t input, Vector &result) {
	if (NumericLimits<date_t>::maximum() == input) return string_t("");

	int32_t date[3];
	Date::convert(input, date[0], date[1], date[2]);

	idx_t year_length;
	bool add_bc;
	idx_t length = Date::length(date, year_length, add_bc);

	string_t resultString = StringVector::emptyString(result, length);
	auto data = resultString.getDataWriteable();

	Date::format(data, date, year_length, add_bc);

	return resultString;
}


struct StringCastFromTimestamp {
template <class SRC>
static inline string_t operation(SRC input, Vector &result) {
	ErrorHandler::errorNotImplemented("Unimplemented type for StringCastFromDate cast!");
	return {};
}
};

template <>
inline string_t StringCastFromTimestamp::operation(timestamp_t input, Vector &vector) {
	if (NumericLimits<timestamp_t>::maximum() == input) return string_t("");

	int32_t date_entry;
	int64_t time_entry;
	Timestamp::convert(input, date_entry, time_entry);

	int32_t date[3], time[4];
	Date::convert(date_entry, date[0], date[1], date[2]);
	Timestamp::convert(time_entry, time[0], time[1], time[2], time[3]);

	// format for timestamp is DATE TIME (separated by space)
	idx_t year_length;
	bool add_bc;
	char micro_buffer[6];
	idx_t date_length = Date::length(date, year_length, add_bc);
	idx_t time_length = Timestamp::length(time, micro_buffer);
	idx_t length = date_length + time_length + 1;

	string_t result = StringVector::emptyString(vector, length);
	auto data = result.getDataWriteable();

	Date::format(data, date, year_length, add_bc);
	data[date_length] = ' ';
	Timestamp::format(data + date_length + 1, time_length, time, micro_buffer);

	return result;
}

}
