#pragma once

//#define _TEST
//#define _REMCONSOLE

//#define _CAMLOCK
//#define _FOG

//#define _WORLD

#ifdef _TEST
const bool test = true;
#else
const bool test = false;
#endif

#include "State.h"
#include "oiTUC.h"
#include "Inventory.h"
#include "Entities.h"
#include "TUCShopInventory.h"
using namespace osomi;

//TODO: Optimizations for cube drawing
//TODO: Shadows
//TODO: BLOODMOON?

namespace tuc {
	class GameState : public State {
	private:
		oiTUC *t;
		bool f1, shopOpen;
		Player *player;
		TUCShopInventory *shop;
		Inventory *inventory;
		Inventory *itemInv;
		TUCResources *resources;

		GameObject *bound;
		unsigned int blocksEnd;

		void triggerButtons(float delta, float l, float r, float m) {
			if (b <= 0) {
				vec3 d;
				if (clickWorld(d)) {
					unsigned int i = UINT_MAX, j = i, k = i;
					GameObject *go = nullptr;
					pick(d, i, j, k, go);
					b += .1;
					if (i != UINT_MAX && j != UINT_MAX && k != UINT_MAX) {
						if (k == 0 && l) {
							if (getWorld()->get(i, j, 1) == 0) {
								getWorld()->set(i, j, 1, inventory->get(inventory->getSelected()));
								getWorld()->refresh();
							}
						}
						else if (k == 1 && r) {
							getWorld()->set(i, j, 1, 0);
							getWorld()->refresh();
						}
						else if (k == 0 && m) {
							if (!player->hasPath()) {
								player->followPath(getWorld()->astar(player->getPos(), vec2(i, j)), getWorld()->getWidth(), getWorld()->getHeight(), 2);
							}
						}
					}
					else if (go != nullptr && !player->hasPath()) {
						bound = go;
						player->followPath(getWorld()->astar(player->getPos(), go->getPos()), getWorld()->getWidth(), getWorld()->getHeight(), 2);
					}
				}
			}
			else b -= delta;
		}
	public:
		~GameState() {
			if (t != nullptr)delete t;
		}

		Light *testL;
		GameState() : State(Shader::createShader("vert.glsl", "frag.glsl"), "Arial.fnt", 1) {
			//Setting visuals (ambient light, fog, skybox and UI).
			if (!test) {
				setAmbient(vec3(0.4f, 0.4f, 0.4f));
				#ifdef _FOG
					setFog(vec3(0.1f, 0.1f, 0.1f), 0.05f);
				#endif
			}
			else {
				setAmbient(vec3(1, 1, 1));
				setFog(vec3(0.1f, 0.1f, 0.1f), 0.f);
			}
			setSkyBox("night", "skyboxvert.glsl", "skyboxfrag.glsl", vec3(50, 50, 50.f));
			initGUI("uivert.glsl", "uifrag.glsl", "uvvert.glsl", "uvfrag.glsl");

			if(!test)testL = addLight(Light(vec3(0, 30, 0), vec3(1, 1, 1), 100));

			//Creating world and loading tileset with data
			t = oiTUC::load("Data.oiTUC", getCubeRenderer(), getLoader(), getModels(), getRenderers(), ts = new TileSet(), resources);
			blocksEnd = 9;
			setWorld(25, 25, 2, ts);

			//Inventory UI
			inventory = new Inventory(getUIHandler(), 9);
			for (unsigned int i = 0; i < 9; i++)
				inventory->set(i, 0, i == 0 ? blocksEnd + 1 : i == 1 ? blocksEnd + 3 : i % 9 + 1);

			//Player UI
			itemInv = new Inventory(getUIHandler(), 5, true);
			for (unsigned int i = 0; i < 5; i++)
				itemInv->set(i, 0, 0x80000000 + i % 5 + 4);

			//Shop UI
			shop = new TUCShopInventory(getUIHandler(), t);

			Model *warriorObj = loadModel("Player.obj", ModelLoader::NONE);
			Object o = Object(Renderer(warriorObj), vec3(-2, -0.99f, -2));
			o.setSize(vec3(1.f / 100, 1.f / 100, 1.f / 100));
			player = (Player*)addObject(o, new Player("Claus"));
			#ifdef _CAMLOCK
				if(!test)
					c->attachGameObject(player);
			#endif

			bound = nullptr;
			getWorld()->set(11, 11, 1, 1);
			getWorld()->set(10, 11, 1, 1);
			getWorld()->set(11, 10, 1, 1);
			getWorld()->set(10, 10, 1, 1);

			for (unsigned int i = 0; i < 9; i++) {
				unsigned int b = rand() % 3;
				GameObject *go = b == 1 ? (GameObject*)(new Tree()) : (b == 2 ? (GameObject*)(new Iron()) : (GameObject*)(new Rock()));
				const char *name = b == 1 ? "Tree.obj" : (b == 2 ? "Iron.obj" : "Rock.obj");
				addObject(Object(loadModel(name, ModelLoader::XZ), vec3(5 + 2 * i, 0, 10)), go);
			}

			getWorld()->init();

			setMeshWorld("Grass.jpg");
		}
		float b = 0;
		void update(double delta) {
			State::update(delta);

			float hor = getKey(KeyHandler::HORIZONTAL);
			float vert = getKey(KeyHandler::VERTICAL);
			float up = getKey(KeyHandler::UP);

			bool toggleShop = getKey(KeyHandler::R);
			bool toggleInvs = getKey(KeyHandler::F1);

			if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
				int count;
				const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
				int bcount;
				const unsigned char *bts = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &bcount);

				//Cross controller (xbox, playstation)
				bool isxbox = count == 6 && bcount == 14;
				bool invertY = !isxbox;

				float xval = 0;
				float yval = 0;
				float mxval = 0;
				float myval = 0;

				int xMouse = 0, yMouse = 1, mxMouse = 2, myMouse = !isxbox ? 5 : 3;

				//Start; 7 XBOX or 9 PS is escape (Start/options)
				int headUp = isxbox ? 8 : 10, pressCursor = headUp + 1, leftClick = 4, rightClick = 5;

				float mxRange = 1;

				//Check buttons
				bool headingUp = bts[headUp];
				bool pressingCursor = bts[pressCursor];
				bool pressLeft = bts[leftClick];
				bool pressRight = bts[rightClick];

				float inaccuraccy = !isxbox ? 0.05f : 0.01f;

				//Check axes
				for (int i = 0; i < count; i++) {
					if (i == xMouse)
						xval += abs(axes[i]) < inaccuraccy ? 0 : axes[i];
					if (i == yMouse)
						yval += (invertY ? -1 : 1) * (abs(axes[i]) < inaccuraccy ? 0 : axes[i]);
					if (i == mxMouse)
						mxval += (abs(axes[i]) < inaccuraccy ? 0 : axes[i]) / mxRange;
					if (i == myMouse)
						myval += abs(axes[i]) < inaccuraccy ? 0 : (invertY ? 1 : -1) * axes[i];
				}

				/*for (int b = 0; b < bcount; b++) {
					printf("%u %i\n", bts[b], b);
				}
				printf("\n\n");*/

				//X Y movement
				if (!headingUp) {
					if (hor != 0 && xval != 0)
						hor = (hor + xval) / 2;
					else if (xval != 0)
						hor = xval;

					if (vert != 0 && yval != 0)
						vert = (vert + yval) / 2;
					else if (yval != 0)
						vert = yval;
				}
				else {
					if (up != 0 && yval != 0)
						up = (up + yval) / 2;
					else if (yval != 0)
						up = yval;
				}

				//Mouse movement
				if (!pressingCursor) {
					float speedX = 1.0 / .5f;
					float nx = mxval * (float)delta * speedX * getWidth() + getMouseX();
					if (nx < 8)
						setMouseX(8);
					else if (nx > getWidth() - 8)
						setMouseX(getWidth() - 8);
					else
						setMouseX(nx);

					float speedY = 1.0 / .5f;
					float ny = myval * (float)delta * speedY * getHeight() + getMouseY();
					if (ny < 8)
						setMouseY(8);
					else if (ny > getHeight() - 8)
						setMouseY(getHeight() - 8);
					else
						setMouseY(ny);
				}
				else {
					float bigVal = abs(mxval) > abs(myval) ? mxval : myval;
					if (abs(bigVal) > inaccuraccy) {
						bool ii = abs(mxval) <= abs(myval);
						if (ii) {
							if (itemInv->isVisible() && getUIHandler()->isVisible())
								itemInv->offset(bigVal * delta);
						}
						else {
							if (inventory->isVisible() && getUIHandler()->isVisible()) 
								inventory->offset(bigVal * delta);
						}
					}
				}
				
				//Clicking
				if (pressLeft || pressRight) {
					bool active = false;
					if (getUIHandler() != nullptr)getUIHandler()->click(getMouseX() / getWidth() * 2 - 1, (1 - getMouseY() / getHeight()) * 2 - 1, active);
					if(!active)triggerButtons(delta, pressLeft, pressRight, 0);
				}

				if (bts[3])
					toggleInvs = true;
				if (bts[2])
					toggleShop = true;
			}

			c->move(vec3(hor * delta * 5, up * delta * 5, vert * delta * 5));
			if (getWorldType() == 1)c->changeZoom(up * delta);

			if (c->getEye().y < 3)c->setEye(vec3(c->getEye().x, 3, c->getEye().z));
			if (getKey(KeyHandler::LEFT) || getKey(KeyHandler::RIGHT) || getKey(KeyHandler::MIDDLE)) {
				triggerButtons(delta, getKey(KeyHandler::LEFT), getKey(KeyHandler::RIGHT), getKey(KeyHandler::MIDDLE));
			}
			if (getKey(KeyHandler::V)) {
				player->getObject()->shake(5, 1, 5);
				printf("Wiggle wiggle wiggle wiggle\n");
				player->move(vec3(0, delta, 0));
			}
			if (bound != nullptr) {
				if (!player->hasPath()) {
					if (instanceof<Damagable>(bound)) {
						//string str = t->getItem(itemInv->get(itemInv->getSelected()))->getName();
						bool shouldDamage = true;
						if (instanceof<Tree>(bound)) {
							if (itemInv->get(itemInv->getSelected()) != 0x80000007)
								shouldDamage = false;
						}
						if (instanceof<Rock>(bound) || instanceof<Iron>(bound)) {
							if (itemInv->get(itemInv->getSelected()) != 0x80000008)
								shouldDamage = false;
						}
						printf("%u %u\n", itemInv->getSelected(), itemInv->get(itemInv->getSelected()));
						if (shouldDamage) {
							((Damagable*)bound)->damage(player->getDamage());
							if (!bound->isAlive()) {
								if (instanceof<Tree>(bound)) resources->add("Wood", 0x4);
								if (instanceof<Rock>(bound)) resources->add("Stone", 0x2);
								if (instanceof<Iron>(bound)) resources->add("Iron", 0x1);
								bound = nullptr;
							}
						}
					}
				}
			}
			if (toggleInvs) {
				if (!f1) {
					toggleAll();
					f1 = true;
				}
			}
			else f1 = false;
			if (toggleShop) {
				if (!shopOpen) {
					shop->getInventory()->toggle();
					shopOpen = true;
				}
			}
			else shopOpen = false;
		}
	};
}