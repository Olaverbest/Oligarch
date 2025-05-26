all:
	g++ main.cpp src/interpreter.cpp src/stack.cpp src/logger.cpp -o oligarch -std=c++23 -Wall

clean:
	rm oligarch