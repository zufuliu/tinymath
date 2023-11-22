A tiny math expression evaluation engine and an interactive calculator.

## Operator

| operator | description |
| - | - |
| `( )` | expression group |
| `x ** y` | exponent $x^y$ |
| `+x` | unary plus |
| `-x` | unary plus |
| `!x` | logical NOT |
| `~x` | bitwise NOT |
| `x * y` | multiplication |
| `x / y` | division |
| `x // y` | floor division |
| `x % y` | remainder |
| `x + y` | addition |
| `x - y` | subtraction |
| `x << y` | bitwise left shift |
| `x >> y` | bitwise right shift |
| `x < y` | less than |
| `x <= y` | less than or equal |
| `x > y` | greater than |
| `x >= y` | greater than or equal |
| `x == y` | equal |
| `x != y` | not equal |
| `x & y` | bitwise AND |
| `x ^ y` | bitwise XOR |
| `x \| y` | bitwise OR |
| `x && y` | logical AND |
| `x \|\| y` | logical OR |
| `x ?: y` | coalescing, `x ? x : z` |
| `x ?? y` | coalescing, `x ? x : z` |
| `x ? y : z` | ternary |

See https://en.cppreference.com/w/cpp/language/operator_precedence, https://docs.python.org/3/reference/expressions.html#operator-precedence, https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Operator_precedence and https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Reference/Global_Objects/Number/isSafeInteger for more document.

## Constant

| name | value |
| - | - |
| `e` | 2.71828182845904523536 |
| `pi` | 3.14159265358979323846 |

## Function

| name |
| - |
| [`double abs(double x)`](https://en.cppreference.com/w/cpp/numeric/math/abs) |
| [`double ceil(double x)`](https://en.cppreference.com/w/cpp/numeric/math/ceil) |
| [`double floor(double x)`](https://en.cppreference.com/w/cpp/numeric/math/floor) |
| [`double round(double x)`](https://en.cppreference.com/w/cpp/numeric/math/round) |
| [`double trunc(double x)`](https://en.cppreference.com/w/cpp/numeric/math/trunc) |
| [`double min(double x, double y)`](https://en.cppreference.com/w/cpp/numeric/math/fmin) |
| [`double max(double x, double y)`](https://en.cppreference.com/w/cpp/numeric/math/fmax) |
| [`double cbrt(double x)`](https://en.cppreference.com/w/cpp/numeric/math/cbrt) |
| [`double exp(double x)`](https://en.cppreference.com/w/cpp/numeric/math/exp) |
| [`double exp2(double x)`](https://en.cppreference.com/w/cpp/numeric/math/exp2) |
| [`double hypot(double x, double y)`](https://en.cppreference.com/w/cpp/numeric/math/hypot) |
| [`double log(double x)`](https://en.cppreference.com/w/cpp/numeric/math/log) |
| [`double log10(double x)`](https://en.cppreference.com/w/cpp/numeric/math/log10) |
| [`double log2(double x)`](https://en.cppreference.com/w/cpp/numeric/math/log2) |
| [`double pow(double base, double exp)`](https://en.cppreference.com/w/cpp/numeric/math/pow) |
| [`double sqrt(double x)`](https://en.cppreference.com/w/cpp/numeric/math/sqrt) |
| [`double acos(double x)`](https://en.cppreference.com/w/cpp/numeric/math/acos) |
| [`double asin(double x)`](https://en.cppreference.com/w/cpp/numeric/math/asin) |
| [`double atan(double x)`](https://en.cppreference.com/w/cpp/numeric/math/atan) |
| [`double atan2(double y, double x)`](https://en.cppreference.com/w/cpp/numeric/math/atan2) |
| [`double cos(double x)`](https://en.cppreference.com/w/cpp/numeric/math/cos) |
| [`double sin(double x)`](https://en.cppreference.com/w/cpp/numeric/math/sin) |
| [`double tan(double x)`](https://en.cppreference.com/w/cpp/numeric/math/tan) |

See https://en.cppreference.com/w/cpp/numeric/math and https://docs.python.org/3/library/math.html for function description.
