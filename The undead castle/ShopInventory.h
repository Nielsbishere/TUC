#pragma once
#include "vec2.h"
#include "Resources.h"
#include "GUIHandler.h"
namespace osomi {
	class ShopInventory {
	private:
		GUIHandler *handler;
		GUIButton *inventory;
		unsigned int width, height;
		static void shopSlot(float xperc, float yperc, GUIButton *b) {
			b->getParent()->setData(1, b->getItem());
			b->getParent()->setData(2, b->getId());
		}
	public:
		ShopInventory(GUIHandler *gh) : width(9), height(4), handler(gh) {
			inventory = gh->attach(new GUIButton(nothing, Resources::loadTexture("UIBar.png"), 0, 2, vec2(1 * .5f, .5f), vec2(0, 1 / 8.f)));
			inventory->setRotate(2);
			inventory->setData(1, 1);
			inventory->setData(2, 1);

			for (int j = height-1; j >= 0; j--) {
				for (unsigned int i = 0; i < width; i++) {
					GUIButton *sl = new GUIButton(shopSlot, Resources::loadTexture("UISlot.png"), 0, 0, vec2(0.125f / 2, 1 / 8.f), vec2(i / 9.f + 1 / 36.f - 1 / 144.f, -3 / 8.f + j / 4.f));
					inventory->addSubUI(sl);
				}
			}
			hide();
		}
		~ShopInventory() {
			handler->detach(inventory->getId());
		}
		void hide() {
			inventory->hide();
		}
		void toggle() {
			inventory->toggle();
		}
		void show() {
			inventory->show();
		}
		void hide(unsigned int i, unsigned int j) {
			if (i >= width || j >= height)return;
			inventory->getSub(j * width + i)->hide();
		}
		void show(unsigned int i, unsigned int j) {
			if (i >= width || j >= height)return;
			inventory->getSub(j * width + i)->show();
		}
		void toggle(unsigned int i, unsigned int j) {
			if (i >= width || j >= height)return;
			inventory->getSub(j * width + i)->toggle();
		}
		void set(unsigned int i, unsigned int j, unsigned int item) {
			if (i >= width || j >= height)return;
			inventory->getSub(j * width + i)->setItem(item);
		}
	};
}