#pragma once

#include "Hierarchy.h"
#include "FilesContainer.h"

/// @brief structure storing all file system info + files
class FileSystem {
	string rootFile = "";
	string root = "/";
	ull size = 0;
	const ull capacity = 0;
	const int chunkSize = 10;

	FilesContainer filesContainer;

	void capacityCheck(const string& content);
public:
	FileSystem(string fileName, string rootDir, ull cap, ull size, int chSize, int lastChIndex);

	FileSystem(
		string fileName, 
		string rootDir, 
		ull cap, 
		ull size, 
		int chSize,
		int lastChIndex,
		vector<vector<string>>& paths,
		std::istream& file);

	void save() const;
	void ls(const vector<string>& path) const;
	void cd(const vector<string>& path) const;
	void mkdir(const vector<string>& path, const string& dir) const;
	void rmdir(const vector<string>& path, const string& dir) const;
	void rm(const vector<string>& path, const string& file);
	void cat(const vector<string>& path, const string& file) const;
	void write(const vector<string>& path, const string& file, const string& content);
	void writeAppend(const vector<string>& path, const string& file, const string& content);
	void cp(const vector<string>& srcPath, const string& srcName, const vector<string>& destPath, const string& destName);

};