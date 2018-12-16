#pragma once
#include "vec2.h"
#include "vec3.h"
#include "Texture.h"
#include "Shader.h"
#include <string>
#include <vector>
#include <ctime>
using namespace std;


namespace osomi {
	class GUIButton;
	static void nothing(float x, float y, GUIButton *b) {}
	class GUIHandler;
	class GUIButton {
		friend class GUIHandler;
	private:
		unsigned int id, idOff, item;
		vec2 scale, offset;
		vec2 uvScale, uvOffset;
		Texture *t;
		vector<GUIButton*> guis;
		void(*onClick)(float, float, GUIButton*);
		GUIButton *parent;
		vector<float> data;
		unsigned int rotate;
		vec3 itemRotation;
		bool visible;
		void bind(GLuint vbo, GLuint vertex, GLuint indices) {
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBindVertexArray(vertex);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		}
	public:
		GUIButton(void(*_onClick)(float x, float y, GUIButton *b), Texture *_t, unsigned int _item, unsigned int dataSize, vec2 _scale = vec2(1, 1), vec2 _offset = vec2(0, 0), vec2 _uvScale = vec2(1, 1), vec2 _uvOffset = vec2(0, 0)) : id(0), scale(_scale), offset(_offset), uvScale(_uvScale), uvOffset(_uvOffset), t(_t), onClick(_onClick), item(_item), parent(nullptr), rotate(0), itemRotation(vec3()) {
			idOff = 0;
			data = vector<float>(dataSize);
			visible = true;
		}
		~GUIButton() {
			for (unsigned int i = 0; i < guis.size(); i++)delete guis[i];
		}
		void addSubUI(GUIButton *g) {
			if (g != nullptr) {
				g->setId(++idOff);
				g->scale = vec2(g->scale.x * scale.x, g->scale.y * scale.y);
				g->offset = vec2(g->offset.x + g->scale.x - scale.x + offset.x, g->offset.y + offset.y);
				g->parent = this;
				guis.push_back(g);
			}
		}
		void setData(unsigned int i, float j) {
			data[i-1] = j;
		}
		void setItem(unsigned int i) {
			item = i;
		}
		void setRotate(unsigned int j) {
			rotate = j;
		}
		void hide() {
			visible = false;
		}
		void show() {
			visible = true;
		}
		void toggle() {
			visible = !visible;
		}
		vec2 getScale() { return scale; }
		vec2 getPos() { return offset; }
		unsigned int getId() { return id; }
		GUIButton *getParent() { return parent; }
		float getData(unsigned int i) {
			return data[i-1];
		}
		unsigned int getItem() { return item; }
		GUIButton *getSub(unsigned int id) {
			if (id >= idOff)return nullptr;
			return guis[id];
		}
		bool isVisible() { return visible; }
		unsigned int totalSubs() { return guis.size(); }
	protected:
		void click(float x, float y, bool &activated) {
			if (!visible)return;
			for (unsigned int i = 0; i < guis.size() && !activated; i++)
				guis[i]->click(x, y, activated);
			if (t == nullptr || activated || onClick == nullptr)return;
			vec2 s = -scale + offset;
			if (s == vec2(-0.f, -0.f))s = vec2::zero();
			vec2 e = scale + offset;
			vec2 pos = vec2((x - s.x) / (e.x - s.x), (y - s.y) / (e.y - s.y));
			activated = pos.x >= 0 && pos.y >= 0 && pos.x < 1 && pos.y < 1;
			if (activated) 
				(*onClick)(pos.x, pos.y, this);
		}
		void draw(Shader *s, Shader *s2, TileSet *ts, unsigned int vertices, GLuint vbo, GLuint vertex, GLuint indices) {
			if (!visible)return;
			if (t != nullptr) {
				s->use();
				mat4 m = mat4::translate(vec3(offset.x, offset.y, 0)) * mat4::scale(vec3(scale.x, scale.y, 1));
				s->setMVP(m, mat4(), mat4(), vec3());
				glUniform2fv(glGetUniformLocation(s->getId(), "uvScale"), 1, &uvScale[0]);
				glUniform2fv(glGetUniformLocation(s->getId(), "uvOffset"), 1, &uvOffset[0]);
				t->bind();
				bind(vbo, vertex, indices);
				glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_SHORT, (char*)0);
			}
			if (item != 0) {
				glEnable(GL_DEPTH_TEST);
				glClear(GL_DEPTH_BITS);
				glDisable(GL_CULL_FACE);
				s2->use();
				mat4 p = mat4::ortho(-1, 1, -1, 1, 0.1, 100);
				vec3 eye = vec3(1, 1, 50);
				mat4 v = mat4::lookat(eye, vec3(0, 0, 0), vec3(0, 1, 0));
				mat4 m = mat4::scale(ts->getScale(item)) * mat4::rotate(itemRotation);
				s2->setMVP(m, v, p, eye);
				vec2 offset = vec2(this->offset.x, this->offset.y);
				glUniform2fv(glGetUniformLocation(s2->getId(), "offset"), 1, &offset[0]);
				vec2 scale = this->scale / 1.5;
				glUniform2fv(glGetUniformLocation(s2->getId(), "scale"), 1, &scale[0]);
				if(!(item & 0x80000000))ts->render(item - 1, Model::ALL);
				else ts->render(item, Model::ALL);
				glEnable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
			}
			for (unsigned int i = 0; i < guis.size(); i++)
				guis[i]->draw(s, s2, ts, vertices, vbo, vertex, indices);
		}
		void update(float delta) {
			if (!visible)return;
			for (unsigned int i = 0; i < guis.size(); i++)
				guis[i]->update(delta);
			if(item != 0 && (parent == nullptr || parent->rotate == 0 || parent->getData(parent->rotate) == 0 || (int)parent->getData(parent->rotate) == id))itemRotation += vec3(0, delta, 0);
		}
		void setId(unsigned int _id) {
			if (id != 0)return;
			id = _id;
		}
	};
}