#pragma once

#include "File.h"

/// @brief class for organising file system relations - directories and files
class Hierarchy {
	Node* root = nullptr;

	Node* currentDirectory = nullptr;

	int maxDepth = 0;

	/// @brief delete all nodes
	/// @param n starting node - root in most cases
	static void clearHierarchy(Node* n) {
		if (!n) return;

		list<Node*>& children = n->children;
		for (auto it = children.begin(); it != children.end(); it++) {
			clearHierarchy(*it);
		}

		delete n;
		n = nullptr;
	}

	/// @brief apply path operation to corresponding node
	/// be it '.' '..' or child name
	/// @param node current node
	/// @param path operation
	/// @return node after applied operation
	static Node* applyPathToNode(Node* node, const string& path) {
		if (path == ".") return node;
		if (path == "..") return node->parent;
		else {
			const list<Node*>& children = node->children;
			for (auto it = children.begin(); it != children.end(); it++) {
				//std::cout << "Current:" << node->name << " Searched:" << (*it)->name << " Target:" << path << " PPPP\n";
				//std::cout << toLower((*it)->name) << " <-> " << toLower(path) << "\n";
				if (toLower((*it)->name) == toLower(path)) return *it;
			}
		}
		return nullptr;
	}

	/// @brief find the node meeting the path and type info
	/// @param path path to corresponding node
	/// @param type type of node
	/// @return the node
	Node* findNode(const vector<string>& path, const Type& type) {
		if (path.size() < 2) throw std::exception("Invalid path in findNode");
		Node* n = root;
		size_t i = 0;
		if (path[0] == root->name) i++;
		for (i; i < path.size(); i++) n = applyPathToNode(n, path[i]);

		return n;
	}

	/// @brief traverse the tree and fix the depth properties of the nodes
	/// just in case any bugs arise when adding or removing from the tree
	/// @param current current node - probably root
	/// @param depth current depth to be applied
	void fixDepth(Node* current, int depth = 0) {
		current->depth = depth;
		if (depth > maxDepth) maxDepth = depth;
		if (!current->children.size()) return;
		list<Node*>& children = current->children;
		for (auto it = children.begin(); it != children.end(); it++) {
			fixDepth(*it, depth + 1);
		}
	}

	/// @brief get node type - only when parsing
	/// @param name node name
	/// @return node type
	Type getNodeType(const string& name) {
		if (name[name.size() - 1] == 'F') return Type::file;
		else return Type::directory;
	}

public:
	Hierarchy() {}

	static Hierarchy& Get() {
		static Hierarchy hierarchy_instance;
		return hierarchy_instance;
	}

	Hierarchy(const Hierarchy&) = delete;
	Hierarchy& operator=(const Hierarchy&) = delete;

	bool empty() const {
		return root == nullptr;
	}

	/// @brief an array of tree leaves to be parsed and inserted into the tree
	/// @param rootName name for root node
	/// @param paths all leaves in the tree resulting in the whole tree structure
	void addHierarchy(const string& rootName, vector<vector<string>>& paths) {
		if (root) return;
		root = new Node(rootName, Type::directory, 0, nullptr);
		root->path.push_back(rootName);
		currentDirectory = root;

		for (size_t i = 0; i < paths.size(); i++) {

			if (paths[i].size() < 2) throw std::exception("Invalid path in addHierarchy");
			Node* current = root;

			const vector<string>& path = paths[i];
			//j = 1 because skipping the root folder - all paths start with @rootname/...
			for (size_t j = 1; j < path.size(); j++) {
				string name = path[j];
				
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
					for (auto ch = children.begin(); ch != children.end(); ch++) {
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

	~Hierarchy() {
		clearHierarchy(root);
	}

	/// @brief get path of current directory
	/// @return current directory
	string getCurrentDirectory() {
		string dir;

		vector<string> dirPath;
		constructFolderPath(currentDirectory, dirPath);
		for (size_t i = 0; i < dirPath.size(); i++) {
			dir += dirPath[i];
			if (i != dirPath.size() - 1) dir += '/';
		}
		return dir;
	}

	void curDir() {
		std::cout << getCurrentDirectory() << '>';
	}
	
	/// @brief navigate to directory along the path 
	/// @param path array of directories
	/// @return target directory
	Node* goToDir(const vector<string>& path) {
		if (!path.size()) return currentDirectory;
		Node* dir = currentDirectory;
		size_t i = 0;
		if (toLower(path[0]) == ":" + toLower(root->name)) {
			dir = root;
			i++;
		}
		for (i; i < path.size(); i++) {
			dir = applyPathToNode(dir, path[i]);
			if (!dir) return nullptr;
		}
		return dir;
	}

	/// @brief create a file node in the hierarchy
	/// @param path path to parent directory
	/// @param fileName file name
	/// @return newly created node
	Node* mkfile(const vector<string>& path, const string& fileName) {
		Node* dir = goToDir(path);
		if (!dir) throw std::exception("Invalid path in cd");

		Node* newFile = new Node(fileName, Type::file, dir->depth + 1, dir);
		newFile->parent = dir;
		vector<string> p = dir->path;
		p.push_back(fileName);
		newFile->path = p;
		dir->children.push_back(newFile);
		return newFile;
	}

	/// @brief navigate to directory
	/// @param path target directory
	void cd(const vector<string>& path) {
		Node* dir = goToDir(path);
		if (!dir) throw std::exception("Invalid path in cd");
		currentDirectory = dir;
	}

	/// @brief make directory at specific location
	/// @param path target parent directory
	/// @param mkDir new directory name
	void mkdir(const vector<string>& path, const string& mkDir) {
		Node* dir = goToDir(path);
		if (!dir) throw std::exception("Invalid path in mkdir");

		Node* newDir = new Node(mkDir, Type::directory, dir->depth + 1, dir);
		vector<string> p = dir->path;
		p.push_back(mkDir);
		newDir->path = p;
		dir->children.push_back(newDir);
	}

	/// @brief remove directory
	/// @param path target parent directory
	/// @param rmDir target directory
	void rmdir(const vector<string>& path, const string& rmDir) {
		Node* dir = goToDir(path);
		if (!dir) throw std::exception("Invalid path in rmdir");

		list<Node*>& children = dir->children;
		for (auto it = children.begin(); it != children.end(); it++) {
			Node* rmNode = *it;
			if (rmNode->name == rmDir && rmNode->type == Type::directory) {
				if (!rmNode->children.size()) {
					children.erase(it);
					delete rmNode;
					break;
				}
				else throw std::exception("Folder not empty");
			}
		}
	}

	/// @brief list directory children
	/// @param path target directory
	void ls(const vector<string>& path) {
		Node* dir = goToDir(path);
		if (!dir) throw std::exception("Invalid path in ls");

		if (!dir->children.size()) throw std::exception("Directory is empty...");
		const list<Node*>& children = dir->children;
		for (auto it = children.begin(); it != children.end(); it++) {
			Node* n = *it;
			std::cout << n->name;
			if (n->type == Type::directory) std::cout << "/";
			std::cout << "\n";
		}
	}

	/// @brief return node with path
	/// @param path target directory/file node
	/// @return target node
	Node* getNode(const vector<string>& path) {
		Node* node = goToDir(path);
		if (!node) throw std::exception("Invalid path in getNode");
		return node;
	}

	/// @brief get file referenced in node with path and name
	/// @param path target parent directory
	/// @param fileName target file node
	/// @return node file reference
	File* getFile(const vector<string>& path, const string& fileName) {
		Node* dir = goToDir(path);
		if (!dir) throw std::exception("Invalid path in getFile");
		string fName = toLower(fileName);

		list<Node*>& children = dir->children;
		for (auto it = children.begin(); it != children.end(); it++) {
			Node* node = *it;
			//std::cout << toLower(node->name) << " " << fName << "\n";
			if (toLower(node->name) == fName && node->type == Type::file) {
				return node->file;
			}
		}
		return nullptr;
	}

	/// @brief link node and file 
	/// @param path target file node
	/// @param file file to be linked with
	void linkNodeFilePointers(const vector<string>& path, File* file) {
		Node* n = findNode(path, Type::file);
		if (!n) throw std::exception("Invalid path in linking node-file pointers");

		n->file = file;
		file->setNode(n);
	}

	/// @brief construct folder path from root
	/// @param current node to get path of
	/// @param path path to store result onto
	void constructFolderPath(Node* current, vector<string>& path) const {
		while (current != root) {
			path.push_back(current->name);
			current = current->parent;
		}
		path.push_back(root->name);
		std::reverse(path.begin(), path.end());
	}

	void constructFolderPath(vector<string>& path) const {
		constructFolderPath(root, path);
	}

	/// @brief get all hierarchy leaves
	/// @param current starting node - probably root
	/// @param paths paths to store result onto
	void getLeaves(Node* current, vector<vector<string>>& paths) const {
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
			for (auto it = children.begin(); it != children.end(); it++) {
				getLeaves(*it, paths);
			}
		}
	}

	/// @brief get all leaves and return them as combined string
	/// @return tree structure
	string print() const {
		if (!root) return "";
		string str;

		vector<vector<string>> paths;
		getLeaves(root, paths);

		for (size_t i = 0; i < paths.size(); i++) {
			for (size_t j = 0; j < paths[i].size(); j++) {
				str += paths[i][j] + '/';
			}
			str.pop_back();
			str += '\n';
		}

		return str;
	}

};