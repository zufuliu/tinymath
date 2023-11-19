#pragma once

namespace eval {

enum class Precedence : uint8_t {
	None,
	Primary,
	Ternary,
	Coalescing,
	LogicalOr,
	LogicalAnd,
	BitwiseOr,
	BitwiseXor,
	BitwiseAnd,
	Equality,
	Relational,
	Shift,
	Addition,
	Multiplication,
	UnaryPrefix,
	Exponentiation,
};

struct Context {
	char *endPtr = nullptr;
	bool failure = false;
	void Reset() noexcept {
		endPtr = nullptr;
		failure = false;
	}
	double Evaluate(const char *input, Precedence parent = Precedence::None) noexcept;
	double EvaluateFunction(const char *input) noexcept;
};

}
