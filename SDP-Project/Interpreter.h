#pragma once

#include "API.h"

/// @brief structure to parse commands and send them to the api to execute
class Interpreter {
	API api;

	void clearConsole() const;
	void printCommands() const;
	void start();
	void parse(const string& line);
	void execute(const vector<string>& cmd);

	static void validateInput(const string& line);
	static string prepareStringPath(const string& path);
public:
	Interpreter();
	Interpreter(const string& fileName);

};