#include "Chunk.h"

Chunk::Chunk(const string& name, const string& content, Chunk* source) 
	: name(name), content(content), source(source) {

}

Chunk::~Chunk() {
	if (source) {
		//deleting reference is no problem
		list<Chunk*>& srcPointedBy = source->pointedBy;
		for (auto it = srcPointedBy.begin(); it != srcPointedBy.end(); ++it) {
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
			for (auto fIt = pointedBy.begin(); fIt != pointedBy.end(); ++fIt) {
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
				for (auto pb = pointedBy.begin(); pb != pointedBy.end(); ++pb) {
					(*pb)->source = reference;
					reference->pointedBy.push_back(*pb);
				}
			}
			else {
				//if no reference get first element
				reference = pointedBy.front();
				reference->source = nullptr;
				reference->content = content;
				for (auto pb = pointedBy.begin(); pb != pointedBy.end(); ++pb) {
					(*pb)->source = reference;
					reference->pointedBy.push_back(*pb);
				}
			}
			pointedBy.clear();
		}
	}
}

unsigned long Chunk::calcStringChecksum(const string& content) {
	unsigned long checksum = 0;
	for (size_t i = 0; i < content.size(); ++i) checksum += content[i];
	return checksum;
}

unsigned long Chunk::calcChecksum() const {
	if (!source) return calcStringChecksum(content);
	return calcStringChecksum(source->content);
}

void Chunk::save(std::ofstream& output) const {
	output << name.c_str() << ':';
	if (source) output << source->name << '\n';
	else output << '\n' << content << '\n';
}

size_t Chunk::size() const {
	return (source ? source->content.size() : content.size());
}

void Chunk::print() const {
	if (source) source->print();
	else cout << content;
}

string Chunk::getContent() const {
	return (source ? source->content : content);
}
