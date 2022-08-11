#pragma once

#include <string>
#include <list>
#include <fstream>
#include <iostream>

using std::string;
using std::list;
using std::cout;

//also for File* property in Chunk in File.h
class File;

/// @brief structure to store chunk info
struct Chunk {
	string name;
	string content;

	Chunk* source = nullptr;
	list<Chunk*> pointedBy;

	File* file = nullptr;

	Chunk(const string& name, const string& content, Chunk* source = nullptr);
	~Chunk();

	/// @brief calculate checksum of content
	/// @param content input
	/// @return checksum
	static unsigned long calcStringChecksum(const string& content);

	/// @brief calculate checksum of chunk
	/// @return checksum
	unsigned long calcChecksum() const;

	/// @brief write chunk to file
	/// @param rootFile output file stream
	void save(std::ofstream& output) const;

	/// @brief print chunk contents
	void print() const;

	/// @brief get chunk size
	/// @return chunk size
	size_t size() const;

	/// @brief get chunk content
	/// @return chunk content
	string getContent() const;

};