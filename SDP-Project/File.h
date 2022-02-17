#pragma once

#include "Helper.h"
#include "Chunk.h"

static int lastChunkIndex = 0;

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
	void prepareRM() {
		for (auto it = chunks.begin(); it != chunks.end(); it++) {
			Chunk* chunk = *it;
			if (!chunk->source) {
				if (chunk->pointedBy.size()) {
					Chunk* reference = nullptr;

					list<Chunk*>& pointedBy = chunk->pointedBy;
					for (auto fIt = pointedBy.begin(); fIt != pointedBy.end(); fIt++) {
						//search for reference outside this file
						if ((*fIt)->file != this) {
							reference = *fIt;
							pointedBy.erase(fIt);
							break;
						}
					}

					if (reference) {
						//deep copy and swap reference and source
						reference->source = nullptr;
						reference->content = chunk->content;
						for (auto pb = pointedBy.begin(); pb != pointedBy.end(); pb++) {
							(*pb)->source = reference;
							reference->pointedBy.push_back(*pb);
						}
						chunk->pointedBy.clear();
						chunk->source = reference;
						reference->pointedBy.push_back(chunk);
					}
				}
			}
		}
	}

	/// @brief calculate file checksum and return it
	unsigned long calcChecksum() const {
		unsigned long chksm = '\0';
		for (auto it = chunks.begin(); it != chunks.end(); it++) {
			chksm += (*it)->calcChecksum();
		}
		return chksm;
	}
public:
	~File() {
		deleteFileContents();
	}

	/// @brief delete all chunks in the file
	void deleteFileContents() {
		if (chunks.empty()) return;
		prepareRM();

		for (auto it = chunks.begin(); it != chunks.end();) {
			if (!(*it)->source) {
				if ((*it)->pointedBy.size()) {
					std::cout << "WTF ARE YOU DOING???\n";
				}
			}
			delete* it;
			it = chunks.erase(it);
		}
	}

	void setName(const string& name) {
		this->name = name;
	}

	void setPath(const vector<string>& path) {
		this->path = path;
	}

	const vector<string>& getPath() const {
		return path;
	}

	void setChunks(const list<Chunk*>& chunks) {
		this->chunks = chunks;
	}

	const list<Chunk*>& getChunks() const {
		return chunks;
	}

	void removeLastChunk() {
		chunks.pop_back();
	}

	//get the corresponding node of the file in the tree structure
	Node* getNode() const {
		return hierarchyNode;
	}

	void setNode(Node* n) {
		hierarchyNode = n;
	}

	void setSize(ull size) {
		this->size = size;
	}

	ull getSize() const {
		return size;
	}

	//add chunk to the end of the file
	void appendChunk(Chunk* chunk) {
		chunks.push_back(chunk);
		size += chunk->size();
	}

	void setChecksum() {
		checksum = calcChecksum();
	}

	void setChecksum(unsigned long chksm) {
		checksum = chksm;
	}

	void updateSizeAndChecksum() {
		unsigned long chksm = '\0';
		ull s = 0;
		for (auto it = chunks.begin(); it != chunks.end(); it++) {
			Chunk* chunk = *it;
			chksm += chunk->calcChecksum();
			s += (chunk->source ? chunk->source->content.size() : chunk->content.size());
		}
		size = s;
		checksum = chksm;
	}

	void checkChecksum() {
		unsigned long chksm = calcChecksum();
		if (chksm != checksum) throw std::exception("Corrupted file...");
	}


	/// @brief save file info through the stream
	/// @param rootFile stream to file on disk
	void save(std::ofstream& rootFile) const {
		for (size_t i = 0; i < path.size(); i++) {
			string pathSegment = path[i];
			for (size_t j = 0; j < pathSegment.size(); j++) {
				rootFile << path[i][j];
			}
			if (i != path.size() - 1) rootFile << '/';
		}
		rootFile << ":\n";
		rootFile << "Size:\n";
		rootFile << size << '\n';
		rootFile << "Checksum:" << '\n';
		rootFile << checksum << "\n";
		rootFile << "Contents:" << '\n';
		for (auto it = chunks.begin(); it != chunks.end(); it++) {
			Chunk* chunk = *it;
			chunk->save(rootFile);
		}
		rootFile << '\n';
	}

	/// @brief copy command returning a copy of current file
	File* cp() {
		File* f = new File();
		f->size = size;
		f->checksum = checksum;
		try {
			for (auto it = chunks.begin(); it != chunks.end(); it++) {
				Chunk* examineChunk = *it;
				Chunk* cpChunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
				//creating new chunk and redirecting it to source or to the chunk being examined now
				if (examineChunk->source) {
					cpChunk->source = examineChunk->source;
					examineChunk->source->pointedBy.push_back(cpChunk);
				}
				else {
					cpChunk->source = examineChunk;
					examineChunk->pointedBy.push_back(cpChunk);
				}

				cpChunk->file = f;
				f->appendChunk(cpChunk);
			}
			return f;
		}
		catch (...) {
			delete f;
			throw;
		}
	}

	/// @brief print file info
	void print() const {
		std::cout << "Path:" << pathToString(path) << '\n';
		std::cout << "Size:" << size << '\n';
		std::cout << "Checksum:" << checksum << '\n';
		std::cout << "Contents:\n";
	
		for (auto it = chunks.begin(); it != chunks.end(); it++) {
			(*it)->print();
		}
		std::cout << "\n";
	}
	
};