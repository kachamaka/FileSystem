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

//forward declaration for File* property in Node
//also for File* property in Chunk in File.h
//maybe also for FileSystem
class File;

/// @brief nodes for the tree structure
class Node {
public:
	string name;
	vector<string> path;
	int depth = -1;

	Node* parent;
	list<Node*> children;

	File* file = nullptr;

	Type type;

	/// @brief remove child of node 
	/// @param child child to be removed
	void removeChild(Node* child) {
		if (!children.size()) return;
		for (auto it = children.begin(); it != children.end(); it++) {
			if ((*it) == child) {
				children.erase(it);
				break;
			}
		}
	}

	Node(const string& n, Type t, int d, Node* p) : name(n), type(t), depth(d), parent(p) {

	}
};

//std::ostream& operator<<(std::ostream& os, const vector<string>& v) {
//	for (size_t i = 0; i < v.size(); i++) {
//		os << v[i] << (i == v.size() - 1 ? '\0' : '/');
//	}
//	return os;
//}

/// @brief combine path into string
/// @param path path to be combined
/// @return result
string pathToString(const vector<string>& path) {
	if (!path.size()) return "";
	string strPath;
	for (size_t i = 0; i < path.size(); i++) {
		strPath += path[i] + '/';
	}
	strPath.pop_back();
	return strPath;
}

/// @brief split string into array of string by the delimiter character
/// @param str source string
/// @param delim separating symbol
/// @return result
vector<string> split(string str, char delim = '/') {
	vector<string> result;
	while (str.size()) {
		size_t index = str.find(delim);
		if (index != string::npos) {
			result.push_back(str.substr(0, index));
			str = str.substr(index + 1);
			//if (str.size() == 0) result.push_back(str);
		}
		else {
			result.push_back(str);
			str = "";
		}
	}
	return result;
}

/// @brief calc checksum of string
/// @param content input
/// @return input checksum
unsigned long calcStringChecksum(const string& content) {
	unsigned long checksum = 0;
	for (size_t i = 0; i < content.size(); i++) checksum += content[i];
	return checksum;
}

string toLower(const string& s) {
	string str = s;
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] = str[i] - 'A' + 'a';
		}
	}
	return str;
}