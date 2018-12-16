#pragma once
#include <vector>
using namespace std;
namespace tuc {
	class Resource {
	private:
		unsigned int item;
		string name;
	public:
		Resource(unsigned int i, string n) : item(i), name(n) {}
		Resource() : Resource(0, "") {}
		unsigned int getItem() { return item; }
		string getName() { return name; }
	};
	class TUCResources {
		friend class oiTUC;
	private:
	protected:
		struct ResCon {
			Resource *r;
			unsigned int am;
			ResCon(Resource *_r, unsigned int _am) : r(_r), am(_am) {}
		};
		vector<ResCon> resources;
	public:
		void add(string res, unsigned int am) {
			for(unsigned int i=0;i<resources.size();i++)
				if (resources[i].r->getName() == res) {
					resources[i].am += am;
					return;
				}
		}
		unsigned int get(string res) {
			for (unsigned int i = 0; i<resources.size(); i++)
				if (resources[i].r->getName() == res) 
					return resources[i].am;
		}
	};
}