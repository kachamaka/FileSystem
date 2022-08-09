#include "FilesContainer.h"

FilesContainer::~FilesContainer() {
	for (auto it = files.begin(); it != files.end();) {
		delete* it;
		it = files.erase(it);
	}
	for (auto it = toLink.begin(); it != toLink.end();) {
		delete* it;
		it = toLink.erase(it);
	}
}

void FilesContainer::add(File* f) {
	files.push_back(f);
}

void FilesContainer::addLink(Chunk* c) {
	toLink.push_back(c);
}

void FilesContainer::link() {
	for (auto it = toLink.begin(); it != toLink.end();) {
		Chunk* sourceChunk = findChunk((*it)->content);
		if (!sourceChunk) throw std::exception("Parse error...");
		Chunk* ch = *it;
		ch->content = "";
		ch->source = sourceChunk;
		sourceChunk->pointedBy.push_back(ch);
		it = toLink.erase(it);
	}
}

void FilesContainer::calcChecksums() const {
	for (auto it = files.begin(); it != files.end(); ++it) {
		(*it)->setChecksum();
	}
}

void FilesContainer::updateFiles() const {
	for (auto it = files.begin(); it != files.end(); ++it) {
		(*it)->updateSizeAndChecksum();
	}
}

void FilesContainer::save(std::ofstream& rootFile) const {
	rootFile << "Files:\n";
	for (auto it = files.begin(); it != files.end(); ++it) {
		File* file = *it;
		file->save(rootFile);
	}
}

void FilesContainer::rm(const vector<string>& path, const string& fileName, ull& size) {
	File* f = Hierarchy::Get().getFile(path, fileName);
	if (!f) throw fileNotFound;
	ull s = f->getSize();
	for (auto it = files.begin(); it != files.end(); ++it) {
		if (*it == f) {
			files.erase(it);
			break;
		}
	}

	Node* hierarchyNode = f->getNode();
	delete f;
	hierarchyNode->parent->removeChild(hierarchyNode);
	delete hierarchyNode;
	size -= s;
}

void FilesContainer::cat(const vector<string>& path, const string& fileName) const {
	File* f = Hierarchy::Get().getFile(path, fileName);
	if (!f) throw fileNotFound;
	f->checkChecksum();

	f->print();
}

void FilesContainer::write(const vector<string>& path, const string& fileName, const string& content, const int chunkSize, ull& size) {
	File* f = Hierarchy::Get().getFile(path, fileName);
	if (!f) {
		Node* hierarchyFile = Hierarchy::Get().mkfile(path, fileName);
		f = new File();
		f->setName(fileName);
		vector<string> p = hierarchyFile->parent->path;
		p.push_back(fileName);
		f->setPath(p);
		f->setNode(hierarchyFile);
		hierarchyFile->file = f;
		files.push_back(f);
	}
	ull contentSize = content.size();
	ull fSize = f->getSize();

	//only 1 chunk
	if (chunkSize >= content.size()) {
		Chunk* chunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
		Chunk* source = findOtherChunk(chunk, content);
		if (source) {
			chunk->source = source;
			source->pointedBy.push_back(chunk);
		}
		else chunk->content = content;

		chunk->file = f;
		f->deleteFileContents();
		f->appendChunk(chunk);
	}
	else {
		size_t lastIndex = 0;
		list<Chunk*> chunks;
		try {
			//several chunks
			size_t i = chunkSize;
			for (size_t i = chunkSize; i < content.size(); i += chunkSize) {
				string chunkContent = content.substr(lastIndex, i - lastIndex);
				Chunk* chunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
				Chunk* source = findOtherChunk(chunk, chunkContent);
				if (source) {
					chunk->source = source;
					source->pointedBy.push_back(chunk);
				}
				else chunk->content = chunkContent;
				chunk->file = f;

				chunks.push_back(chunk);
				lastIndex = i;
			}

			//add last chunk
			string chunkContent = content.substr(lastIndex);
			Chunk* chunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
			Chunk* source = findOtherChunk(chunk, chunkContent);
			if (source) {
				chunk->source = source;
				source->pointedBy.push_back(chunk);
			}
			else chunk->content = chunkContent;
			chunks.push_back(chunk);
		}
		catch (...) {
			for (auto it = chunks.begin(); it != chunks.end();) {
				delete* it;
				it = chunks.erase(it);
			}
			throw;
		}

		f->deleteFileContents();
		f->setChunks(chunks);

	}
	f->updateSizeAndChecksum();
	if (size >= fSize) size -= fSize;
	else throw std::exception("Invalid file operation...");
	size += contentSize;
}

void FilesContainer::importFile(const string& src, const vector<string>& dir, const string& file, const int chunkSize, ull& size) {
	std::ifstream input(src);
	if (!input) throw std::exception("No such source file...");

	File* dest = Hierarchy::Get().getFile(dir, file);
	if (dest) throw std::exception("Destination file already exists...");

	string content;
	string line;
	while (std::getline(input, line)) content += line;

	write(dir, file, content, chunkSize, size);
}

void FilesContainer::writeAppend(const vector<string>& path, const string& fileName, const string& content, const int chunkSize, ull& size) const {
	File* f = Hierarchy::Get().getFile(path, fileName);
	if (!f) throw fileNotFound;

	string appendContent;
	//delete last chunk if needed and align content
	if (f->getChunks().back()->size() < chunkSize) {
		Chunk* last = f->getChunks().back();
		appendContent += last->getContent();
		f->removeLastChunk();
		delete last;
	}
	appendContent += content;

	if (chunkSize >= appendContent.size()) {
		Chunk* chunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
		Chunk* source = findOtherChunk(chunk, appendContent);
		if (source) {
			chunk->source = source;
			source->pointedBy.push_back(chunk);
		}
		else chunk->content = appendContent;

		chunk->file = f;
		f->appendChunk(chunk);
	}
	else {
		size_t lastIndex = 0;
		list<Chunk*> chunks;
		try {

			//size_t i = chunkSize;
			for (size_t i = chunkSize; i < appendContent.size(); i += chunkSize) {
				string chunkContent = appendContent.substr(lastIndex, i - lastIndex);
				Chunk* chunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
				Chunk* source = findOtherChunk(chunk, chunkContent);
				if (source) {
					chunk->source = source;
					source->pointedBy.push_back(chunk);
				}
				else chunk->content = chunkContent;
				chunk->file = f;
				chunks.push_back(chunk);

				lastIndex = i;
			}

			//add last chunk
			string chunkContent = appendContent.substr(lastIndex);
			Chunk* chunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
			Chunk* source = findOtherChunk(chunk, chunkContent);
			if (source) {
				chunk->source = source;
				source->pointedBy.push_back(chunk);
			}
			else chunk->content = chunkContent;
			chunks.push_back(chunk);
		}
		catch (...) {
			for (auto it = chunks.begin(); it != chunks.end();) {
				delete* it;
				it = chunks.erase(it);
			}
			throw;
		}

		f->addChunks(chunks);

	}
	f->updateSizeAndChecksum();
	size += content.size();
}

void FilesContainer::importAppend(const string& src, const vector<string>& dir, const string& file, const int chunkSize, ull& size) const {
	std::ifstream input(src);
	if (!input) throw std::exception("No such source file...");

	File* dest = Hierarchy::Get().getFile(dir, file);
	if (!dest) throw std::exception("No such destination file found...");

	string content;
	string line;
	while (std::getline(input, line)) content += line;

	writeAppend(dir, file, content, chunkSize, size);
}

void FilesContainer::exportFile(const vector<string>& src, const string& file, const string& dest) {
	File* f = Hierarchy::Get().getFile(src, file);
	if (!f) throw std::exception("No such file found...");

	std::ifstream destFile(dest);
	if (destFile) throw std::exception("File with such destination name already exists...");
	destFile.close();

	std::ofstream output(dest);
	if (!output) throw std::exception("Couldn't create file with such name...");
	
	output << f->getContent();
}


void FilesContainer::cp(const vector<string>& srcPath, const string& srcName, const vector<string>& destPath, const string& destName, ull& size) {
	File* src = Hierarchy::Get().getFile(srcPath, srcName);
	if (!src) throw std::exception("Couldn't find source file...");
	src->checkChecksum();

	File* dest = Hierarchy::Get().getFile(destPath, destName);
	if (dest) throw std::exception("Destination file already exists...");

	Node* parentNode = Hierarchy::Get().getNode(destPath);
	if (!parentNode) throw std::exception("No such destination path...");


	dest = src->cp();
	dest->setName(destName);

	try {
		Node* n = new Node(destName, Type::file, parentNode->depth + 1, parentNode);
		Hierarchy::Get().constructFolderPath(n->path);
		n->path.push_back(destName);
		n->file = dest;
		parentNode->children.push_back(n);
		dest->setNode(n);
		dest->setPath(n->path);
		files.push_back(dest);
	}
	catch (...) {
		delete dest;
		throw;
	}
	size += dest->getSize();
}

void FilesContainer::print(const string& fPath) const {
	for (auto it = files.begin(); it != files.end(); ++it) {
		if (pathToString((*it)->getPath()) == fPath) {
			(*it)->print();
		}
	}
	std::cout << "\n";
}

Chunk* FilesContainer::findChunk(const string& name) const {
	for (auto f = files.begin(); f != files.end(); ++f) {
		const list<Chunk*>& chunks = (*f)->getChunks();
		for (auto ch = chunks.begin(); ch != chunks.end(); ++ch) {
			if ((*ch)->name == name) return *ch;
		}
	}
	return nullptr;
}

Chunk* FilesContainer::findOtherChunk(Chunk* current, const string& content) const {
	for (auto f = files.begin(); f != files.end(); ++f) {
		const list<Chunk*>& chunks = (*f)->getChunks();
		for (auto ch = chunks.begin(); ch != chunks.end(); ++ch) {
			//find chunk other than current that has same contents in order to link one to the other
			if ((*ch)->content == content && current != *ch) return *ch;
		}
	}
	return nullptr;
}