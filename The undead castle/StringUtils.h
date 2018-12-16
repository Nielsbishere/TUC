#pragma once
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
namespace osomi {
	class StringUtils {
	public:
		static bool endsWith(string s, string end) {
			if (s.size() < end.size())return false;
			return s.substr(s.size() - end.size()) == end;
		}
		static bool startsWith(string s, string begin) {
			if (s.size() < begin.size())return false;
			return s.substr(0, begin.size()) == begin;
		}
		static string fromFile(string file) {
			ifstream str(file);
			if (!str.good()) {
				printf("Failed to get a file as a string\n");
				return "";
			}
			return string(istreambuf_iterator<char>(str), istreambuf_iterator<char>());
		}
		static string numString(unsigned int i, string prefix, string postfix) {
			stringstream ss;
			ss << prefix << i << postfix;
			return ss.str();
		}
		static string substring(string s, unsigned int i) {
			return s.substr(i);
		}
		static string substring(string s, unsigned int st, unsigned int en) {
			return s.substr(st, en);
		}
		static vector<string> split(string s, char c) {
			unsigned int i = 0;
			vector<string> res;
			string curr;
			while (i < s.size()) {
				char cur = s[i];
				if (cur == c) {
					res.push_back(curr);
					curr = "";
				}
				else
					curr += cur;
				i++;
			}
			res.push_back(curr);
			return res;
		}
		static string grabPath(string s) {
			vector<string> str = split(s, '/');
			if (str.size() == 1) return "";
			string st;
			for (unsigned int i = 0; i < str.size() - 1; i++)
				st = st + str[i] + "/";
			return st;
		}
		static string grabFileNoExt(string s) {
			vector<string> str = split(s, '.');
			if (str.size() == 1) return s;
			string st;
			for (unsigned int i = 0; i < str.size() - 1; i++)
				st = st + str[i] + (i != str.size()-2 ? "." : "");
			return st;
		}
		static string grabFile(string s) {
			vector<string> str = split(s, '/');
			return str[str.size() - 1];
		}
		static string trim(string s, bool dubbleWhitespace=false) {
			if (s.size() == 0)return "";
			unsigned int st, e;
			for (unsigned int i = 0; i < s.size(); i++) {
				if (s[i] != ' ' && s[i] != '\t') {
					st = i;
					break;
				}
			}
			for (unsigned int i = s.size(); i > 0; i--) {
				if (s[i-1] != ' ' && s[i-1] != '\t') {
					e = i;
					break;
				}
			}
			s = s.substr(st, e);
			if(!dubbleWhitespace){
				unsigned int nd = 0;
				bool b = false;
				for (int i = s.size(); i > 0; i--) {
					if (!b) {
						if (s[i-1] == ' ' || s[i-1] == '\t') {
							nd = i;
							b = true;
						}
					}else {
						if (!(s[i-1] == ' ' || s[i-1] == '\t')) {
							if (i + 1 != nd) 
								s = StringUtils::substring(s, 0, i) + " " + StringUtils::substring(s, nd);
							b = false;
						}
					}
				}
			}
			return s;
		}
		static unsigned int parseInt(string s) {
			int i = stoi(s);
			return *(unsigned int*)(&i);
		}
		static int parseSInt(string s) {
			return stoi(s);
		}
	};
}