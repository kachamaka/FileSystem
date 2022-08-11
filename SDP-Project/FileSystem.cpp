#include "FileSystem.h"

FileSystem::FileSystem(string fileName, string rootDir, ull cap, ull size, int chSize, int lastChIndex)
	: rootFile(fileName), root(rootDir), capacity(cap), size(size), chunkSize(chSize) {
	vector<vector<string>> paths;
	Hierarchy::Get().addHierarchy(rootDir, paths);

	lastChunkIndex = lastChIndex;
}

FileSystem::FileSystem(
	string fileName, 
	string rootDir, 
	ull cap, 
	ull size, 
	int chSize, 
	int lastChIndex, 
	vector<vector<string>>& paths, 
	std::istream& file) 
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
		//f->setSize(stoull(line));

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

	filesContainer.integrityCheck(size);

	filesContainer.updateFiles(size);
}

void FileSystem::save() {
	std::ofstream outfile(rootFile);
	if (!outfile) throw fileNotFound;

	filesContainer.updateFiles(size);

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

void FileSystem::ls(const vector<string>& path) const {
	Hierarchy::Get().ls(path);
}

void FileSystem::cd(const vector<string>& path) const {
	Hierarchy::Get().cd(path);
}

void FileSystem::mkdir(const vector<string>& path, const string& dir) const {
	Hierarchy::Get().mkdir(path, dir);
}

void FileSystem::rmdir(const vector<string>& path, const string& dir) const {
	Hierarchy::Get().rmdir(path, dir);
}

void FileSystem::rm(const vector<string>& path, const string& file) {
	filesContainer.rm(path, file, size);
}

void FileSystem::cat(const vector<string>& path, const string& file) const {
	filesContainer.cat(path, file);
}

void FileSystem::write(const vector<string>& path, const string& file, const string& content) {
	filesContainer.write(path, file, content, chunkSize, lastChunkIndex, size, capacity);
}

void FileSystem::importFile(const string& src, const vector<string>& dest, const string& file) {
	filesContainer.importFile(src, dest, file, chunkSize, lastChunkIndex, size, capacity);
}

void FileSystem::writeAppend(const vector<string>& path, const string& file, const string& content) {
	filesContainer.writeAppend(path, file, content, chunkSize, lastChunkIndex, size, capacity);
}

void FileSystem::importAppend(const string& src, const vector<string>& dest, const string& file) {
	filesContainer.importAppend(src, dest, file, chunkSize, lastChunkIndex, size, capacity);
}

void FileSystem::exportFile(const vector<string>& src, const string& file, const string& dest) {
	filesContainer.exportFile(src, file, dest);
}

void FileSystem::cp(const vector<string>& srcPath, const string& srcName, const vector<string>& destPath, const string& destName) {
	filesContainer.cp(srcPath, srcName, destPath, destName, lastChunkIndex, size, capacity);
}