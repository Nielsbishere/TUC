#pragma once

#include "State.h"

namespace osomi {
	class StateManager {
	private:
		State *st;
	public:
		StateManager() {}
		~StateManager() {
			destroy();
		}
		void resize(int w, int h) {
			if (st != nullptr)st->resize(w, h);
		}
		void update(double delta) {
			if (st != nullptr)st->update(delta);
		}
		void render(int w, int h) {
			if (st != nullptr)st->render(w, h);
		}
		void keypress(int key, int state) {
			if (st != nullptr)st->keypress(key, state);
		}
		void mousepress(int key, int state) {
			bool press = false;
			if (st != nullptr)st->mousepress(key, state, press);
		}
		void setMousePosition(float x, float y) {
			if (st != nullptr)st->setMousePosition(x, y);
		}
		void setState(State *s) {
			if (st != nullptr) {
				delete st;
				st = nullptr;
			}
			st = s;
		}
		void getMouse(float &x, float &y) {
			if (st != nullptr)st->getMousePos(x, y);
		}
	protected:
		void destroy() {
			if (st != nullptr)delete st;
			st = nullptr;
		}
	};
};