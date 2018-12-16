#pragma once
#include "vec2.h"
#include "Resources.h"
#include "GUIHandler.h"
namespace osomi {
	class Inventory {
	private:
		GUIHandler *handler;
		GUIButton *inventory;
		unsigned int width, height;
		static void slot(float xperc, float yperc, GUIButton *b) {
			b->getParent()->setData(1, b->getItem());
			b->getParent()->setData(2, b->getId());
		}
	public:
		Inventory(GUIHandler *gh, unsigned int w, bool inverse=false) : width(w), height(1), handler(gh) {
			if (width < 1)width = 1;
			if (height < 1)height = 1;
			vec2 scale = vec2(1 * .5f, 0.25f * .5f); 
			vec2 offset = vec2(0, -1 + 1 / 8.f);
			if (inverse) {
				scale = scale.swap();
				offset = offset.swap();
				offset.x = 2 - scale.x + offset.x;
			}
			inventory = gh->attach(new GUIButton(nothing, Resources::loadTexture("UIBar.png"), 0, 2, scale, offset));
			inventory->setRotate(2);
			inventory->setData(1, 1);
			inventory->setData(2, 1);
			
			for (int j = height-1; j >= 0; j--){
				for (unsigned int i = 0; i < width; i++) {
					scale = vec2(0.125f / 2, 1 / 2.f);
					offset = vec2((inverse ? (float)width - 1 - i : i) / width + 1 / 36.f - 1 / 144.f, 1 / 8);
					if (inverse) {
						scale = scale.swap();
						scale.x /= 2;
						scale.y *= 2;
						offset = offset.swap();
						offset.y = offset.y - 1/3.f - 1/12.f;
						offset.x = offset.x + 1 / 32.f;
					}
					GUIButton *sl = new GUIButton(slot, Resources::loadTexture("UISlot.png"), 0, 0, scale, offset);
					inventory->addSubUI(sl);
				}
			}
		}
		~Inventory() {
			handler->detach(inventory->getId());
		}
		void set(unsigned int i, unsigned int j, unsigned int item) {
			if (i >= width || j >= height)return;
			unsigned int id = j * width + i;
			inventory->getSub(id)->setItem(item);
		}
		unsigned int getSelected() {
			return Math::max(inventory->getData(2), 0);
		}
		unsigned int get(unsigned int i) {
			if (i > width || i == 0)return 0;
			i -= 1;
			return inventory->getSub(i)->getItem();
		}
		bool isVisible() {
			return inventory != nullptr && inventory->isVisible();
		}
		void offset(float f) {
			float n = inventory->getData(2) + f;
			while(n < 1)n += inventory->totalSubs();
			while(n >= inventory->totalSubs()+1)n -= inventory->totalSubs();
			inventory->setData(2, n);
			inventory->setData(1, inventory->getSub(n-1)->getItem());
		}
	};
}