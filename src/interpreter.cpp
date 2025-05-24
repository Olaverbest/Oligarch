#include "interpreter.h"

#include <iostream>
#include <fstream>
#include <sstream>

Stack::Stack(uint size) : m_StackSize(size), m_Buffer(size, 0), m_StackPointer(-1) {}

void Stack::log() const {
	std::cout << "Stack contents (bottom -> top): ";
	if (m_StackPointer < 0) std::cout << "<Empty>";
	else for (int i = 0; i <= m_StackPointer; i++) std::cout << m_Buffer[i] << " ";
	std::cout << std::endl;
}

int Stack::top() const {
	if (m_StackPointer < 0) {
		std::cerr << "Error: Attempting to read from an empty stack!" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	return m_Buffer[m_StackPointer];
}

void Stack::push(int number) {
	if (m_StackPointer + 1 >= m_StackSize) {
		std::cerr<< "Error: Attempting to push over the stack limit of " << m_StackSize << std::endl;
		std::exit(EXIT_FAILURE);
	}
	m_Buffer[++m_StackPointer] = number;
}

int Stack::pop() {
	if (m_StackPointer < 0) {
		std::cerr<< "Error: Attempting to pop from an empty stack!" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	return m_Buffer[m_StackPointer--];
}

Interpreter::Interpreter(uint stackSize) : m_Stack(stackSize) {}

void Interpreter::readFile(const std::string &file) {
	std::ifstream infile(file);
	if (!infile) {
		std::cerr << "Could not open the file: " << file << std::endl;
		return;
	}

	std::string line;
	int instruction_counter = 0;

	while (std::getline(infile, line)) {
		// Trim whitespace
		line.erase(0, line.find_first_not_of(" \t\r\n"));
		line.erase(line.find_last_not_of(" \t\r\n") + 1);

		if (line.empty()) continue;

		// Check for label
		if (line.back() == ':') {
			std::string label =  line.substr(0, line.size() - 1);
			m_Label_Tracker[label] = m_Program.size();
			continue;
		}

		// Split line into parts
		std::istringstream iss(line);
		std::string part;
		while (iss >> part) {
			if (part.front() == '"') {
				std::string quoted = part;
				while (quoted.back() != '"' && iss >> part) {
					quoted += " " + part;
				}
				m_Program.push_back(quoted);
			} else {
				m_Program.push_back(part);
			}
		}

		instruction_counter++;
	}
}

int Interpreter::resolveValue(const std::string& token) const {
	if (m_Variables.count(token)) return m_Variables.at(token);
	try {
		return std::stoi(token);
	} catch (std::invalid_argument&) {
		std::cerr << "Error: Could not resolve value for token: " << token << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void Interpreter::run() {
	size_t program_counter = 0;
	while (program_counter < m_Program.size()) {
		std::string opcode = m_Program[program_counter];
		
		if (opcode == "exit") {
			//std::cout << "OPCODE: exit, PROGAM COUNTER: " << program_counter << std::endl;
			break;
		}

		executeInstruction(opcode, program_counter);
	}
}

void Interpreter::executeInstruction(std::string opcode, size_t& program_counter) {
	//std::cout << "OPCODE: " << opcode << ", PROGRAM COUNTER: " << program_counter << std::endl;
	program_counter++;

	if (opcode == "push") {
		m_Stack.push(resolveValue(m_Program[program_counter++]));

	} else if (opcode == "pop") {
		m_Stack.pop();

	} else if (opcode == "set") {
		std::string var_name = m_Program[program_counter++];
		std::string value_token = m_Program[program_counter++];
		int value;
		if (value_token == "pop") {
			value = m_Stack.pop();
		} else {
			value = resolveValue(value_token);
		}

		m_Variables[var_name] = value;
		//std::cout << "Variable '" << var_name << "' set to " << value << std::endl;

	} else if (opcode == "get") {
		m_Stack.push(m_Variables[m_Program[program_counter++]]);

	} else if (opcode == "log") {
		m_Stack.log();

	} else if (opcode == "write") {
		std::string arg = m_Program[program_counter++];

		if (arg.front() == '"' && arg.back() == '"') {
			std::cout << arg.substr(1, arg.size() - 2) << std::endl;
		} else if (arg == "pop") {
			std::cout << m_Stack.top() << std::endl;
			m_Stack.pop();
		} else {
			std::cout << arg << std::endl;
		}

	} else if (opcode == "read") {
		int number;
		std::cin >> number;
		m_Stack.push(number);

	} else if (opcode == "add") {
		if (m_Stack.size() < 2) {
			std::cerr << "Error: not enought values on stack for arithmetic operation" << std::endl;
			std::exit(EXIT_FAILURE);
		}

		int a = m_Stack.pop();
		int b = m_Stack.pop();
		m_Stack.push(a + b);

	} else if (opcode == "sub") {
		if (m_Stack.size() < 2) {
			std::cerr << "Error: not enought values on stack for arithmetic operation" << std::endl;
			std::exit(EXIT_FAILURE);
		}

		int a = m_Stack.pop();
		int b = m_Stack.pop();
		m_Stack.push(b - a);

	} else if (opcode == "mul") {
		if (m_Stack.size() < 2) {
			std::cerr << "Error: not enought values on stack for arithmetic operation" << std::endl;
			std::exit(EXIT_FAILURE);
		}

		int a = m_Stack.pop();
		int b = m_Stack.pop();
		m_Stack.push(a * b);

	} else if (opcode == "div") {
		if (m_Stack.size() < 2) {
			std::cerr << "Error: not enought values on stack for arithmetic operation" << std::endl;
			std::exit(EXIT_FAILURE);
		}

		int a = m_Stack.pop();
		int b = m_Stack.pop();
		m_Stack.push(a / b);
		
	} else if (opcode == "jmp") {
		std::string label = m_Program[program_counter++];
		if (!m_Label_Tracker.count(label)) {
			std::cerr << "Error: Label: " << label << " not found!" << std::endl;
			std::exit(EXIT_FAILURE);
		}
		program_counter = m_Label_Tracker[label];
		
	} else if (opcode == "jme" || opcode == "jmg" || opcode == "jml" || opcode == "jne") {
		int top = m_Stack.top();
		int cmp = resolveValue(m_Program[program_counter++]);
		std::string label = m_Program[program_counter++];

		if (!m_Label_Tracker.count(label)) {
			std::cerr << "Error: Label: " << label << " not found!" << std::endl;
			std::exit(EXIT_FAILURE);
		}

		//std::cout << "CMP: stack top = " << top << ", compare to = " << cmp << std::endl;

		if ((opcode == "jme" && top == cmp) ||
			(opcode == "jmg" && top > cmp) ||
			(opcode == "jml" && top < cmp) ||
			(opcode == "jne" && top != cmp)) {
			program_counter = m_Label_Tracker[label];
		}
		m_Stack.pop();
	}
}