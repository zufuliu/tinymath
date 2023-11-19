// cl /EHsc /std:c++20 /W4 /DNDEBUG /MD /Ox /Ot /GS- /GR- calc.cpp eval.cpp
// clang-cl /EHsc /std:c++20 /W4 /DNDEBUG /MD /Ox /Ot /GS- /GR- calc.cpp eval.cpp
// g++ -std=gnu++20 -DNDEBUG -O3 -fno-rtti -Wall -Wextra calc.cpp eval.cpp
// clang++ -std=gnu++20 -DNDEBUG -O3 -fno-rtti -Wall -Wextra calc.cpp eval.cpp
#include <cstdint>
#include <cfloat>
#include <string>
#include <iomanip>
#include <iostream>
#include "eval.h"

int __cdecl main() {
	std::cout << ">>> " << std::setprecision(DBL_DECIMAL_DIG - 1);
	std::string input;
	eval::Context context;
	while (std::getline(std::cin, input, '\n')) {
		if (input.find("exit") != std::string::npos || input.find("quit") != std::string::npos) {
			break;
		}
		context.Reset();
		const double result = context.Evaluate(input.c_str());
		std::cout << result << '\n';
		if (context.failure) {
			std::cout << "failed near: " << context.endPtr << '\n';
		}
		std::cout << ">>> ";
	}
	return 0;
}
