#pragma once

#include "FileSystem.h"

/// @brief api to communicate with file system
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

	void ls(const vector<string>& path) const {
		fs->ls(path);
	}

	void ls() const {
		vector<string> path = { "." };
		fs->ls(path);
	}

	void save() const {
		fs->save();
	}

	void cd(const vector<string>& path) const {
		fs->cd(path);
	}

	void rm(const vector<string>& path, const string& file) {
		fs->rm(path, file);
	}

	void cp(const vector<string>& srcPath, const string& srcName, const vector<string>& destPath, const string& destName) {
		fs->cp(srcPath, srcName, destPath, destName);
	}

	void cat(const vector<string>& path, const string& file) const {
		fs->cat(path, file);
	}

	void write(const vector<string>& path, const string& file, const string& content) {
		fs->write(path, file, content);
	}

	void importFile(const string& src, const vector<string>& dest, const string& file) {
		fs->importFile(src, dest, file);
	}

	void writeAppend(const vector<string>& path, const string& file, const string& content) const {
		fs->writeAppend(path, file, content);
	}

	void importAppend(const string& src, const vector<string>& dest, const string& file) {
		fs->importAppend(src, dest, file);
	} 

	void exportFile(const vector<string>& src, const string& file, const string& dest) {
		fs->exportFile(src, file, dest);
	}

	void mkdir(const vector<string>& path, const string& dir) const {
		fs->mkdir(path, dir);
	}

	void rmdir(const vector<string>& path, const string& dir) const {
		fs->rmdir(path, dir);
	}
};