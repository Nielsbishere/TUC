#pragma once
using namespace std;
#include <string>
#include <vector>
#include <fstream>
#include "BinaryHelper.h"
#include "StringUtils.h"
#include "Resources.h"

#include "TUCResources.h"

#include "ModelLoader.h"

unsigned char uc(unsigned int a, char *mem) { return *reinterpret_cast<unsigned char*>(&mem[a]); }
unsigned int ui(unsigned int a, char *mem) { unsigned int i = (uc(a, mem) << 24) | (uc(a + 1, mem) << 16) | (uc(a + 2, mem) << 8) | uc(a + 3, mem); return *reinterpret_cast<unsigned int*>(&i); }

using namespace osomi;
namespace tuc {
	class oiTUC {
	public:
		class Category {
		private:
			Category *parent;
			unsigned int item;
			string name;
		public:
			Category(Category *p, unsigned int i, string n) : parent(p), item(i), name(n){}
			Category() : Category(nullptr, 0, "") {}
			Category *getParent() { return parent; }
			unsigned int getItem() { return item; }
			string getName() { return name; }
		};
		class Item {
		public:
			class Res {
			private:
				Resource *res;
				unsigned int amount;
			public:
				Res(Resource *r, unsigned int am) : res(r), amount(am) {}
				Res() : Res(nullptr, 0) {  }
				Resource *getResource() { return res; }
				unsigned int getAmount() { return amount; }
			};
		private:
			unsigned int item;
			unsigned char amount, level;
			Category *category;
			string name;
			vector<Res> cost;
		public:
			Item(unsigned int it, unsigned char am, unsigned int lvl, Category *cat, string n, vector<Res> c) : item(it), amount(am), level(lvl), category(cat), name(n), cost(c) {}
			Item() : Item(0, 0, 0, nullptr, "", vector<Res>()) {}
			unsigned int getItem() { return item; }
			unsigned char getAmount() { return amount; }
			unsigned char getLevel() { return level; }
			Category *getCategory() { return category; }
			string getName() { return name; }
			vector<Res> getCost() { return cost; }
		};
		class InventoryItem {
		private:
			unsigned int id;
			unsigned char level;
			Item *droppedFrom;
			string name;
			unsigned char flags;
		public:
			InventoryItem(unsigned int invid, unsigned char lvl, Item *parent, string n, unsigned char f) : id(invid | 0x80000000), level(lvl), droppedFrom(parent), name(n), flags(f) {}
			InventoryItem() : InventoryItem(0, 0, nullptr, "", 0){}
			unsigned int getItem() { return id; }
			unsigned char getLevel() { return level; }
			Item *getDrop() { return droppedFrom; }
			string getName() { return name; }
			bool isResource() { return flags & 0x1; }
		};
	private:
		vector<Category> *cats;
		vector<Resource> *res;
		vector<Item> *items;
		vector<InventoryItem> *iitems;
		oiTUC(vector<Category> *cs, vector<Resource> *r, vector<Item> *i, vector<InventoryItem> *inv) : cats(cs), res(r), items(i), iitems(inv){}
	public:
		~oiTUC() {
			delete cats;
			delete res;
			delete items;
			delete iitems;
		}
		vector<Category*> getCategories(Category *c) {
			vector<Category*> cs;
			for (unsigned int i = 0; i < cats->size(); i++)
				if ((*cats)[i].getParent() == c)cs.push_back(&(*cats)[i]);
			return cs;
		}
		vector<Item> getItems(Category *c) {
			vector<Item> is;
			for (unsigned int i = 0; i < items->size(); i++)
				if ((*items)[i].getCategory() == c)is.push_back((*items)[i]);
			return is;
		}
		string getName(unsigned int id) {
			if (id == 0)return "Air";
			if (id & 0x80000000) {
				id &= 0x7FFFFFFF;
				//Grab it
			}
			else {
				id -= 1;
				//Grab it from item
			}
		}
		static oiTUC *load(string file, TileRenderer *cube, ModelLoader *loader, vector<Model*> &models, vector<TileRenderer*> &renderers, TileSet *ts, TUCResources *&allResources, bool log=true) {
			if (!StringUtils::endsWith(file, ".oiTUC")) {
				printf("Couldn't open the file! Err%u (%s) (#%s)\n", 0, file.c_str(), BinaryHelper::asHex((unsigned int)0));
				return nullptr;
			}
			ifstream input(file, ios::binary | ios::in);
			if (!input.good()) {
				printf("Couldn't open the file! Err%u (%s) (#%s)\n", 1, file.c_str(), BinaryHelper::asHex((unsigned int)0));
				return nullptr;
			}
			printf("Loading file data...\n");
			input.seekg(0, ios::end);
			unsigned int l = input.tellg();
			unsigned int index = 0;
			input.seekg(0, ios::beg);
			if (l < index + 1) {
				printf("Invalid file! Err%u (%s) (#%s)\n", 2, file.c_str(), BinaryHelper::asHex(index).c_str());
				return nullptr;
			}
			char *mem = (char*)malloc(l);
			input.read(mem, l);
			unsigned char rS = uc(0, mem);
			index++;

			vector<Category> *cats = new vector<Category>(rS);
			unsigned int cati = 0;
			for (unsigned int i = 0; i < rS; i++) {
				if (l < index + 6) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 3, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					return nullptr;
				}
				unsigned char cat = uc(index, mem);
				if (cat > cati) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 4, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					return nullptr;
				}
				unsigned int it = ui(index + 1, mem);
				unsigned char len = uc(index + 5, mem);
				index += 6;
				if (l < index + len) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 5, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					return nullptr;
				}
				char *st = &mem[index];
				string name = string(st, len);

				(*cats)[cati] = Category(cat == 0 ? nullptr : &(*cats)[cat - 1], it, name);
				if(log)printf("Creating cat with %p %u %s\n", (*cats)[cati].getParent(), it, name.c_str());
				cati++;
				index += len;
			}
			if (l < index + 1) {
				printf("Invalid file! Err%u (%s) (#%s)\n", 6, file.c_str(), BinaryHelper::asHex(index).c_str());
				delete[] mem;
				delete cats;
				return nullptr;
			}
			unsigned char rL = uc(index, mem);
			index++;

			vector<Resource> *res = new vector<Resource>(rL);
			unsigned int resi = 0;
			for (unsigned int i = 0; i < rL; i++) {
				if (l < index + 5) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 7, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					return nullptr;
				}
				unsigned int it = ui(index, mem);
				unsigned char len = uc(index + 4, mem);
				index += 5;
				if (l < index + len) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 9, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					return nullptr;
				}
				char *st = &mem[index];
				string name = string(st, len);

				(*res)[resi] = Resource(it, name);
				if(log)printf("Creating res with %u %s\n", it, name.c_str());
				resi++;
				index += len;
			}

			if (l < index + 1) {
				if(log)printf("Invalid file! Err%u (%s) (#%s)\n", 10, file.c_str(), BinaryHelper::asHex(index).c_str());
				delete[] mem;
				delete cats;
				delete res;
				return nullptr;
			}
			unsigned char iL = uc(index, mem);
			index++;

			vector<Item> *is = new vector<Item>(iL);
			unsigned int itemi = 0;
			for (unsigned int i = 0; i < iL; i++) {
				if (l < index + 4) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 11, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					return nullptr;
				}
				unsigned char am = uc(index, mem);
				unsigned char lvl = uc(index + 1, mem);
				unsigned char cat = uc(index + 2, mem);
				unsigned char len = uc(index + 3, mem);
				if (cat > cati) {
					printf("Invalid file! Err%u.%u (%s) (#%s)\n", 4, 1, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					return nullptr;
				}
				Category *categ = &(*cats)[cat];
				index += 4;
				if (l < index + len + 1) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 13, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					return nullptr;
				}
				char *st = &mem[index];
				string name = string(st, len);
				if (len == 0)name = "Grass";
				index += len;

				len = uc(index, mem);
				index++;
				if (l < index + len + 1) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 14, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					return nullptr;
				}
				st = &mem[index];
				string extension = string(st, len);
				Texture *t = nullptr;
				if (len != 0) t = Resources::loadTexture(name + "." + extension);
				index += len;

				TileRenderer *tr = nullptr;
				unsigned char modelT = uc(index, mem);
				index++;
				if (modelT == 0) tr = cube;
				else if (modelT == 1) {
					string model = name + ".obj";
					bool exists = loader->exists(model);
					Model *m = loader->loadObj(model, ModelLoader::XZ);
					if (m == nullptr) {
						printf("Invalid file! Err%u (%s) (#%s)\n", 21, file.c_str(), BinaryHelper::asHex(index).c_str());
						delete[] mem;
						delete cats;
						delete res;
						delete is;
						return nullptr;
					}
					if (!exists) {
						models.push_back(m);
						renderers.push_back(tr = new TileRenderer(m));
					}
					else {
						printf("Invalid file! Err%u (%s) (#%s)\n", 22, file.c_str(), BinaryHelper::asHex(index).c_str());
						delete[] mem;
						delete cats;
						delete res;
						delete is;
						return nullptr;
					}
					if(log)printf("Loaded %s\n", model.c_str());
				}
				else {
					printf("Invalid file! Err%u (%s) (#%s)\n", 23, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					return nullptr;
				}
				if (l < index + 1) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 17, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					return nullptr;
				}
				unsigned char ress = uc(index, mem);
				index++;
				if (l < index + ress * 5) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 18, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					return nullptr;
				}
				vector<Item::Res> r(ress);
				for (unsigned int i = 0; i < ress; i++) {
					unsigned char resp = uc(index, mem);
					unsigned int am = ui(index+1, mem);
					index += 5;
					if(resp >= res->size()) {
						printf("Invalid file! Err%u (%s) (#%s)\n", 19, file.c_str(), BinaryHelper::asHex(index).c_str());
						delete[] mem;
						delete cats;
						delete res;
						delete is;
						return nullptr;
					}
					r[i] = Item::Res(&(*res)[resp], am);
				}

				(*is)[itemi] = Item(itemi+1, am, lvl, categ, name, r);
				ts->assign(ts->size(true), t, tr);
				if (log)printf("Assigned %u to %p %p %s\n", ts->size(true) - 1, t, tr, name.c_str());
				if(log)printf("Creating item with %u %u %p %s %s %p %u %u\n", am, lvl, categ, name.c_str(), extension.c_str(), tr, itemi, is->size());
				itemi++;
			}
			if (l < index + 1) {
				printf("Invalid file! Err%u (%s) (#%s)\n", 24, file.c_str(), BinaryHelper::asHex(index).c_str());
				delete[] mem;
				delete cats;
				delete res;
				delete is;
				return nullptr;
			}
			unsigned char iil = uc(index, mem);
			index++;
			vector<InventoryItem> *ii = new vector<InventoryItem>(iil);
			for (unsigned int i = 0; i < iil; i++) {
				if (l < index + 6) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 25, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					delete ii;
					return nullptr;
				}
				unsigned char lvl = uc(index, mem);
				unsigned int item = ui(index + 1, mem);
				if (item > is->size()) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 26, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					delete ii;
					return nullptr;
				}
				Item *it = item == 0 ? nullptr : &(*is)[item - 1];
				unsigned char slen = uc(index + 5, mem);
				index += 6;
				if (l < index + slen + 1) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 27, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					delete ii;
					return nullptr;
				}
				char *st = &mem[index];
				string name = string(st, slen);
				index += slen;
				unsigned char flags = uc(index, mem);
				index++;
				ii->push_back(InventoryItem(i, lvl, it, name, flags));
				string model = name + ".obj";
				TileRenderer *tr = nullptr;
				bool exists = loader->exists(model);
				Model *m = loader->loadObj(model, ModelLoader::XZ_CENTER);
				if (m == nullptr) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 28, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					return nullptr;
				}
				if (!exists) {
					models.push_back(m);
					renderers.push_back(tr = new TileRenderer(m));
				}
				else {
					printf("Invalid file! Err%u (%s) (#%s)\n", 29, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					return nullptr;
				}
				if (l < index + 12) {
					printf("Invalid file! Err%u (%s) (#%s)\n", 30, file.c_str(), BinaryHelper::asHex(index).c_str());
					delete[] mem;
					delete cats;
					delete res;
					delete is;
					delete ii;
					return nullptr;
				}
				unsigned int xf = ui(index, mem);
				unsigned int yf = ui(index+4, mem);
				unsigned int zf = ui(index+8, mem);
				index += 12;
				float x = *reinterpret_cast<float*>(&xf);
				float y = *reinterpret_cast<float*>(&yf);
				float z = *reinterpret_cast<float*>(&zf);
				ts->assign(i | 0x80000000, nullptr, tr, vec3(x, y, z));
				if (log)printf("Creating inventory item with %u %u %u %p %s %u\n", i, lvl, item, it, name.c_str(), flags);
			}
			printf("Finished loading file data!\n");
			delete[] mem;
			allResources = new TUCResources();
			for(unsigned int i=0;i<res->size();i++)allResources->resources.push_back(TUCResources::ResCon(&(*res)[i], 0));
			return new oiTUC(cats, res, is, ii);
		}
	};
}