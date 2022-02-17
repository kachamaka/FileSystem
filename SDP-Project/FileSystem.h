#pragma once

#include "Hierarchy.h"
#include "FilesContainer.h"

/// @brief structure storing all file system info + files
class FileSystem {
	string rootFile = "";
	string root = "/";
	const ull capacity = 0;
	ull size = 0;
	const int chunkSize = 10;

	FilesContainer filesContainer;

	void capacityCheck(const string& content) {
		if (size + content.size() > capacity) throw std::exception("Not enough space...");
	}

public:
	FileSystem(string fileName, string rootDir, ull cap, ull size, int chSize, int lastChIndex)
		: rootFile(fileName), root(rootDir), capacity(cap), size(size), chunkSize(chSize) {
		vector<vector<string>> paths;
		Hierarchy::Get().addHierarchy(rootDir, paths);

		lastChunkIndex = lastChIndex;
	}

	FileSystem(string fileName, string rootDir, ull cap, ull size, int chSize, int lastChIndex, vector<vector<string>>& paths, std::istream& file)
		: rootFile(fileName), root(rootDir), capacity(cap), size(size), chunkSize(chSize) {
		Hierarchy::Get().addHierarchy(rootDir, paths);

		lastChunkIndex = lastChIndex;

		//send rest of stream to parse files
		string line;
		while (!file.eof()) {

			std::getline(file, line);
			if (line.empty()) continue;

			File* f = new File();
			//get root name
			line.pop_back();
			vector<string> path = split(line);
			f->setName(path[path.size() - 1]);
			f->setPath(path);

			//get file size
			std::getline(file, line);
			std::getline(file, line);
			f->setSize(stoull(line));

			//get checksum
			std::getline(file, line);
			std::getline(file, line);
			f->setChecksum(stoul(line));

			std::getline(file, line);
			if (line == "Contents:") {
				std::getline(file, line);

				while (line.size() && !file.eof()) {
					Chunk* ch = nullptr;
					if (line[line.size() - 1] == ':') {
						line.pop_back();
						string chContent;
						std::getline(file, chContent);
						ch = new Chunk(line, chContent);
					}
					else {
						//to link
						size_t delim = line.find(':');
						string name = line.substr(0, delim);
						string source = line.substr(delim + 1);
						ch = new Chunk(name, source);
						filesContainer.addLink(ch);
					}
					ch->file = f;
					f->appendChunk(ch);
					std::getline(file, line);
				}
			}
			filesContainer.add(f);
			Hierarchy::Get().linkNodeFilePointers(path, f);
		}
		filesContainer.link();

		filesContainer.calcChecksums();
	}

	void save() const {
		std::ofstream outfile(rootFile);
		if (!outfile) throw std::exception("Couldn't find file...");
		//save all info onto file
		outfile << "Root:\n";
		outfile << root.c_str() << '\n';
		outfile << "Capacity:\n";
		outfile << capacity << '\n';
		outfile << "Size:\n";
		outfile << size << '\n';
		outfile << "ChunkSize:\n";
		outfile << chunkSize << '\n';
		outfile << "LastChunkIndex:\n";
		outfile << lastChunkIndex << '\n';
		outfile << "Hierarchy:\n";
		outfile << Hierarchy::Get().print().c_str() << '\n';

		filesContainer.save(outfile);
	}

	void ls(const vector<string>& path) const {
		Hierarchy::Get().ls(path);
	}

	void cd(const vector<string>& path) const {
		Hierarchy::Get().cd(path);
	}

	void mkdir(const vector<string>& path, const string& dir) const {
		Hierarchy::Get().mkdir(path, dir);
	}

	void rmdir(const vector<string>& path, const string& dir) const {
		Hierarchy::Get().rmdir(path, dir);
	}

	void rm(const vector<string>& path, const string& file) {
		filesContainer.rm(path, file, size);
	}

	void cat(const vector<string>& path, const string& file) const {
		filesContainer.cat(path, file);
	}

	void write(const vector<string>& path, const string& file, const string& content) {
		filesContainer.write(path, file, content, chunkSize, size);
	}

	void writeAppend(const vector<string>& path, const string& file, const string& content) {
		filesContainer.writeAppend(path, file, content, chunkSize, size);
	}

	void cp(const vector<string>& srcPath, const string& srcName, const vector<string>& destPath, const string& destName) {
		filesContainer.cp(srcPath, srcName, destPath, destName, size);
	}

};