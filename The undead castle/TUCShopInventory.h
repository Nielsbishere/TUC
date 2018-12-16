#pragma once
#include "oiTUC.h"
#include "ShopInventory.h"
using namespace osomi;
namespace tuc {
	class TUCShopInventory {
	private:
		ShopInventory *si;
		oiTUC::Category *cat;
		oiTUC *data;
		void initScreen() {
			vector<oiTUC::Category*> cats = data->getCategories(cat);
			if (cat != nullptr) {
				si->show(0, 0);
				si->set(0, 0, cat->getItem());
				vector<oiTUC::Item> items = data->getItems(cat);
				for (unsigned int i = 1; i < 9 && i-1 < items.size(); i++) {
					si->set(i, 0, items[i-1].getItem());
					si->show(i, 0);
				}
			}
			for (unsigned int j = 1; j < 4 && j - 1 < cats.size(); j++) {
				oiTUC::Category *c = cats[j - 1];
				si->show(0, j - (cat == nullptr ? 1 : 0));
				si->set(0, j - (cat == nullptr ? 1 : 0), c->getItem());
				vector<oiTUC::Item> items = data->getItems(c);
				for (unsigned int i = 1; i < 9 && i - 1 < items.size(); i++) {
					si->set(i, j - (cat == nullptr ? 1 : 0), items[i - 1].getItem());
					si->show(i, j - (cat == nullptr ? 1 : 0));
				}
			}
		}
	public:
		TUCShopInventory(GUIHandler *handler, oiTUC *dat): data(dat) {
			si = new ShopInventory(handler);
			for (int j = 3; j >= 0; j--)
				for (int i = 0; i < 9; i++)
					si->hide(i, j);
			cat = nullptr;
			initScreen();
		}
		~TUCShopInventory() {
			delete si;
		}
		ShopInventory *getInventory() { return si; }
	};
}