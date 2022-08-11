#pragma once

#include "FileSystem.h"

/// @brief API to communicate with file system
class API {
	FileSystem* fs = nullptr;

public:
	API() {
		string root = "root.bin";
		string rootDir = "root";
		fs = new FileSystem(root, rootDir, INT_MAX, 0, 512, 0);
	}

	API(const string& fileName) {
		std::ifstream infile(fileName);
		if (!infile) throw std::exception("No such file...\n");

		ull cap = 0, size = 0;
		int chSize = 0, lastChIndex = 0;
		string root;
		vector<vector<string>> paths;

		string line;
		while (std::getline(infile, line)) {
			if (line == "Root:") {
				std::getline(infile, line);
				root = line;
			}
			else if (line == "Capacity:") {
				std::getline(infile, line);
				cap = std::stoull(line);
			}
			else if (line == "Size:") {
				std::getline(infile, line);
				size = std::stoull(line);
			}
			else if (line == "ChunkSize:") {
				std::getline(infile, line);
				chSize = std::stoi(line);
			}
			else if (line == "LastChunkIndex:") {
				std::getline(infile, line);
				lastChIndex = std::stoi(line);
			}
			else if (line == "Hierarchy:") {
				std::getline(infile, line);
				while (line != "") {
					string path = line;
					vector<string> splitPath = split(line);
					paths.push_back(splitPath);

					std::getline(infile, line);
				}
			}
			else if (line == "Files:") break;
		}
		fs = new FileSystem(fileName, root, cap, size, chSize, lastChIndex, paths, infile);
	}

	~API() {
		delete fs;
	}

	/// @brief ls function for printing files and directories in directory with specified path
	/// @param path path
	void ls(const vector<string>& path) const {
		fs->ls(path);
	}

	/// @brief ls function for current directory
	void ls() const {
		vector<string> path = { "." };
		fs->ls(path);
	}

	/// @brief save file system information
	void save() const {
		fs->save();
	}

	/// @brief change current directory
	/// @param path path
	void cd(const vector<string>& path) const {
		fs->cd(path);
	}

	/// @brief remove file
	/// @param path path
	/// @param file file name
	void rm(const vector<string>& path, const string& file) {
		fs->rm(path, file);
	}

	/// @brief make directory
	/// @param path path to directory
	/// @param dir new directory name
	void mkdir(const vector<string>& path, const string& dir) const {
		fs->mkdir(path, dir);
	}

	/// @brief remove directory
	/// @param path path to directory
	/// @param dir directory name to be removed
	void rmdir(const vector<string>& path, const string& dir) const {
		fs->rmdir(path, dir);
	}

	/// @brief copy file
	/// @param srcPath src path to directory
	/// @param srcName src file name
	/// @param destPath dest path to directory
	/// @param destName dest file name
	void cp(const vector<string>& srcPath, const string& srcName, const vector<string>& destPath, const string& destName) {
		fs->cp(srcPath, srcName, destPath, destName);
	}

	/// @brief print file content
	/// @param path path
	/// @param file file name
	void cat(const vector<string>& path, const string& file) const {
		fs->cat(path, file);
	}

	/// @brief write content to file
	/// @param path path
	/// @param file file name
	/// @param content content
	void write(const vector<string>& path, const string& file, const string& content) {
		fs->write(path, file, content);
	}

	/// @brief import file from outer file system
	/// @param src src file path
	/// @param dest dest dir path
	/// @param file dest file name
	void importFile(const string& src, const vector<string>& dest, const string& file) {
		fs->importFile(src, dest, file);
	}

	/// @brief write with append
	/// @param path path
	/// @param file file name
	/// @param content content
	void writeAppend(const vector<string>& path, const string& file, const string& content) const {
		fs->writeAppend(path, file, content);
	}

	/// @brief import with append
	/// @param src src file path
	/// @param dest dest dir path
	/// @param file dest file name
	void importAppend(const string& src, const vector<string>& dest, const string& file) {
		fs->importAppend(src, dest, file);
	} 

	/// @brief export file to outer file system
	/// @param src src file path
	/// @param file src file name
	/// @param dest dest file path
	void exportFile(const vector<string>& src, const string& file, const string& dest) {
		fs->exportFile(src, file, dest);
	}

};