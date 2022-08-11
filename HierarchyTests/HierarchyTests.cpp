#define CATCH_CONFIG_MAIN

#include "../SDP-Project/catch.hpp"
#include "../SDP-Project/Hierarchy.h"

TEST_CASE("Empty tree") {
	Hierarchy h;
	REQUIRE(h.empty());
}

SCENARIO("Add to hierarchy") {
	Hierarchy h;
	string data = "Root";
	vector<vector<string>> paths = { {"Root", "dirD"}, {"Root", "file1.txtF"}, {"Root", "dir2", "dir3", "dir4", "file.txtF"} };
	h.addHierarchy(data, paths);

	REQUIRE(!h.empty());

	REQUIRE(h.getCurrentDirectory() == "Root");

	vector<string> v = { "dir2" };
	Node* node = h.goToDir(v);

	REQUIRE(node != nullptr);
	REQUIRE(node->name == v[0]);

	h.cd({ "dir2", "dir3" });
	REQUIRE(h.getCurrentDirectory() == "Root/dir2/dir3");
	h.cd({ ":Root" });
	REQUIRE(h.getCurrentDirectory() == "Root");

	h.mkdir({ "dir2" }, "testDir");
	node = h.goToDir({ "dir2", "testDir" });
	REQUIRE(node != nullptr);
	REQUIRE(node->name == "testDir");
	REQUIRE(node->type == Type::directory);

	//searching by : for targeting root
	vector<string> searchPath = { ":Root", "dir2", "testDir" };
	string fname = "myFile.txt";
	vector<string> originalPath = { "Root", "dir2", "testDir", fname};
	h.mkfile(searchPath, fname);
	File* f = h.getFile(searchPath, fname);
	REQUIRE(f == nullptr);
	//searchPath.push_back(fname);
	node = h.getNode(searchPath);
	for (auto it = node->children.begin(); it != node->children.end(); ++it) {
		if ((*it)->name == fname) {
			node = *it;
			break;
		}
	}
	REQUIRE(node->name == fname);
	REQUIRE(node->path == originalPath);

	string print = "Root/dirD\nRoot/file1.txtF\nRoot/dir2/dir3/dir4/file.txtF\nRoot/dir2/testDir/myFile.txtF\n";
	REQUIRE(h.print() == print);

}
