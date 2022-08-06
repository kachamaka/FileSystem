#include "Helper.h"

string Helper::pathToString(const vector<string>& path) {
	if (!path.size()) return "";
	string strPath;
	for (size_t i = 0; i < path.size(); i++) {
		strPath += path[i] + '/';
	}
	strPath.pop_back();
	return strPath;
}

vector<string> Helper::split(string str, char delim) {
	vector<string> result;
	while (str.size()) {
		size_t index = str.find(delim);
		if (index != string::npos) {
			result.push_back(str.substr(0, index));
			str = str.substr(index + 1);
			//if (str.size() == 0) result.push_back(str);
		}
		else {
			result.push_back(str);
			str = "";
		}
	}
	return result;
}

string Helper::toLower(const string& s) {
	string str = s;
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] = str[i] - 'A' + 'a';
		}
	}
	return str;
}