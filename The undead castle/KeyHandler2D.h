#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
using namespace std;
namespace osomi {
	class KeyHandler {
	public:
		enum EKeyType {
			HORIZONTAL, VERTICAL, UP, LEFT, RIGHT, MIDDLE, F1, R, V
		};
	private:
		enum EResult {
			NEG, POS
		};
		struct Key {
			bool press;
			int k;
			EResult res;
			EKeyType macro;
			Key(int key, EResult result, EKeyType name) : k(key), res(result), macro(name), press(false) {}
		};
		vector<Key> keys;
		vector<Key> mouse;
		void listen(int k, EResult res, EKeyType macro) {
			keys.push_back(Key(k, res, macro));
		}
		void listenMouse(int k, EResult res, EKeyType macro) {
			mouse.push_back(Key(k, res, macro));
		}
	public:
		void listen(EKeyType t) {
			switch (t) {
			case HORIZONTAL:
				listen(GLFW_KEY_A, NEG, HORIZONTAL);
				listen(GLFW_KEY_LEFT, NEG, HORIZONTAL);
				listen(GLFW_KEY_D, POS, HORIZONTAL);
				listen(GLFW_KEY_RIGHT, POS, HORIZONTAL);
				break;
			case VERTICAL:
				listen(GLFW_KEY_W, POS, VERTICAL);
				listen(GLFW_KEY_UP, POS, VERTICAL);
				listen(GLFW_KEY_S, NEG, VERTICAL);
				listen(GLFW_KEY_DOWN, NEG, VERTICAL);
				break;
			case UP:
				listen(GLFW_KEY_SPACE, POS, UP);
				listen(GLFW_KEY_LEFT_SHIFT, NEG, UP);
				break;
			case F1:
				listen(GLFW_KEY_F1, POS, F1);
			case R:
				listen(GLFW_KEY_R, POS, R);
				break;
			case V:
				listen(GLFW_KEY_V, POS, V);
				break;
			case LEFT:
				listenMouse(GLFW_MOUSE_BUTTON_LEFT, POS, LEFT);
				break;
			case MIDDLE:
				listenMouse(GLFW_MOUSE_BUTTON_MIDDLE, POS, MIDDLE);
				break;
			case RIGHT:
				listenMouse(GLFW_MOUSE_BUTTON_RIGHT, POS, RIGHT);
				break;
			default:
				return;
			}
		}
		KeyHandler() {
			listen(KeyHandler::HORIZONTAL);
			listen(KeyHandler::VERTICAL);
			listen(KeyHandler::UP);
			listen(KeyHandler::F1);
			listen(KeyHandler::R);
			listen(KeyHandler::V);
			listen(KeyHandler::LEFT);
			listen(KeyHandler::MIDDLE);
			listen(KeyHandler::RIGHT);
		}
		float get(EKeyType k, bool clamp=true) {
			if (k == RIGHT || k == LEFT || k == MIDDLE) {
				for (unsigned int i = 0; i < mouse.size(); i++) {
					if (!mouse[i].press || mouse[i].macro != k)continue;
					return 1;
				}
			}
			float f=0;
			for (unsigned int i = 0; i < keys.size(); i++) {
				if (!keys[i].press || keys[i].macro != k)continue;
				f += keys[i].res == NEG ? -1 : 1;
			}
			if (clamp) {
				if (f < -1)f = -1;
				else if (f > 1)f = 1;
			}
			return f;
		}
		void handle(int k, int s) {
			bool b = s != GLFW_RELEASE;
			for (unsigned int i = 0; i < keys.size(); i++) {
				if (keys[i].k != k)continue;
				keys[i].press = b;
			}
		}
		void handleMouse(int k, int s) {
			bool b = s != GLFW_RELEASE;
			for (unsigned int i = 0; i < mouse.size(); i++) {
				if (mouse[i].k != k)continue;
				mouse[i].press = b;
			}
		}
	};
}