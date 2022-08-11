#pragma once

#include "API.h"

/// @brief class to parse commands and send them to the api to execute
class Interpreter {
	API api;

	/// @brief clear console text
	void clearConsole() const;

	/// @brief print all possible commands
	void printCommands() const;

	/// @brief start interpreter 
	void start();

	/// @brief parse input to array with commands
	/// @param line input
	void parse(const string& line);

	/// @brief execute command
	/// @param cmd array of commmand and arguments
	void execute(const vector<string>& cmd);

	/// @brief generic input validation
	/// @param line input
	static void validateInput(const string& line);

	/// @brief modify path string and prepare for split
	/// @param path input path
	/// @return new path
	static string prepareStringPath(const string& path);
public:

	Interpreter();
	Interpreter(const string& fileName);
};