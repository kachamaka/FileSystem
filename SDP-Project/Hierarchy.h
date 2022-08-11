#pragma once

#include "File.h"

#define invalidHierarchyPath \
std::exception("Invalid path in hierarchy");

#define invalidCDPath \
std::exception("Invalid path in cd");

#define invalidMkDirPath \
std::exception("Invalid path in mkdir");

#define invalidRmDirPath \
std::exception("Invalid path in rmdir");

#define folderNotEmpty \
std::exception("Folder not empty");

#define invalidLSPath \
std::exception("Invalid path in ls");

#define invalidGetNodePath \
std::exception("Invalid path in getNode");

#define invalidFindNodePath \
std::exception("Invalid path in findNode");

#define emptyDirectory \
std::exception("Directory is empty...");

#define invalidGetFilePath \
std::exception("Invalid path in getFile");

#define invalidLinkNodesPath \
std::exception("Invalid path in linking node-file pointers");


/// @brief class for organising file system relations - directories and files
class Hierarchy {
	Node* root = nullptr;

	Node* currentDirectory = nullptr;

	int maxDepth = 0;

	/// @brief delete all nodes
	/// @param n starting node - root in most cases
	static void clearHierarchy(Node* n);

	/// @brief apply path operation to corresponding node
	/// be it '.' '..' or child name
	/// @param node current node
	/// @param path operation
	/// @return node after applied operation
	static Node* applyPathToNode(Node* node, const string& path);

	/// @brief find the node meeting the path and type info
	/// @param path path to corresponding node
	/// @param type type of node
	/// @return the node
	Node* findNode(const vector<string>& path, const Type& type);

	/// @brief traverse the tree and fix the depth properties of the nodes
	/// just in case any bugs arise when adding or removing from the tree
	/// @param current current node - probably root
	/// @param depth current depth to be applied
	void fixDepth(Node* current, int depth = 0);

	/// @brief get node type - only when parsing
	/// @param name node name
	/// @return node type
	Type getNodeType(const string& name) const;

	/// @brief get all hierarchy leaves
	/// @param current starting node - probably root
	/// @param paths paths to store result onto
	void getLeaves(Node* current, vector<vector<string>>& paths) const;

public:
	Hierarchy() {}

	static Hierarchy& Get() {
		static Hierarchy hierarchy_instance;
		return hierarchy_instance;
	}

	Hierarchy(const Hierarchy&) = delete;
	Hierarchy& operator=(const Hierarchy&) = delete;

	~Hierarchy() {
		clearHierarchy(root);
	}
	
	bool empty() const;

	void clear() {
		clearHierarchy(root);
		root = nullptr;
		currentDirectory = nullptr;
		maxDepth = 0;
	}

	/// @brief an array of tree leaves to be parsed and inserted into the tree
	/// @param rootName name for root node
	/// @param paths all leaves in the tree resulting in the whole tree structure
	void addHierarchy(const string& rootName, vector<vector<string>>& paths);

	/// @brief get path of current directory
	/// @return current directory
	string getCurrentDirectory();
	void curDir();
	
	/// @brief navigate to directory along the path 
	/// @param path array of directories
	/// @return target directory
	Node* goToDir(const vector<string>& path);

	/// @brief create a file node in the hierarchy
	/// @param path path to parent directory
	/// @param fileName file name
	/// @return newly created node
	Node* mkfile(const vector<string>& path, const string& fileName);

	/// @brief navigate to directory
	/// @param path target directory
	void cd(const vector<string>& path);

	/// @brief make directory at specific location
	/// @param path target parent directory
	/// @param mkDir new directory name
	void mkdir(const vector<string>& path, const string& mkDir);

	/// @brief remove directory
	/// @param path target parent directory
	/// @param rmDir target directory
	void rmdir(const vector<string>& path, const string& rmDir);

	/// @brief list directory children
	/// @param path target directory
	void ls(const vector<string>& path);

	/// @brief return node with path
	/// @param path target directory/file node
	/// @return target node
	Node* getNode(const vector<string>& path);

	/// @brief get file referenced in node with path and name
	/// @param path target parent directory
	/// @param fileName target file node
	/// @return node file reference
	File* getFile(const vector<string>& path, const string& fileName);

	/// @brief link node and file 
	/// @param path target file node
	/// @param file file to be linked with
	void linkNodeFilePointers(const vector<string>& path, File* file);

	/// @brief construct folder path from root
	/// @param current node to get path of
	/// @param path path to store result onto
	void constructFolderPath(Node* current, vector<string>& path) const;
	void constructFolderPath(vector<string>& path) const;

	/// @brief get all leaves and return them as combined string
	/// @return tree structure
	string print() const;

};