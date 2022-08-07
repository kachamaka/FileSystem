#pragma once

#include "File.h"
#include "Hierarchy.h"

/// @brief structure for containing all files
class FilesContainer {
	list<File*> files;

	list<Chunk*> toLink;

	/// @brief find chunk with specific name
	/// @param name chunk name
	/// @return found chunk
	Chunk* findChunk(const string& name) const;

	/// @brief find chunk that is not the current one
	/// @param current current chunk
	/// @param content chunk content
	/// @return found chunk
	Chunk* findOtherChunk(Chunk* current, const string& content) const;
public:
	~FilesContainer();

	void add(File* f);
	void addLink(Chunk* c);

	/// @brief link all chunks to the corresponding source chunk
	void link();

	/// @brief calculate anew all files checksums
	void calcChecksums() const;

	void updateFiles() const;

	/// @brief save all files onto file through stream
	/// @param rootFile stream to file
	void save(std::ofstream& rootFile) const;

	/// @brief remove file
	/// @param path path to file directory
	/// @param fileName file name
	/// @param size size to update after deletion
	void rm(const vector<string>& path, const string& fileName, ull& size);

	/// @brief print file info
	/// @param path file directory
	/// @param fileName file name
	void cat(const vector<string>& path, const string& fileName) const;

	/// @brief write content to existing file / or create new file
	/// @param path file directory
	/// @param fileName file name
	/// @param content file content to be written/overwritten
	/// @param chunkSize max chunk size
	/// @param size size to be updated after operation
	void write(const vector<string>& path, const string& fileName, const string& content, const int chunkSize, ull& size);
	
	void importFile(const string& src, const vector<string>& dest, const string& file, const int chunkSize, ull& size);

	/// @brief append content to end of file
	/// @param path file directory
	/// @param fileName file info
	/// @param content content to be appended
	/// @param chunkSize max chunk size
	/// @param size size to be updated after operation
	void writeAppend(const vector<string>& path, const string& fileName, const string& content, const int chunkSize, ull& size) const;

	void importAppend(const string& src, const vector<string>& dir, const string& file, const int chunkSize, ull& size) const;

	void exportFile(const vector<string>& src, const string& file, const string& dest);

	/// @brief copy file onto another path
	/// @param srcPath source file directory
	/// @param srcName source file name
	/// @param destPath destination file directory
	/// @param destName destination file name
	/// @param size size to be updated after operation
	void cp(const vector<string>& srcPath, const string& srcName, const vector<string>& destPath, const string& destName, ull& size);

	/// @brief print file info
	/// @param fPath path to file
	void print(const string& fPath) const;
};