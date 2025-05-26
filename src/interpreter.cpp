#include "interpreter.h"

#include <iostream>
#include <fstream>
#include <sstream>

Interpreter::Interpreter(unsigned int stackSize, Logger* logger) : m_Stack(stackSize, m_Logger), m_Logger(logger) {}

void Interpreter::readFile(const std::string &file) {
	std::ifstream infile(file);
	if (!infile) {
		m_Logger->log(LogLevel::ERROR, "Could not open the file: " + file);
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
				std::string string_literal = part;
				while (string_literal.back() != '"' && iss >> part) {
					string_literal += " " + part;
				}
				m_Program.push_back(string_literal);
			} else {
				m_Program.push_back(part);
			}
		}

		instruction_counter++;
	}
}

Value Interpreter::resolveValue(const std::string& token) const {
	if (m_Variables.count(token)) return m_Variables.at(token);

	if (token.front() == '"' && token.back() == '"')
		return Value(token.substr(1, token.size() - 2));

	try {
		return Value(std::stoi(token));
	} catch (std::invalid_argument&) {
		m_Logger->log(LogLevel::ERROR, "Could not resolve value for token: " + token);
		std::exit(EXIT_FAILURE);
	}
}

void Interpreter::run() {
	size_t program_counter = 0;
	while (program_counter < m_Program.size()) {
		std::string opcode = m_Program[program_counter];
		
		if (opcode == "exit") {
			m_Logger->log(LogLevel::DEBUG, "OPCODE: exit, PROGRAM COUNTER: " + std::to_string(program_counter));
			break;
		}

		executeInstruction(opcode, program_counter);
	}
}

void Interpreter::executeInstruction(std::string opcode, size_t& program_counter) {
	m_Logger->log(LogLevel::DEBUG, "OPCODE: " + opcode + ", PROGRAM COUNTER: " + std::to_string(program_counter));
	program_counter++;

	if (opcode == "push") {
		m_Stack.push(resolveValue(m_Program[program_counter++]));

	} else if (opcode == "pop") {
		m_Stack.pop();

	} else if (opcode == "set") {
		std::string var_name = m_Program[program_counter++];
		std::string value_token = m_Program[program_counter++];

		Value value;
		if (value_token == "pop") {
			value = m_Stack.pop();
		} else {
			value = resolveValue(value_token);
		}

		m_Variables[var_name] = value;
		if (value.type == ValueType::Int) m_Logger->log(LogLevel::DEBUG, "Variable '" + var_name + "' set to " + std::to_string(value.asInt()));
		else if (value.type == ValueType::String) m_Logger->log(LogLevel::DEBUG, "Variable '" + var_name + "' set to " + value.asString());

	} else if (opcode == "get") {
		m_Stack.push(m_Variables[m_Program[program_counter++]]);

	} else if (opcode == "log") {
		m_Stack.log();

	} else if (opcode == "write") {
		std::string arg = m_Program[program_counter++];

		if (arg.front() == '"' && arg.back() == '"') {
			std::cout << arg.substr(1, arg.size() - 2) << std::endl;

		} else if (arg == "pop") {
			Value val = m_Stack.pop();
			if (val.type == ValueType::Int) std::cout << val.asInt() << std::endl;
			else if (val.type == ValueType::String) std::cout << val.asString() << std::endl;
			else std::cout << "<Unknown>" << std::endl;

		} else {
			Value val = resolveValue(arg);
			if (val.type == ValueType::Int) std::cout << val.asInt() << std::endl;
			else if (val.type == ValueType::String) std::cout << val.asString() << std::endl;
		}

	} else if (opcode == "read") {
		std::string input;
		std::getline(std::cin, input);

		try {
			int number = std::stoi(input);
			m_Stack.push(Value(number));
		} catch(...) {
			m_Stack.push(Value(input));
		}
		

	} else if (opcode == "add") {
		if (m_Stack.size() < 2) {
			m_Logger->log(LogLevel::ERROR, "Not enought values on stack for arithmetic operation");
			std::exit(EXIT_FAILURE);
		}

		Value a = m_Stack.pop();
		Value b = m_Stack.pop();

		if (a.type != ValueType::Int || b.type != ValueType::Int) {
			m_Logger->log(LogLevel::ERROR, "add: operands must be integers");
			std::exit(EXIT_FAILURE);
		}

		m_Stack.push(Value(a.asInt() + b.asInt()));

	} else if (opcode == "sub") {
		if (m_Stack.size() < 2) {
			m_Logger->log(LogLevel::ERROR, "Not enought values on stack for arithmetic operation");
			std::exit(EXIT_FAILURE);
		}

		Value a = m_Stack.pop();
		Value b = m_Stack.pop();

		if (a.type != ValueType::Int || b.type != ValueType::Int) {
			m_Logger->log(LogLevel::ERROR, "sub: operands must be integers");
			std::exit(EXIT_FAILURE);
		}

		m_Stack.push(Value(b.asInt() - a.asInt()));

	} else if (opcode == "mul") {
		if (m_Stack.size() < 2) {
			m_Logger->log(LogLevel::ERROR, "Not enought values on stack for arithmetic operation");
			std::exit(EXIT_FAILURE);
		}

		Value a = m_Stack.pop();
		Value b = m_Stack.pop();

		if (a.type != ValueType::Int || b.type != ValueType::Int) {
			m_Logger->log(LogLevel::ERROR, "div: operands must be integers");
			std::exit(EXIT_FAILURE);
		}

		m_Stack.push(Value(a.asInt() * b.asInt()));

	} else if (opcode == "div") {
		if (m_Stack.size() < 2) {
			m_Logger->log(LogLevel::ERROR, "Not enought values on stack for arithmetic operation");
			std::exit(EXIT_FAILURE);
		}

		Value a = m_Stack.pop();
		Value b = m_Stack.pop();

		if (a.type != ValueType::Int || b.type != ValueType::Int) {
			m_Logger->log(LogLevel::ERROR, "mul: operands must be integers");
			std::exit(EXIT_FAILURE);
		}

		m_Stack.push(Value(a.asInt() / b.asInt()));
		
	} else if (opcode == "jmp") {
		std::string label = m_Program[program_counter++];
		if (!m_Label_Tracker.count(label)) {
			m_Logger->log(LogLevel::ERROR, "Label: " + label + " not found!");
			std::exit(EXIT_FAILURE);
		}
		program_counter = m_Label_Tracker[label];
		
	} else if (opcode == "jme" || opcode == "jmg" || opcode == "jml" || opcode == "jne") {
		Value top = m_Stack.top();
		Value cmp = resolveValue(m_Program[program_counter++]);
		std::string label = m_Program[program_counter++];

		if (!m_Label_Tracker.count(label)) {
			m_Logger->log(LogLevel::ERROR, "Label: " + label + " not found!");
			std::exit(EXIT_FAILURE);
		}

		m_Logger->log(LogLevel::DEBUG, "CMP: stack top = " + top.asString() + ", compare to = " + cmp.asString());

		if ((opcode == "jme" && top.asInt() == cmp.asInt()) ||
			(opcode == "jmg" && top.asInt() > cmp.asInt()) ||
			(opcode == "jml" && top.asInt() < cmp.asInt()) ||
			(opcode == "jne" && top.asInt() != cmp.asInt())) {
			program_counter = m_Label_Tracker[label];
		}
		m_Stack.pop();
	}
}