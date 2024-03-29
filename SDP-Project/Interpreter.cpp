#include "Interpreter.h"

Interpreter::Interpreter(const string& fileName) : api(fileName) {
	start();
}

void Interpreter::validateInput(const string& line) {
	//validation in generic case
	if (line.size() == 0) throw invalidCommand;

	bool inQuotes = false;
	for (size_t i = 0; i < line.size(); ++i) {
		if (line[i] == '\"') inQuotes = !inQuotes;
	}

	if (inQuotes) throw invalidCommand;
}

string Interpreter::prepareStringPath(const string& path) {
	string str = path;
	if (str[str.size() - 1] != '/') str += '/';
	return str;
}

void Interpreter::clearConsole() const {
#ifdef _WIN32
	std::system("cls");
#else
	std::system("clear");
#endif
}

void Interpreter::printCommands() const {
	cout << "mkdir <path> - create directory\n";
	cout << "rmdir <path> - delete empty directory\n";
	cout << "ls <path> - list directory contents\n";
	cout << "save - save all data\n";
	cout << "cd <path> - change current directory\n";
	cout << "cp <src> <dest> - copy file\n";
	cout << "rm <path> - remove file\n";
	cout << "cat <path> - output file contents\n";
	cout << "write <path> <content> <+append?> - write content to file / overwrite if exists / append if <+append> is present\n";
	cout << "import <src> <dest> <+append?> - import file from outer file system to current / append if <+append> is present\n";
	cout << "export <src> <dest> - export file from this file system to outer\n";
	cout << "cls - clear console\n";
	cout << "exit - exit program\n";
}

void Interpreter::start() {
	while (true) {
		Hierarchy::Get().curDir();

		string line;
		std::getline(std::cin, line);

		if (toLower(line) == "exit") break;

		try {
			parse(line);
		}
		catch (const std::exception& e) {
			cout << e.what() << std::endl;
		}
	}
}

void Interpreter::parse(const string& line) {
	validateInput(line);

	vector<string> cmds;

	size_t argStart = 0;
	size_t argEnd = 0;

	bool inQuotes = false;
	bool inWord = false;

	for (size_t i = 0; i < line.size(); ++i) {
		if (!inWord) {
			if (line[i] == '\"') {
				if (!inQuotes) argStart = i + 1;
				inQuotes = !inQuotes;
			}
			if (!inQuotes) {
				if (line[i] == ' ') {
					argStart = i + 1;
					continue;
				}
				else {
					if (line[i] == '\"') {
						//after exiting quotes iteration
						argEnd = i;
						cmds.push_back(line.substr(argStart, argEnd - argStart));
						argStart = argEnd + 1;
						continue;
					}
					else {
						argStart = i;
						if (i == line.size() - 1) {
							//catch one letter word case at the end
							argEnd = line.size();
							cmds.push_back(line.substr(argStart, argEnd - argStart));
						}
						inWord = true;
					}
				}
			}
		}
		else {
			if (line[i] == ' ') {
				argEnd = i;
				cmds.push_back(line.substr(argStart, argEnd - argStart));
				argStart = argEnd + 1;
				inWord = false;
			}
			else if (i == line.size() - 1) {
				argEnd = i;
				cmds.push_back(line.substr(argStart, argEnd - argStart + 1));
				inWord = false;
			}
		}
	}

	execute(cmds);
}


void Interpreter::execute(const vector<string>& cmd) {
	if (cmd.size() == 0) throw invalidCommand;
	string command = toLower(cmd[0]);

	if (command == "?" || command == "help") {
		printCommands();
	}
	else if (command == "cls" || command == "clear") {
		clearConsole();
	}
	else if (cmd.size() == 1) {
		if (command == "ls") api.ls();
		else if (command == "save") {
			api.save();
			cout << "File system save successfully!\n";
		}
		else throw invalidCommand;
	}
	else if (cmd.size() == 2) {
		if (cmd[1] == "") throw invalidCommand;
		if (command == "mkdir") {
			vector<string> path = split(prepareStringPath(cmd[1]));
			string dir = path[path.size() - 1];
			path.pop_back();
			api.mkdir(path, dir);
			cout << "Directory created successfully!\n";
		}
		else if (command == "rmdir") {
			vector<string> path = split(prepareStringPath(cmd[1]));
			string dir = path[path.size() - 1];
			path.pop_back();
			api.rmdir(path, dir);
			cout << "Directory deleted successfully!\n";
		}
		else if (command == "ls") {
			vector<string> path = split(prepareStringPath(cmd[1]));
			api.ls(path);
		}
		else if (command == "cd") {
			vector<string> path = split(prepareStringPath(cmd[1]));
			api.cd(path);
		}
		else if (command == "rm") {
			vector<string> path = split(prepareStringPath(cmd[1]));
			string fName = path[path.size() - 1];
			path.pop_back();
			api.rm(path, fName);
			cout << "File removed successfully!\n";
		}
		else if (command == "cat") {
			vector<string> path = split(prepareStringPath(cmd[1]));
			string fName = path[path.size() - 1];
			path.pop_back();
			api.cat(path, fName);
		}
		else throw invalidCommand;
	}
	else if (cmd.size() == 3) {
		if (cmd[1] == "" || cmd[2] == "") throw invalidCommand;
		if (command == "cp") {
			vector<string> srcPath = split(prepareStringPath(cmd[1]));
			string srcName = srcPath[srcPath.size() - 1];
			srcPath.pop_back();

			vector<string> destPath = split(prepareStringPath(cmd[2]));
			string destName = destPath[destPath.size() - 1];
			destPath.pop_back();

			api.cp(srcPath, srcName, destPath, destName);
			cout << "File copied successfully!\n";
		}
		else if (command == "write") {
			//without append
			vector<string> path = split(prepareStringPath(cmd[1]));
			string fName = path[path.size() - 1];
			path.pop_back();
			api.write(path, fName, cmd[2]);
			cout << "File written successfully!\n";
		}
		else if (command == "import") {
			//without append
			vector<string> path = split(prepareStringPath(cmd[2]));
			string fName = path[path.size() - 1];
			path.pop_back();
			api.importFile(cmd[1], path, fName);
			cout << "File imported successfully!\n";
		}
		else if (command == "export") {
			vector<string> path = split(prepareStringPath(cmd[1]));
			string fName = path[path.size() - 1];
			path.pop_back();
			api.exportFile(path, fName, cmd[2]);
			cout << "File exported successfully!\n";
		}
		else throw invalidCommand;
	}
	else if (cmd.size() == 4) {
		if (cmd[1] == "" || cmd[2] == "" || cmd[3] == "") throw invalidCommand;
		if (command == "write") {
			if (cmd[3] != "+append") throw invalidCommand;
			//with append
			vector<string> path = split(prepareStringPath(cmd[1]));
			string fName = path[path.size() - 1];
			path.pop_back();
			api.writeAppend(path, fName, cmd[2]);
			cout << "File append successful!\n";
		}
		else if (command == "import") {
			//with append
			vector<string> path = split(prepareStringPath(cmd[2]));
			string fName = path[path.size() - 1];
			path.pop_back();
			api.importAppend(cmd[1], path, fName);
			cout << "Import append successful!\n";
		}
		else throw invalidCommand;
	}
	else throw invalidCommand;

}