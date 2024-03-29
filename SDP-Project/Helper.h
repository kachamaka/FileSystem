#pragma once

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>

#define invalidCommand \
std::invalid_argument("Please enter a valid command. Type \"?\" for more info")

using std::string;
using std::vector;
using std::list;
using ull = unsigned long long;

enum Type {
	directory,
	file
};

/// @brief structure with functions used by several classes
struct Helper {

	/// @brief combine path into string
	/// @param path path to be combined
	/// @return result
	static string pathToString(const vector<string>& path);

	/// @brief split string into array of string by the delimiter character
	/// @param str source string
	/// @param delim separating symbol
	/// @return result
	static vector<string> split(string str, char delim = '/');

	/// @brief convert string to lower characters only and return it
	/// @param s source string
	/// @return new string with all lower characters
	static string toLower(const string& s);
};
