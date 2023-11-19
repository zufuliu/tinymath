// cl /c /EHsc /std:c++20 /W4 /DNDEBUG /MD /Ox /Ot /GS- /GR- /FAcs eval.cpp
// clang-cl /c /EHsc /std:c++20 /W4 /DNDEBUG /MD /Ox /Ot /GS- /GR- /FAcs eval.cpp
// g++ -S -std=gnu++20 -DNDEBUG -O3 -fno-rtti -Wall -Wextra eval.cpp
// clang++ -S -std=gnu++20 -DNDEBUG -O3 -fno-rtti -Wall -Wextra eval.cpp
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "eval.h"

namespace eval {

// https://en.cppreference.com/w/cpp/language/operator_precedence
// https://docs.python.org/3/reference/expressions.html#operator-precedence
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Operator_precedence
enum class Operator : uint8_t {
	None,
	Add,			// +a, a + b
	Subtract,		// -a, a - b
	Multiply,		// a * b
	Divide,			// a / b
	FloorDivide,	// a // b
	Remainder,		// a % b
	Exponent,		// a ** b
	LogicalNot,		// prefix logical not: !a, postfix factorial: a!
	BitwiseNot,		// ~a
	LeftShift,		// a << b
	RightShift,		// a >> b
	// UnsignedRightShift,// a >>> b
	LessThan,		// a < b
	LessEqual,		// a <= b
	GreaterThan,	// a > b
	GreaterEqual,	// a >= b
	Equal,			// a == b
	Unequal,		// a != b
	BitwiseAnd,		// a & b
	BitwiseXor,		// a ^ b
	BitwiseOr,		// a | b
	LogicalAnd,		// a && b
	LogicalOr,		// a || b
	Coalesce,		// a ?: b, a ?? b
	Ternary,		// a ? b : c
};

constexpr bool IsSpace(uint8_t ch) noexcept {
	return ch <= ' ';
}
constexpr bool IsFuncNameStart(uint8_t ch) noexcept {
	return (ch >= 'a' && ch <= 'z');
}
constexpr bool IsFuncNameChar(uint8_t ch) noexcept {
	return (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9');
}

using FuncPtrRaw = void (__cdecl *)(void);
using FuncPtrD_D = double (__cdecl *)(double x);
using FuncPtrDD_D = double (__cdecl *)(double x, double y);
// TODO: use __builtin_bit_cast()
FuncPtrRaw FromFuncD_D(FuncPtrD_D func) noexcept {
	return reinterpret_cast<FuncPtrRaw>(func);
}
FuncPtrRaw FromFuncDD_D(FuncPtrDD_D func) noexcept {
	return reinterpret_cast<FuncPtrRaw>(func);
}

enum class Signature : uint8_t {
	None,
	D_D,
	DD_D,
};

struct FunctionInfo {
	unsigned len;
	Signature signature;
	const char *name;
	FuncPtrRaw func;
};

constexpr unsigned maxFuncNameLen = 5;
constexpr double MATH_E = 2.71828182845904523536;
constexpr double MATH_PI = 3.14159265358979323846;
#define MakeD_D(name, func)		{sizeof(name) - 1, Signature::D_D, name, FromFuncD_D(func)}
#define MakeDD_D(name, func)	{sizeof(name) - 1, Signature::DD_D, name, FromFuncDD_D(func)}
// https://en.cppreference.com/w/cpp/numeric/math
// https://docs.python.org/3/library/math.html
const FunctionInfo functionList[] = {
	MakeD_D("abs", fabs),
	MakeD_D("ceil", ceil),
	MakeD_D("floor", floor),
	MakeD_D("round", round),
	MakeD_D("trunc", trunc),
	MakeDD_D("min", fmin),
	MakeDD_D("max", fmax),

	MakeD_D("cbrt", cbrt),
	MakeD_D("exp", exp),
	MakeD_D("exp2", exp2),
	MakeDD_D("hypot", hypot),
	MakeD_D("log", log),
	MakeD_D("log10", log10),
	MakeD_D("log2", log2),
	MakeDD_D("pow", pow),
	MakeD_D("sqrt", sqrt),

	MakeD_D("acos", acos),
	MakeD_D("asin", asin),
	MakeD_D("atan", atan),
	MakeDD_D("atan2", atan2),
	MakeD_D("cos", cos),
	MakeD_D("sin", sin),
	MakeD_D("tan", tan),
};

double Context::Evaluate(const char *input, Precedence parent) noexcept {
	double result = 0;
	bool has_value = false;
	while (*input) {
		const uint8_t ch = *input;
		if (IsSpace(ch)) {
			++input;
		} else if ((ch >= '0' && ch <= '9') || ch == '.') {
			if (has_value) {
				has_value = false;
				break;
			}
			if (ch == '0') {
				int radix = uint8_t(input[1]) | 0x20;
				if (radix == 'x') {
					radix = 16;
				} else if (radix == 'b') {
					radix = 2;
				} else if (radix == 'o') {
					radix = 8;
				}
				if (radix <= 16) {
					input += 2;
					const int64_t value = strtoll(input, &endPtr, radix);
					if (input == endPtr) {
						break;
					}
					result = double(value);
					input = endPtr;
					has_value = true;
					continue;
				}
			}
			result = strtod(input, &endPtr);
			if (input == endPtr) {
				break;
			}
			input = endPtr;
			has_value = true;
		} else if (ch == '(') {
			if (has_value) {
				has_value = false;
				break;
			}
			result = Evaluate(input + 1, Precedence::Primary);
			input = endPtr;
			if (failure || *input != ')') {
				break;
			}
			++input;
			has_value = true;
		} else if (IsFuncNameStart(ch)) {
			if (has_value) {
				has_value = false;
				break;
			}
			result = EvaluateFunction(input);
			input = endPtr;
			if (failure) {
				break;
			}
			has_value = true;
		} else {
			const char * const back = input;
			++input;
			Operator op = Operator::None;
			Precedence precedence = Precedence::None;
			switch (ch) {
			case '+':
				op = Operator::Add;
				precedence = has_value ? Precedence::Addition : Precedence::UnaryPrefix;
				break;
			case '-':
				op = Operator::Subtract;
				precedence = has_value ? Precedence::Addition : Precedence::UnaryPrefix;
				break;
			case '*':
				if (*input == '*') {
					++input;
					op = Operator::Exponent;
					precedence = Precedence::Exponentiation;
				} else {
					op = Operator::Multiply;
					precedence = Precedence::Multiplication;
				}
				break;
			case '/':
				op = Operator::Divide;
				precedence = Precedence::Multiplication;
				if (*input == '/') {
					++input;
					op = Operator::FloorDivide;
				}
				break;
			case '%':
				op = Operator::Remainder;
				precedence = Precedence::Multiplication;
				break;
			case '!':
				if (*input == '=') {
					++input;
					op = Operator::Unequal;
					precedence = Precedence::Equality;
				} else {
					op = Operator::LogicalNot;
					precedence = Precedence::UnaryPrefix;
				}
				break;
			case '~':
				op = Operator::BitwiseNot;
				precedence = Precedence::UnaryPrefix;
				break;
			case '<':
				if (*input == '<') {
					++input;
					op = Operator::LeftShift;
					precedence = Precedence::Shift;
				} else {
					op = Operator::LessThan;
					precedence = Precedence::Relational;
					if (*input == '=') {
						++input;
						op = Operator::LessEqual;
					}
				}
				break;
			case '>':
				if (*input == '>') {
					++input;
					op = Operator::RightShift;
					precedence = Precedence::Shift;
					// if (*input == '>') {
					// 	++input;
					// 	op = Operator::UnsignedRightShift;
					// }
				} else {
					op = Operator::GreaterThan;
					precedence = Precedence::Relational;
					if (*input == '=') {
						++input;
						op = Operator::GreaterEqual;
					}
				}
				break;
			case '=':
				if (*input == '=') {
					++input;
					op = Operator::Equal;
					precedence = Precedence::Equality;
				}
				break;
			case '&':
				if (*input == '&') {
					++input;
					op = Operator::LogicalAnd;
					precedence = Precedence::LogicalAnd;
				} else {
					op = Operator::BitwiseAnd;
					precedence = Precedence::BitwiseAnd;
				}
				break;
			case '^':
				op = Operator::BitwiseXor;
				precedence = Precedence::BitwiseXor;
				break;
			case '|':
				if (*input == '|') {
					++input;
					op = Operator::LogicalOr;
					precedence = Precedence::LogicalOr;
				} else {
					op = Operator::BitwiseOr;
					precedence = Precedence::BitwiseOr;
				}
				break;
			case '?':
				if (*input == ':' || *input == '?') {
					++input;
					op = Operator::Coalesce;
					precedence = Precedence::Coalescing;
				} else {
					op = Operator::Ternary;
					precedence = Precedence::Ternary;
				}
				break;
			}
			if (precedence == Precedence::None || has_value != (precedence != Precedence::UnaryPrefix)) {
				input = back;
				break;
			}
			if (precedence < parent && (parent != Precedence::Exponentiation || precedence != Precedence::UnaryPrefix)) {
				input = back;
				break;
			}
			const double value = Evaluate(input, precedence);
			input = endPtr;
			if (failure) {
				break;
			}
			has_value = true;
			switch (op) {
			case Operator::Add:
				result += value;
				break;
			case Operator::Subtract:
				result -= value;
				break;
			case Operator::Multiply:
				result *= value;
				break;
			case Operator::Divide:
			case Operator::FloorDivide:
				result /= value;
				if (op == Operator::FloorDivide) {
					result = floor(result);
				}
				break;
			case Operator::Remainder:
				result = fmod(result, value);
				break;
			case Operator::Exponent:
				result = pow(result, value);
				break;
			case Operator::LogicalNot:
				result = value ? 0 : 1;
				break;
			case Operator::BitwiseNot:
				result = double(~int64_t(value));
				break;
			case Operator::LeftShift:
				result = double(int64_t(result) << int(value));
				break;
			case Operator::RightShift:
				result = double(int64_t(result) >> int(value));
				break;
			// case Operator::UnsignedRightShift:
			// 	result = double(int64_t(uint64_t(int64_t(result)) >> int(value)));
			// 	break;
			case Operator::LessThan:
				result = result < value;
				break;
			case Operator::LessEqual:
				result = result <= value;
				break;
			case Operator::GreaterThan:
				result = result > value;
				break;
			case Operator::GreaterEqual:
				result = result >= value;
				break;
			case Operator::Equal:
				result = result == value;
				break;
			case Operator::Unequal:
				result = result != value;
				break;
			case Operator::BitwiseAnd:
				result = double(int64_t(result) & int64_t(value));
				break;
			case Operator::BitwiseXor:
				result = double(int64_t(result) ^ int64_t(value));
				break;
			case Operator::BitwiseOr:
				result = double(int64_t(result) | int64_t(value));
				break;
			case Operator::LogicalAnd:
				result = result && value;
				break;
			case Operator::LogicalOr:
				result = result || value;
				break;
			case Operator::Coalesce:
				if (result == 0) {
					result = value;
				}
				break;
			case Operator::Ternary:
				has_value = false;
				if (*input == ':') {
					const double right = Evaluate(input + 1, precedence);
					input = endPtr;
					if (!failure) {
						has_value = true;
						result = result ? value : right;
					}
				}
				break;
			default:
				break;
			}
			if (!has_value) {
				break;
			}
		}
	}
	if (!has_value) {
		failure = true;
	}
	endPtr = const_cast<char *>(input);
	return result;
}

double Context::EvaluateFunction(const char *input) noexcept {
	double result = 0;
	bool has_value = false;
	unsigned len = 0;
	const char *ptr = input;
	do {
		++len;
		++ptr;
	} while (IsFuncNameChar(*ptr));
	if (len == 1) {
		if (*input == 'e') {
			has_value = true;
			input = ptr;
			result = MATH_E;
		}
	} else if (len == 2) {
		if (len == 2 && input[0] == 'p' && input[1] == 'i') {
			has_value = true;
			input = ptr;
			result = MATH_PI;
		}
	} else if (len <= maxFuncNameLen) {
		while (*ptr && IsSpace(*ptr)) {
			++ptr;
		}
		if (*ptr == '(') {
			for (const FunctionInfo &info : functionList) {
				if (len == info.len && memcmp(info.name, input, len) == 0) {
					input = ptr;
					const double arg1 = Evaluate(input + 1, Precedence::Primary);
					input = endPtr;
					const char next = (info.signature == Signature::DD_D) ? ',' : ')';
					if (!failure && *input == next) {
						if (info.signature == Signature::D_D) {
							++input;
							has_value = true;
							FuncPtrD_D func = reinterpret_cast<FuncPtrD_D>(info.func);
							result = func(arg1);
						} else {
							const double arg2 = Evaluate(input + 1, Precedence::Primary);
							input = endPtr;
							if (!failure && *input == ')') {
								++input;
								has_value = true;
								FuncPtrDD_D func = reinterpret_cast<FuncPtrDD_D>(info.func);
								result = func(arg1, arg2);
							}
						}
					}
					break;
				}
			}
		}
	}

	failure = !has_value;
	endPtr = const_cast<char *>(input);
	return result;
}

}
