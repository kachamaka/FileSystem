#pragma once

#include "File.h"

/// @brief structure for containing all files
class FilesContainer {
	list<File*> files;

	list<Chunk*> toLink;

public:
	void add(File* f) {
		files.push_back(f);
	}

	void addLink(Chunk* c) {
		toLink.push_back(c);
	}

	/// @brief link all chunks to the corresponding source chunk
	void link() {
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

	/// @brief calculate anew all files checksums
	void calcChecksums() const {
		for (auto it = files.begin(); it != files.end(); it++) {
			(*it)->setChecksum();
		}
	}

	/// @brief save all files onto file through stream
	/// @param rootFile stream to file
	void save(std::ofstream& rootFile) const {
		rootFile << "Files:\n";
		for (auto it = files.begin(); it != files.end(); it++) {
			File* file = *it;
			file->updateSizeAndChecksum();
			file->save(rootFile);
		}
	}

	/// @brief remove file
	/// @param path path to file directory
	/// @param fileName file name
	/// @param size size to update after deletion
	void rm(const vector<string>& path, const string& fileName, ull& size) {
		File* f = Hierarchy::Get().getFile(path, fileName);
		if (!f) throw std::exception("Couldn't find file");
		ull s = f->getSize();
		for (auto it = files.begin(); it != files.end(); it++) {
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

	/// @brief print file info
	/// @param path file directory
	/// @param fileName file name
	void cat(const vector<string>& path, const string& fileName) const {
		File* f = Hierarchy::Get().getFile(path, fileName);
		if (!f) throw std::exception("Couldn't find file...");
		f->checkChecksum();

		f->print();
	}

	/// @brief write content to existing file / or create new file
	/// @param path file directory
	/// @param fileName file name
	/// @param content file content to be written/overwritten
	/// @param chunkSize max chunk size
	/// @param size size to be updated after operation
	void write(const vector<string>& path, const string& fileName, const string& content, const int chunkSize, ull& size) {
		File * f = Hierarchy::Get().getFile(path, fileName);
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
			if (source) chunk->source = source;
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
					if (source) chunk->source = source;
					else chunk->content = chunkContent;
					chunk->file = f;

					chunks.push_back(chunk);
					lastIndex = i;
				}

				//add last chunk
				string chunkContent = content.substr(lastIndex);
				Chunk* chunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
				Chunk* source = findOtherChunk(chunk, chunkContent);
				if (source) chunk->source = source;
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
		else throw std::exception("Invalid remove file operation...");
		size += contentSize;
	}

	/// @brief append content to end of file
	/// @param path file directory
	/// @param fileName file info
	/// @param content content to be appended
	/// @param chunkSize max chunk size
	/// @param size size to be updated after operation
	void writeAppend(const vector<string>& path, const string& fileName, const string& content, const int chunkSize, ull& size) const {
		File* f = Hierarchy::Get().getFile(path, fileName);
		if (!f) throw std::exception("No such file found...");

		string appendContent;
		//delete last chunk if needed and align content
		if (f->getChunks().back()->size() < chunkSize) {
			Chunk* last = f->getChunks().back();
			appendContent += last->content;
			f->removeLastChunk();
			delete last;
		}
		appendContent += content;

		if (chunkSize >= content.size()) {
			Chunk* chunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
			Chunk* source = findOtherChunk(chunk, appendContent);
			if (source) chunk->source = source;
			else chunk->content = appendContent;

			chunk->file = f;
			f->deleteFileContents();
			f->appendChunk(chunk);
		}
		else {
			size_t lastIndex = 0;
			list<Chunk*> chunks;
			try {
				size_t i = chunkSize;
				for (size_t i = chunkSize; i < appendContent.size(); i += chunkSize) {
					string chunkContent = appendContent.substr(lastIndex, i - lastIndex);
					Chunk* chunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
					Chunk* source = findOtherChunk(chunk, chunkContent);
					if (source) chunk->source = source;
					else chunk->content = chunkContent;
					chunk->file = f;
					chunks.push_back(chunk);

					lastIndex = i;
				}

				//add last chunk
				string chunkContent = appendContent.substr(lastIndex);
				Chunk* chunk = new Chunk(("Chunk" + std::to_string(++lastChunkIndex)), "");
				Chunk* source = findOtherChunk(chunk, chunkContent);
				if (source) chunk->source = source;
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
		size += content.size();
	}

	/// @brief copy file onto another path
	/// @param srcPath source file directory
	/// @param srcName source file name
	/// @param destPath destination file directory
	/// @param destName destination file name
	/// @param size size to be updated after operation
	void cp(const vector<string>& srcPath, const string& srcName, const vector<string>& destPath, const string& destName, ull& size) {
		File* src= Hierarchy::Get().getFile(srcPath, srcName);
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

	/// @brief print file info
	/// @param fPath path to file
	void print(const string& fPath) const {
		for (auto it = files.begin(); it != files.end(); it++) {
			if (pathToString((*it)->getPath()) == fPath) {
				(*it)->print();
			}
		}
		std::cout << "\n";
	}

	/// @brief find chunk with specific name
	/// @param name chunk name
	/// @return found chunk
	Chunk* findChunk(const string& name) const {
		for (auto f = files.begin(); f != files.end(); f++) {
			const list<Chunk*>& chunks = (*f)->getChunks();
			for (auto ch = chunks.begin(); ch != chunks.end(); ch++) {
				if ((*ch)->name == name) return *ch;
			}
		}
		return nullptr;
	}

	/// @brief find chunk that is not the current one
	/// @param current current chunk
	/// @param content chunk content
	/// @return found chunk
	Chunk* findOtherChunk(Chunk* current, const string& content) const {
		for (auto f = files.begin(); f != files.end(); f++) {
			const list<Chunk*>& chunks = (*f)->getChunks();
			for (auto ch = chunks.begin(); ch != chunks.end(); ch++) {
				//find chunk other than current that has same contents in order to link one to the other
				if ((*ch)->content == content && current != *ch) return *ch;
			}
		}
		return nullptr;
	}

	~FilesContainer() {
		for (auto it = files.begin(); it != files.end();) {
			delete* it;
			it = files.erase(it);
		}
		for (auto it = toLink.begin(); it != toLink.end();) {
			delete* it;
			it = toLink.erase(it);
		}
	}
};