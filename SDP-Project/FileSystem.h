#pragma once

#include "Hierarchy.h"
#include "FilesContainer.h"

/// @brief class storing all file system info + files
class FileSystem {
	string rootFile = "";
	string root = "/";
	ull size = 0;
	ull lastChunkIndex = 0;
	const ull capacity = 0;
	const int chunkSize = 10;

	FilesContainer filesContainer;
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

	/// @brief save file system data
	void save();

	/// @brief print all files and directories in specified path directory
	/// @param path path
	void ls(const vector<string>& path) const;

	/// @brief change current directory
	/// @param path path to new directory
	void cd(const vector<string>& path) const;

	/// @brief make new directory with specified path
	/// @param path path
	/// @param dir target dir name
	void mkdir(const vector<string>& path, const string& dir) const;

	/// @brief remove dir with specified path
	/// @param path path
	/// @param dir target dir name
	void rmdir(const vector<string>& path, const string& dir) const;

	/// @brief remove file with specified path
	/// @param path path
	/// @param file file name
	void rm(const vector<string>& path, const string& file);

	/// @brief print file content with specified path
	/// @param path path 
	/// @param file file name
	void cat(const vector<string>& path, const string& file) const;

	/// @brief write to file with specified path / create if does not exist
	/// @param path path
	/// @param file file name
	/// @param content file content
	void write(const vector<string>& path, const string& file, const string& content);

	/// @brief import file from outer file system into this one
	/// @param src path to file in outer file system
	/// @param dest destination path in current file system
	/// @param file target file name
	void importFile(const string& src, const vector<string>& dest, const string& file);

	/// @brief write function but append content to file
	/// @param path path
	/// @param file file name
	/// @param content file content
	void writeAppend(const vector<string>& path, const string& file, const string& content);

	/// @brief import function but append content to file
	/// @param src path to file in outer file system
	/// @param dest destination path in current file system
	/// @param file target file name
	void importAppend(const string& src, const vector<string>& dest, const string& file);

	/// @brief export file from current file system to outer one
	/// @param src source path in current file system
	/// @param file target file name
	/// @param dest destination path in outer file system
	void exportFile(const vector<string>& src, const string& file, const string& dest);

	/// @brief copy file in current file system
	/// @param srcPath path to source file
	/// @param srcName source file name
	/// @param destPath path to destination file
	/// @param destName destination file name
	void cp(const vector<string>& srcPath, const string& srcName, const vector<string>& destPath, const string& destName);

};