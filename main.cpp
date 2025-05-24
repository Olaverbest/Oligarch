#include <iostream>
#include "src/interpreter.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: ./oligarch <filename>.oli" << std::endl;
		return -1;
	}

	Interpreter interpreter(256);
	interpreter.readFile(argv[1]);
	interpreter.run();
	return 0;
}