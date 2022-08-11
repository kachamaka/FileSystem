#include "File.h"

void File::prepareRM() {
	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		Chunk* chunk = *it;
		if (!chunk->source) {
			if (chunk->pointedBy.size()) {
				Chunk* reference = nullptr;

				list<Chunk*>& pointedBy = chunk->pointedBy;
				for (auto fIt = pointedBy.begin(); fIt != pointedBy.end(); ++fIt) {
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
					for (auto pb = pointedBy.begin(); pb != pointedBy.end(); ++pb) {
						(*pb)->source = reference;
						reference->pointedBy.push_back(*pb);
					}
					//chunk->pointedBy.clear();
					chunk->content.clear();
					chunk->source = reference;
					reference->pointedBy.push_back(chunk);
				}
			}
		}
	}
}

/// @brief calculate file checksum and return it
unsigned long File::calcChecksum() const {
	unsigned long chksm = '\0';
	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		chksm += (*it)->calcChecksum();
	}
	return chksm;
}

/// @brief delete all chunks in the file
void File::deleteFileContents() {
	if (chunks.empty()) return;
	prepareRM();

	for (auto it = chunks.begin(); it != chunks.end();) {
		Chunk* chunk = *it;
		if (!chunk->source) {
			if (chunk->pointedBy.size()) {
				std::cout << "WTF ARE YOU DOING???\n";
			}
		}
		delete chunk;
		it = chunks.erase(it);
	}
}

void File::setName(const string& name) {
	this->name = name;
}

void File::setPath(const vector<string>& path) {
	this->path = path;
}

const vector<string>& File::getPath() const {
	return path;
}

void File::setChunks(const list<Chunk*>& chunks) {
	this->chunks = chunks;
}

void File::addChunks(const list<Chunk*>& chunks) {
	//this->chunks = chunks;
	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		this->chunks.push_back(*it);
	}
}

const list<Chunk*>& File::getChunks() const {
	return chunks;
}

void File::removeLastChunk() {
	chunks.pop_back();
}

//get the corresponding node of the file in the tree structure
Node* File::getNode() const {
	return hierarchyNode;
}

void File::setNode(Node* n) {
	hierarchyNode = n;
}

void File::setSize(ull size) {
	this->size = size;
}

ull File::getSize() const {
	return size;
}

string File::getContent() const {
	string content;
	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		content += (*it)->getContent();
	}
	return content;
}

//add chunk to the end of the file
void File::appendChunk(Chunk* chunk) {
	chunks.push_back(chunk);
	size += chunk->size();
}

void File::setChecksum() {
	checksum = calcChecksum();
}

void File::setChecksum(unsigned long chksm) {
	checksum = chksm;
}

void File::updateSizeAndChecksum() {
	unsigned long chksm = '\0';
	ull s = 0;
	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		Chunk* chunk = *it;
		chksm += chunk->calcChecksum();
		s += (chunk->source ? chunk->source->content.size() : chunk->content.size());
	}
	size = s;
	checksum = chksm;
}

void File::checkChecksum() {
	unsigned long chksm = calcChecksum();
	if (chksm != checksum) throw std::exception("Corrupted file...");
}

void File::save(std::ofstream& rootFile) const {
	for (size_t i = 0; i < path.size(); ++i) {
		string pathSegment = path[i];
		for (size_t j = 0; j < pathSegment.size(); ++j) {
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
	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		Chunk* chunk = *it;
		chunk->save(rootFile);
	}
	rootFile << '\n';
}

File* File::cp(ull& lastChunkIndex) {
	File* f = new File();
	f->checksum = checksum;
	try {
		for (auto it = chunks.begin(); it != chunks.end(); ++it) {
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

void File::print() const {
	std::cout << "Path:" << pathToString(path) << '\n';
	std::cout << "Size:" << size << '\n';
	std::cout << "Checksum:" << checksum << '\n';
	std::cout << "Contents:\n";

	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		(*it)->print();
	}
	std::cout << "\n";
}