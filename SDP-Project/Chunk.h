#pragma once

#include "Helper.h";

/// @brief structure to store chunk info
struct Chunk {
	string name;
	string content;

	Chunk* source = nullptr;
	list<Chunk*> pointedBy;

	File* file = nullptr;

	Chunk(const string& name, const string& content, Chunk* source = nullptr)
		: name(name), content(content), source(source) {

	}

	~Chunk() {
		if (source) {
			//deleting reference is no problem
			list<Chunk*>& srcPointedBy = source->pointedBy;
			for (auto it = srcPointedBy.begin(); it != srcPointedBy.end(); it++) {
				if (*it == this) {
					srcPointedBy.erase(it);
					break;
				}
			}
		}
		else {
			if (pointedBy.size()) {
				//deleting source requires to change source if possible
				Chunk* reference = nullptr;
				for (auto fIt = pointedBy.begin(); fIt != pointedBy.end(); fIt++) {
					//search for reference outside current file
					if ((*fIt)->file != this->file) {
						reference = *fIt;
						pointedBy.erase(fIt);
						break;
					}
				}
				if (reference) {
					//deep copy and swap reference and source
					reference->source = nullptr;
					reference->content = content;
					for (auto pb = pointedBy.begin(); pb != pointedBy.end(); pb++) {
						(*pb)->source = reference;
						reference->pointedBy.push_back(*pb);
					}
					//source = reference;
					//reference->pointedBy.push_back(chunk);
				}
				else {
					//if no reference get first element
					reference = pointedBy.front();
					reference->source = nullptr;
					reference->content = content;
					for (auto pb = pointedBy.begin(); pb != pointedBy.end(); pb++) {
						(*pb)->source = reference;
						reference->pointedBy.push_back(*pb);
					}
				}
				pointedBy.clear();
			}
		}
	}

	unsigned long calcChecksum() const {
		if (!source) return calcStringChecksum(content);
		return calcStringChecksum(source->content);
	}

	void save(std::ofstream& rootFile) const {
		rootFile << name.c_str() << ':';
		if (source) rootFile << source->name << '\n';
		else rootFile << '\n' << content << '\n';
	}

	size_t size() const {
		return (source ? source->content.size() : content.size());
	}

	void print() const {
		if (source) source->print();
		else std::cout << content;
	}

};