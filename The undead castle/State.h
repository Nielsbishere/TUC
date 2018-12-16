#pragma once
#include "Shader.h"
#include "LightEffect.h"
#include "Camera.h"
#include "GameObject.h"
#include "ModelLoader.h"
#include "World.h"
#include "vec3.h"
#include "Skybox.h"
#include "Raycast.h"
#include "GUIHandler.h"

#include "KeyHandler2D.h"
#include "Resources.h"
#include "Text.h"
#include "MeshWorld.h"

namespace osomi {
	class State {
	private:
		float width, height;
		float mouseX, mouseY;

		GUIHandler *ui;
		KeyHandler kh;

		Shader *s;
		vec3 ambientLight;
		vector<Light> lights;
		vector<LightEffect> effects;

		vector<Model*> models;
		vector<Object*> objects;
		vector<GameObject*> go;
		ModelLoader *loader;
		TileRenderer *cubeRenderer;
		vector<TileRenderer*> renderers;

		World *w;
		MeshWorld *mw;
		TextHandler *th;

		bool isFogEnabled;
		vec3 fogColor;
		float fogDensity;

		unsigned int type;

		Skybox *cubeMap;

		void sendMatrix(mat4 m) {
			if (s == nullptr)return;
			s->setMVP(m, c->getView(), c->getProjection(), c->getEye());
		}
		void renderObject(Object *o, unsigned int sides) {
			if (o == nullptr)return;
			if (w != nullptr) {
				vec3 mi, ma;
				w->getVisible(mi, ma);
				mat4 m = o->getModel();
				vec4 omi4 = m * o->render.m->getMin();
				vec4 oma4 = m * o->render.m->getMax();
				vec3 omi = vec3(omi4.x, omi4.y, omi4.z);
				vec3 oma = vec3(oma4.x, oma4.y, oma4.z);
				if (!Math::doesIntersect2D(mi, ma, omi, oma))return;
			}
			sendMatrix(o->getModel());
			o->render.render((Model::EModelSide)sides, nullptr);
		}
	protected:
		Camera *c;
		TileSet *ts;
		unsigned int getWorldType() { return type; }
		void setAmbient(vec3 ambient) {
			ambientLight = ambient;
		}
		void setFog(vec3 col, float density) {
			fogColor = col;
			fogDensity = density;
			isFogEnabled = true;
		}
		void setSkyBox(string path, string vert, string frag, vec3 size) {
			if (cubeMap != nullptr)return;
			cubeMap = Skybox::load(path, vert, frag, size);
			if (cubeMap == nullptr)
				printf("Couldn't load a cubemap for skybox!\n");
			else printf("Loaded skybox!\n");
		}
		void initGUI(string vert, string frag, string vert2, string frag2) {
			if (ui != nullptr)return;
			ui = GUIHandler::create(vert, frag, vert2, frag2);
			if (ui == nullptr)printf("Couldn't load the GUIHandler!\n");
			else printf("Loaded GUIHandler!\n");
		}
		GUIButton *addGUI(GUIButton *g) {
			if (ui != nullptr)
				ui->attach(g);
			return g;
		}
		void removeGUI(unsigned int i) {
			if (ui != nullptr)
				ui->detach(i);
		}
		Light *addLight(Light l) {
			lights.push_back(l);
			return &lights[lights.size() - 1];
		}
		TileRenderer *addTileRenderer(TileRenderer *tr) {
			renderers.push_back(tr);
			return tr;
		}

		LightEffect *addLightEffect(vec3 pos, vec3 col1, vec3 col2, float power, float duration, bool isLinear = true, bool isPulsing = false) {
			Light *l = addLight(Light(pos, isPulsing ? col2 : col1, power));
			LightEffect le = LightEffect(l, duration, col1, col2, isLinear, isPulsing);
			effects.push_back(le);
			return &effects[effects.size() - 1];
		}
		bool removeLight(Light *l) {
			for (unsigned int k = 0; k < lights.size(); k++)
				if (&lights[k] == l) {
					lights.erase(lights.begin() + k);
					return true;
				}
			return false;
		}
		bool removeLightEffect(LightEffect *le) {
			for (unsigned int k = 0; k < effects.size(); k++)
				if (&effects[k] == le) {
					removeLight(effects[k].getLight());
					effects.erase(effects.begin() + k);
					return true;
				}
			return false;
		}
		Model *loadModel(string file, ModelLoader::EScalingType scaleType) {
			if (loader->exists(file))
				return loader->loadObj(file, scaleType);
			double t = glfwGetTime();
			Model *m = loader->loadObj(file, scaleType);
			if (m == nullptr) {
				printf("Couldn't load a model!\n");
				return nullptr;
			}
			printf("Loaded model \"%s\" within %fs\n", file.c_str(), (float)(glfwGetTime() - t));
			models.push_back(m);
			return m;
		}
		World *getWorld() {
			return w;
		}
		void setWorld(unsigned int w, unsigned int h, unsigned int l, TileSet *ts) {
			if (this->w != nullptr)
				return;
			this->w = new World(w, h, l, type, c, s, ts);
		}
		void setMeshWorld(string tilemap, string vert = "terrainvert.glsl", string frag = "terrainfrag.glsl") {
			if (this->mw != nullptr)return;
			this->mw = MeshWorld::generate(tilemap, vert, frag);
		}
		Object *addObject(Object ob) {
			objects.push_back(new Object(ob));
			return objects[objects.size() - 1];
		}
		TileRenderer *getCubeRenderer() { return cubeRenderer; }
		GameObject *addObject(Object o, GameObject *g) {
			Object *obj = addObject(o);
			obj->link(g);
			g->attachObject(obj);
			go.push_back(g);
			return g;
		}
		void removeObject(Object *o) {
			for (unsigned int i = 0; i < objects.size(); i++) {
				if (objects[i] == o) {
					objects.erase(objects.begin() + i);
					break;
				}
			}
		}
		GameObject *attached(Object *o) {
			return o->getAttached();
		}
		vector<Object*> nonEntity() {
			vector<Object*> obj;
			for (unsigned int i = 0; i < go.size(); i++) {
				if (StringUtils::startsWith(go[i]->getName(), "e_"))continue;
				obj.push_back(go[i]->getObject());
			}
			return obj;
		}
		vector<GameObject*> nonEntityAtt(GameObject *self=nullptr) {
			vector<GameObject*> obj;
			for (unsigned int i = 0; i < go.size(); i++) {
				if (StringUtils::startsWith(go[i]->getName(), "e_"))continue;
				if(self == nullptr || (go[i] != self))obj.push_back(go[i]);
			}
			return obj;
		}
		vector<GameObject*> getClickable() {
			vector<GameObject*> obj;
			for (unsigned int i = 0; i < go.size(); i++) {
				if (go[i] == nullptr || !go[i]->isAlive() || go[i]->getObject() == nullptr || go[i]->getObject()->isGhost())continue;
				obj.push_back(go[i]);
			}
			return obj;
		}
		void pick(vec3 dir, unsigned int &i, unsigned int &j, unsigned int &k, GameObject *&go) {
			vector<GameObject*> clickable = getClickable();
			bool hitsTilemap = false;

			Raycast::raycast(c->getEye(), dir, w != nullptr ? w->tm : nullptr, clickable, hitsTilemap, i, j, k, go);
		}

		float getMouseX() { return mouseX; }
		float getMouseY() { return mouseY; }
		float getWidth() { return width; }
		float getHeight() { return height; }
		ModelLoader *getLoader() { return loader; }
		vector<Model*> &getModels() { return models; }
		vector<TileRenderer*> &getRenderers() { return renderers; }
		GUIHandler *getUIHandler() { return ui; }

		bool clickWorld(vec3 &d) {
			float mx = getMouseX() / getWidth();
			float my = 1 - getMouseY() / getHeight();
			if (!(mx < 0 || my < 0 || mx > 1 || my > 1)) {
				vec4 ogl = vec4(mx * 2 - 1, my * 2 - 1, 1, -1);
				mat4 pinv = c->getProjection();
				if (!pinv.inverse()) {
					printf("Couldn't inverse projection matrix!\n");
					return false;
				}
				vec4 yc = pinv * ogl;
				vec4 eye = vec4(yc.x, yc.y, -1, 0);
				mat4 vinv = c->getView();
				if (!vinv.inverse()) {
					printf("Couldn't inverse view matrix!\n");
					return false;
				}
				vec4 w = vinv * eye;
				vec3 wdir = vec3(w.x, w.y, w.z).normalize();
				d = wdir;
				return true;
			}
			else return false;
		}
		float getKey(KeyHandler::EKeyType t) {
			return kh.get(t);
		}
		void hideAll() {
			if (ui != nullptr)ui->hide();
		}
		void showAll() {
			if (ui != nullptr)ui->show();
		}
		void toggleAll() {
			if (ui != nullptr)ui->toggle();
		}
		TextHandler *getTextHandler() { return th; }
	public:
		State(Shader *_s, string font = "Arial.fnt", unsigned int worldType = 0) : type(worldType) {
			s = _s;
			s->use();
			th = TextHandler::load(font);
			c = new Camera(type);
			loader = new ModelLoader();
			w = nullptr;
			cubeRenderer = new TileRenderer(loader->loadObj("cube.obj", type >= 1 ? ModelLoader::XZ : ModelLoader::XY));
			renderers.push_back(cubeRenderer);
		}
		virtual ~State() {
			if (s != nullptr)delete s;
			if (c != nullptr)delete c;
			delete loader;
			for (unsigned int i = 0; i < renderers.size(); i++)if (renderers[i] != nullptr)delete renderers[i];
			for (unsigned int i = 0; i < go.size(); i++)if (go[i] != nullptr)delete go[i];
			for (unsigned int i = 0; i < objects.size(); i++)if (objects[i] != nullptr)delete objects[i];
			if (w != nullptr)delete w;
			if (cubeMap != nullptr)delete cubeMap;
			if (ui != nullptr)delete ui;
			if (mw != nullptr)delete mw;
		}
		virtual void update(double delta) {
			for (unsigned int i = 0; i < objects.size(); i++)
				objects[i]->update(delta);
			for (int i = go.size() - 1; i >= 0; i--) {
				go[i]->update(delta);
				if (!go[i]->isAlive()) {
					if (go[i]->getObject() != nullptr) {
						for (unsigned int j = 0; j < objects.size(); j++) {
							if (objects[j] == go[i]->getObject()) {
								objects.erase(objects.begin() + j);
								break;
							}
						}
						delete go[i]->getObject();
					}
					delete go[i];
					go.erase(go.begin() + i);
				}
			}
			for (int k = effects.size() - 1; k >= 0; k--)
				if (effects[k].update(delta))
					removeLightEffect(&effects[k]);
			for (unsigned int i = 0; i < go.size(); i++)if (go[i] != nullptr)go[i]->update(delta);
			c->update(delta);
			if (cubeMap != nullptr)cubeMap->update(delta);
			if (ui != nullptr)ui->update(delta);
		}
		virtual void render(int w, int h) {
			if (isFogEnabled)glClearColor(fogColor.x, fogColor.y, fogColor.z, 1);
			if (s == nullptr)return;
			s->use();
			s->setAmbientLight(ambientLight);
			s->setLights(lights);
			s->setFog(isFogEnabled, fogColor, fogDensity);
			c->updateRender(w, h);
			#ifdef _WORLD
				if (this->w != nullptr)
					this->w->render();
			#endif
			if (mw != nullptr && c != nullptr)mw->render(c->getView(), c->getProjection(), c->getEye(), ambientLight, isFogEnabled, fogColor, fogDensity, lights);
			s->use();
			for (unsigned int i = 0; i < objects.size(); i++)
				renderObject(objects[i], Model::ALL);
			if (cubeMap != nullptr)cubeMap->render(c->getView(), c->getProjection(), fogColor);
			glClear(GL_DEPTH_BITS);
			glDisable(GL_DEPTH_TEST);
			if (ui != nullptr)ui->render(ts);
			if (th != nullptr && (ui == nullptr || ui->isVisible()))th->render();
			glEnable(GL_DEPTH_TEST);
		}
		virtual void keypress(int k, int s) {
			kh.handle(k, s);
		}
		virtual void resize(int w, int h) {
			width = w;
			height = h;
		}
		virtual void mousepress(int k, int s, bool &activated) {
			if (ui != nullptr)
				ui->click(mouseX / width * 2 - 1, (1 - mouseY / height) * 2 - 1, activated);
			if (!activated)kh.handleMouse(k, s);
			else kh.handleMouse(k, GLFW_RELEASE);
		}
		void setMousePosition(float x, float y) {
			mouseX = x;
			mouseY = y;
		}
		void setMouseX(float x) {
			mouseX = x;
		}
		void setMouseY(float y) {
			mouseY = y;
		}
		void getMousePos(float &x, float &y) {
			x = mouseX;
			y = mouseY;
		}
	};
}