#include <iostream>

#include "Interpreter.h"

//#include "vld.h"

int main(int argc, char* argv[]) {
	if (argc > 2) {
		std::cout << "Invalid number of arguments\n";
		return 1;
	}

	try {
		if (argc == 1) Interpreter interpreter;
		else Interpreter interpreter(argv[1]);
		//Interpreter interpreter("root.bin");
	}
	catch (const std::exception& e) {
		std::cout << e.what();
		return 1;
	}

}

