#pragma once

#include "Chunk.h"
#include "Helper.h"

#define toLower(arg) Helper::toLower(arg)
#define split(arg) Helper::split(arg)
#define pathToString(arg) Helper::pathToString(arg)

/// @brief nodes for the tree structure
struct Node {
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

/// @brief Class for storing file info and file chunks
class File {
	string name;
	ull size = 0;
	vector<string> path;
	list<Chunk*> chunks;

	unsigned long checksum = 0;

	Node* hierarchyNode = nullptr;

	/// @brief function for redirecting the source of chunks in file 
	/// in order for them to be deleted without issues later
	/// note: making as much chunks in the file as possible a reference instead of source
	void swapChunks(Chunk* chunk, Chunk* reference = nullptr);

	/// @brief calculate file checksum and return it
	unsigned long calcChecksum() const;

public:
	~File() {
		deleteFileContents();
	}

	/// @brief delete all chunks in the file
	void deleteFileContents();

	/// @brief file hierarchy node according to the hierarchy
	/// @param n new node
	void setNode(Node* n);

	/// @brief set file size
	/// @param size new size
	void setSize(ull size);

	/// @brief set checksum by calculating it
	void setChecksum();

	/// @brief set new checksum
	/// @param chksm new checksum
	void setChecksum(unsigned long chksm);

	/// @brief set new name
	/// @param name new name
	void setName(const string& name);

	/// @brief set path
	/// @param path new path
	void setPath(const vector<string>& path);
	
	/// @brief set chunks with new list of chunks
	/// @param chunks 
	void setChunks(const list<Chunk*>& chunks);

	/// @brief append these chunks to already existing ones
	/// @param chunks 
	void addChunks(const list<Chunk*>& chunks);
	
	//get the corresponding node of the file in the tree structure
	Node* getNode() const;

	/// @brief get size
	/// @return size
	ull getSize() const;

	/// @brief get file contents
	/// @return file contents
	string getContent() const;
	
	/// @brief gets file path
	/// @return file path
	const vector<string>& getPath() const;

	/// @brief get file chunks
	/// @return file chunks
	const list<Chunk*>& getChunks() const;

	/// @brief remove last chunk
	void removeLastChunk();

	//add chunk to the end of the file
	void appendChunk(Chunk* chunk);

	/// @brief update file size and checksum
	void updateSizeAndChecksum();

	/// @brief check is file checksum is correct
	void checkChecksum();

	/// @brief save file info through the stream
	/// @param rootFile stream to file on disk
	void save(std::ofstream& rootFile) const;

	/// @brief copy command returning a copy of current file
	File* cp(ull& lastChunkIndex);

	/// @brief print file info
	void print() const;
};