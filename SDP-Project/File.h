#pragma once

#include "Chunk.h"
#include "Helper.h"

#define Helper Helper::Get()
#define toLower(arg) Helper.toLower(arg)
#define split(arg) Helper.split(arg)
#define pathToString(arg) Helper.pathToString(arg)

static int lastChunkIndex = 0;

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
	void prepareRM();

	/// @brief calculate file checksum and return it
	unsigned long calcChecksum() const;

public:
	~File() {
		deleteFileContents();
	}

	/// @brief delete all chunks in the file
	void deleteFileContents();

	void setNode(Node* n);
	void setSize(ull size);
	void setChecksum();
	void setChecksum(unsigned long chksm);
	void setName(const string& name);
	void setPath(const vector<string>& path);
	void setChunks(const list<Chunk*>& chunks);
	
	//get the corresponding node of the file in the tree structure
	Node* getNode() const;
	ull getSize() const;
	const vector<string>& getPath() const;
	const list<Chunk*>& getChunks() const;

	void removeLastChunk();

	//add chunk to the end of the file
	void appendChunk(Chunk* chunk);

	void updateSizeAndChecksum();
	void checkChecksum();

	/// @brief save file info through the stream
	/// @param rootFile stream to file on disk
	void save(std::ofstream& rootFile) const;

	/// @brief copy command returning a copy of current file
	File* cp();

	/// @brief print file info
	void print() const;
};