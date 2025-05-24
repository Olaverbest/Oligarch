#include <iostream>
#include "src/interpreter.h"

// Remove "projects/output.log" if you dont want to log debug calls
// Or you replace it with a file that exists on your machine.

int main(int argc, char* argv[]) {
	Logger logger(true, "projects/output.log");

	if (argc < 2) {
		logger.log(LogLevel::ERROR, "Usage: ./oligarch <filename>.oli");
		return -1;
	}

	Interpreter interpreter(256, &logger);
	interpreter.readFile(argv[1]);
	interpreter.run();
	return 0;
}