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

struct Helper {

	Helper() {}

	static Helper& Get() {
		static Helper hierarchy_instance;
		return hierarchy_instance;
	}

	Helper(const Helper&) = delete;
	Helper& operator=(const Helper&) = delete;

	/// @brief combine path into string
	/// @param path path to be combined
	/// @return result
	string pathToString(const vector<string>& path);

	/// @brief split string into array of string by the delimiter character
	/// @param str source string
	/// @param delim separating symbol
	/// @return result
	vector<string> split(string str, char delim = '/');

	string toLower(const string& s);
};

//std::ostream& operator<<(std::ostream& os, const vector<string>& v) {
//	for (size_t i = 0; i < v.size(); i++) {
//		os << v[i] << (i == v.size() - 1 ? '\0' : '/');
//	}
//	return os;
//}
