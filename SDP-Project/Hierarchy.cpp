#include "Hierarchy.h"

void Hierarchy::clearHierarchy(Node* n) {
	if (!n) return;

	list<Node*>& children = n->children;
	for (auto it = children.begin(); it != children.end(); ++it) {
		clearHierarchy(*it);
	}

	delete n;
	n = nullptr;
}

Node* Hierarchy::applyPathToNode(Node* node, const string& path) {
	if (path == ".") return node;
	if (path == "..") return node->parent;
	else {
		const list<Node*>& children = node->children;
		for (auto it = children.begin(); it != children.end(); ++it) {
			if (toLower((*it)->name) == toLower(path)) return *it;
		}
	}
	return nullptr;
}

Node* Hierarchy::findNode(const vector<string>& path, const Type& type) {
	if (path.size() < 2) throw invalidFindNodePath;
	Node* n = root;
	size_t i = 0;
	if (path[0] == root->name) ++i;
	for (i; i < path.size(); ++i) n = applyPathToNode(n, path[i]);

	return n;
}

void Hierarchy::fixDepth(Node* current, int depth) {
	current->depth = depth;
	if (depth > maxDepth) maxDepth = depth;
	if (!current->children.size()) return;
	list<Node*>& children = current->children;
	for (auto it = children.begin(); it != children.end(); ++it) {
		fixDepth(*it, depth + 1);
	}
}

Type Hierarchy::getNodeType(const string& name) const {
	if (name[name.size() - 1] == 'F') return Type::file;
	else return Type::directory;
}

bool Hierarchy::empty() const {
	return root == nullptr;
}

void Hierarchy::addHierarchy(const string& rootName, vector<vector<string>>& paths) {
	if (root) return;

	root = new Node(rootName, Type::directory, 0, nullptr);
	root->path.push_back(rootName);
	currentDirectory = root;

	for (size_t i = 0; i < paths.size(); ++i) {

		if (paths[i].size() < 2) throw invalidHierarchyPath;
		Node* current = root;

		const vector<string>& path = paths[i];
		//j = 1 because skipping the root folder - all paths start with @rootname/...
		for (size_t j = 1; j < path.size(); ++j) {
			string name = path[j];
			//std::cout << name << " " << j << " " << path.size() - 1 << " \n";

			if (j == path.size() - 1) {
				Type t = getNodeType(name);
				name.pop_back();

				Node* newNode = new Node(name, t, current->depth + 1, current);
				//remove name as last paths element and assign only path before element
				paths[i][paths[i].size() - 1].pop_back();
				newNode->path = paths[i];
				current->children.push_back(newNode);
			}
			else {
				const list<Node*>& children = current->children;
				bool found = false;
				for (auto ch = children.begin(); ch != children.end(); ++ch) {
					Node* node = *ch;

					if (node->name == name) {
						current = node;
						found = true;
						break;
					}
				}
				if (!found) {
					//add missing folder
					Node* newNode = new Node(name, Type::directory, current->depth + 1, current);
					vector<string> p = current->path;
					p.push_back(name);
					newNode->path = p;
					current->children.push_back(newNode);
					current = newNode;
				}
			}
		}
	}
	fixDepth(root);
}

string Hierarchy::getCurrentDirectory() {
	string dir;

	vector<string> dirPath;
	constructFolderPath(currentDirectory, dirPath);
	for (size_t i = 0; i < dirPath.size(); ++i) {
		dir += dirPath[i];
		if (i != dirPath.size() - 1) dir += '/';
	}
	return dir;
}

void Hierarchy::curDir() {
	std::cout << getCurrentDirectory() << '>';
}

Node* Hierarchy::goToDir(const vector<string>& path) {
	if (!path.size()) return currentDirectory;
	Node* dir = currentDirectory;
	size_t i = 0;

	if (toLower(path[0]) == ":" + toLower(root->name)) {
		dir = root;
		++i;
	}
	for (i; i < path.size(); ++i) {
		dir = applyPathToNode(dir, path[i]);
		if (!dir) return nullptr;
	}
	//important so that user cannot enter files as directories
	if (dir->type == Type::file) return nullptr;
	return dir;
}

Node* Hierarchy::mkfile(const vector<string>& path, const string& fileName) {
	Node* dir = goToDir(path);
	if (!dir) throw invalidCDPath;

	Node* newFile = new Node(fileName, Type::file, dir->depth + 1, dir);
	newFile->parent = dir;
	vector<string> p = dir->path;
	p.push_back(fileName);
	newFile->path = p;
	dir->children.push_back(newFile);
	return newFile;
}

void Hierarchy::cd(const vector<string>& path) {
	Node* dir = goToDir(path);
	if (!dir) throw invalidCDPath;
	currentDirectory = dir;
}

void Hierarchy::mkdir(const vector<string>& path, const string& mkDir) {
	Node* dir = goToDir(path);
	if (!dir) throw invalidMkDirPath;

	Node* newDir = new Node(mkDir, Type::directory, dir->depth + 1, dir);
	vector<string> p = dir->path;
	p.push_back(mkDir);
	newDir->path = p;
	dir->children.push_back(newDir);
}

void Hierarchy::rmdir(const vector<string>& path, const string& rmDir) {
	Node* dir = goToDir(path);
	if (!dir) throw invalidRmDirPath;

	list<Node*>& children = dir->children;
	for (auto it = children.begin(); it != children.end(); ++it) {
		Node* rmNode = *it;
		if (rmNode->name == rmDir && rmNode->type == Type::directory) {
			if (!rmNode->children.size()) {
				children.erase(it);
				delete rmNode;
				break;
			}
			else throw folderNotEmpty;
		}
	}
}

void Hierarchy::ls(const vector<string>& path) {
	Node* dir = goToDir(path);
	if (!dir) throw invalidLSPath;

	if (!dir->children.size()) throw emptyDirectory;
	const list<Node*>& children = dir->children;
	for (auto it = children.begin(); it != children.end(); ++it) {
		Node* n = *it;
		std::cout << n->name;
		if (n->type == Type::directory) std::cout << "/";
		std::cout << "\n";
	}
}

Node* Hierarchy::getNode(const vector<string>& path) {
	Node* node = goToDir(path);
	if (!node) throw invalidGetNodePath;
	return node;
}

File* Hierarchy::getFile(const vector<string>& path, const string& fileName) {
	Node* dir = goToDir(path);
	if (!dir) throw invalidGetFilePath;
	string fName = toLower(fileName);

	list<Node*>& children = dir->children;
	for (auto it = children.begin(); it != children.end(); ++it) {
		Node* node = *it;
		//std::cout << toLower(node->name) << " " << fName << "\n";
		if (toLower(node->name) == fName && node->type == Type::file) {
			return node->file;
		}
	}
	return nullptr;
}

void Hierarchy::linkNodeFilePointers(const vector<string>& path, File* file) {
	Node* n = findNode(path, Type::file);
	if (!n) throw invalidLinkNodesPath;

	n->file = file;
	file->setNode(n);
}

void Hierarchy::constructFolderPath(Node* current, vector<string>& path) const {
	while (current != root) {
		path.push_back(current->name);
		current = current->parent;
	}
	path.push_back(root->name);
	std::reverse(path.begin(), path.end());
}

void Hierarchy::constructFolderPath(vector<string>& path) const {
	constructFolderPath(root, path);
}

void Hierarchy::getLeaves(Node* current, vector<vector<string>>& paths) const {
	if (!current) return;
	if (!current->children.size()) {
		if (current->type == Type::file) {
			vector<string> path = current->path;
			path[path.size() - 1].push_back('F');
			paths.push_back(path);
		}
		else {
			//construct path to root
			vector<string> path;
			constructFolderPath(current, path);
			path[path.size() - 1].push_back('D');
			paths.push_back(path);
		}
	}
	else {
		const list<Node*>& children = current->children;
		for (auto it = children.begin(); it != children.end(); ++it) {
			getLeaves(*it, paths);
		}
	}
}

string Hierarchy::print() const {
	if (!root) return "";
	string str;

	vector<vector<string>> paths;
	getLeaves(root, paths);

	for (size_t i = 0; i < paths.size(); ++i) {
		for (size_t j = 0; j < paths[i].size(); ++j) {
			str += paths[i][j] + '/';
		}
		str.pop_back();
		str += '\n';
	}

	return str;
}