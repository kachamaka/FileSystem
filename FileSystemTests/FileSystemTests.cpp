#define CATCH_CONFIG_MAIN

#include "../SDP-Project/catch.hpp"
#include "../SDP-Project/FileSystem.h"
#include <limits.h>
#include <stdio.h>

vector<string> def = { "." };
vector<string> dir1 = { ".", "dir1" };

#define h Hierarchy::Get()

TEST_CASE("Initialization") {
	FileSystem fs("root.bin", "root", 100, 0, 20, 0);
	REQUIRE_NOTHROW(fs.mkdir(def, "dir1"));
	REQUIRE_NOTHROW(fs.write(dir1, "testFile", "just some stupid content for reference and giving context"));
	REQUIRE_NOTHROW(fs.write(dir1, "anotherFile", "some more stupid text???"));
	REQUIRE_THROWS(fs.write(dir1, "file3", "adding some final thoughts"));
	//fs.save();
	h.clear();
}	

TEST_CASE("mkdir, rmdir and cd") {
	FileSystem fs("root.bin", "root", INT_MAX, 0, 20, 0);

	fs.mkdir(def, "myDir");
	Node* n = h.getNode({ "myDir" });
	REQUIRE(n);
	REQUIRE(n->name == "myDir");
	fs.mkdir({ "myDir" }, "dir2");
	n = h.getNode({ "myDir", "dir2"});
	REQUIRE(n);
	REQUIRE(n->name == "dir2");
	
	fs.cd({"myDir"});
	REQUIRE(h.getCurrentDirectory() == "root/myDir");
	fs.cd({ "~" });

	REQUIRE_THROWS(fs.rmdir({}, "myDir"));
	REQUIRE_NOTHROW(fs.rmdir({"myDir"}, "dir2"));

	REQUIRE_THROWS(h.getNode({ "myDir", "dir2" }));
	REQUIRE_NOTHROW(fs.rmdir({}, "myDir"));
	h.clear();
}

TEST_CASE("Write to file") {
	FileSystem fs("root.bin", "root", INT_MAX, 0, 20, 0);

	//empty file name
	REQUIRE_THROWS(fs.write(def, "", "just some stupid content for reference and giving context"));
	//empty content
	REQUIRE_NOTHROW(fs.write(def, "file1", ""));
	//content smaller than chunk size
	REQUIRE_NOTHROW(fs.write(def, "file2", "some content"));
	//content larger than chunk size
	REQUIRE_NOTHROW(fs.write(def, "file3", "more content so that more chunks are created"));
	
	h.clear();
}

TEST_CASE("Append to file") {
	FileSystem fs("root.bin", "root", INT_MAX, 0, 20, 0);
	string str1 = "just some stupid content for reference and giving context";
	string str2 = " some more stupid text";
	REQUIRE_NOTHROW(fs.mkdir(def, "dir1"));
	REQUIRE_NOTHROW(fs.write(dir1, "testFile", str1));
	REQUIRE_NOTHROW(fs.write(dir1, "file3", "adding some final thoughts"));
	REQUIRE_THROWS(fs.writeAppend(def, "testFile", str2));
	REQUIRE_NOTHROW(fs.writeAppend(dir1, "testFile", str2));
	File* f = h.getFile(dir1, "testFile");
	REQUIRE(f);
	REQUIRE(f->getContent() == str1 + str2);
	
	h.clear();
}

TEST_CASE("Create file and add existing text. Chunk deduplication") {
	FileSystem fs("root.bin", "root", INT_MAX, 0, 20, 0);
	REQUIRE_NOTHROW(fs.mkdir(def, "dir1"));
	REQUIRE_NOTHROW(fs.write(dir1, "testFile", "just some stupid content for reference band giving context"));
	REQUIRE_NOTHROW(fs.write(def, "myFile", "just some stupid content for reference but change text now"));
	h.clear();
}

TEST_CASE("Copy file") {
	FileSystem fs("root.bin", "root", INT_MAX, 0, 20, 0);
	REQUIRE_NOTHROW(fs.mkdir(def, "dir1"));
	REQUIRE_NOTHROW(fs.write(dir1, "file1", "just some stupid content for reference band giving context"));
	REQUIRE_NOTHROW(fs.cp(dir1, "file1", def, "file1-copy"));
	REQUIRE(h.getFile(dir1, "file1")->getContent() == h.getFile(def, "file1-copy")->getContent());

	h.clear();
}

TEST_CASE("Import") {
	FileSystem fs("root.bin", "root", INT_MAX, 0, 100, 0);
	REQUIRE_NOTHROW(fs.importFile("sample.txt", {}, "myFile"));

	h.clear();
}

TEST_CASE("Export") {
	FileSystem fs("root.bin", "root", INT_MAX, 0, 100, 0);
	REQUIRE_NOTHROW(fs.importFile("sample.txt", {}, "myFile"));
	REQUIRE_NOTHROW(fs.writeAppend({}, "myFile", " add some text at the end to test if everything works properly"));
	REQUIRE_NOTHROW(fs.exportFile({}, "myFile", "sampleExport.txt"));
	remove("sampleExport.txt");

	fs.save();
	h.clear();
}