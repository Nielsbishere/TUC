#pragma once
#include <vector>
using namespace std;
namespace osomi {
	class ArrayHelper {
	public:
		template<class T> static bool contains(vector<T*> *v, T *t, unsigned int maxOff) {
			for (vector<T*>::iterator i = v->begin(); i != v->end() && (i - v->begin()) < maxOff; i++)
				if (*i == t)return true;
			return false;
		}
		template<class T> static bool contains(vector<T*> *v, T *t) {
			for (vector<T*>::iterator i = v->begin(); i != v->end(); i++)
				if (*i == t)return true;
			return false;
		}
		template<class T> static vector<vector<T>> select(vector<vector<T>> arr, T element) {
			vector<vector<T>> res;
			for (unsigned int i = 0; i < arr.size(); i++) {
				bool contains = false;
				for (unsigned int j = 0; j < arr[i].size(); j++) {
					if (arr[i][j] == element) {
						contains = true;
						break;
					}
				}
				if (contains)res.push_back(arr[i]);
			}
			return res;
		}
		static vector<string> selectStrings(vector<string> arr, char element) {
			vector<string> res;
			for (unsigned int i = 0; i < arr.size(); i++) {
				bool contains = false;
				for (unsigned int j = 0; j < arr[i].size(); j++) {
					if (arr[i][j] == element) {
						contains = true;
						break;
					}
				}
				if (contains)res.push_back(arr[i]);
			}
			return res;
		}
	};
}