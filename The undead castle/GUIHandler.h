#pragma once
#include "Shader.h"
#include "GUIButton.h"
#include <vector>
#include <string>
using namespace std;
namespace osomi {
	class GUIHandler {
	private:
		Shader *s, *s2;
		vector<GUIButton*> guis;
		unsigned int guiNum;
		GLuint vertex, vbo, indices;
		unsigned int vert, vertices;
		bool invisible;
		GUIHandler(Shader *_s, Shader *_s2, GLuint _vertex, GLuint _vbo, GLuint _indices, unsigned int _vert, unsigned int _vertices) : s(_s), s2(_s2), vertex(_vertex), vbo(_vbo), indices(_indices), vert(_vert), vertices(_vertices), invisible(false) {}
	public:
		~GUIHandler() {
			delete s;
			for (unsigned int i = 0; i < guis.size(); i++)delete guis[i];
			glDeleteBuffers(vert * sizeof(GLfloat), &vbo);
			glDeleteBuffers(vertices * sizeof(GLushort), &indices);
			glDeleteVertexArrays(1, &vertex);
		}
		GUIButton *attach(GUIButton *g) {
			if (g != nullptr) {
				g->setId(++guiNum);
				guis.push_back(g);
			}
			return g;
		}
		void detach(unsigned int id) {
			for (int i = guis.size() - 1; i >= 0; i--)
				if (guis[i]->getId() == id) {
					delete guis[i];
					guis.erase(guis.begin() + i);
				}
		}
		void render(TileSet *ts) {
			if (invisible)return;
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			for (unsigned int i = 0; i < guis.size(); i++) 
				guis[i]->draw(s, s2, ts, vertices, vbo, vertex, indices);
			glDisable(GL_BLEND);
		}
		void update(float delta) {
			if (invisible)return;
			for (unsigned int i = 0; i < guis.size(); i++)
				guis[i]->update(delta);
		}
		void click(float x, float y, bool &activated) {
			if (invisible)return;
			for (unsigned int i = 0; i < guis.size() && !activated; i++)
				guis[i]->click(x, y, activated);
		}
		static GUIHandler *create(string vert, string frag, string vert2, string frag2) {
			Shader *s = Shader::createShader(vert, frag);
			if (s == nullptr) return nullptr;
			Shader *s2 = Shader::createShader(vert2, frag2);
			if (s == nullptr) return nullptr;
			GLfloat vertexData[] = {
				-1, -1,
				0, 0,

				1, -1,
				1, 0,

				1, 1,
				1, 1,

				-1, 1,
				0, 1
			};
			GLushort indices[] = {
				0,1,2, 2,3,0
			};
			unsigned int M = 16, N = 6;

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, M * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			GLsizei posl = sizeof(GLfloat) * 2;
			GLsizei uvl = sizeof(GLfloat) * 2;
			GLsizei stride = posl + uvl;

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (char*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (char*)posl);

			GLuint indexId;
			glGenBuffers(1, &indexId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * N, indices, GL_STATIC_DRAW);
			return new GUIHandler(s, s2, vao, vbo, indexId, M, N);
		}
		void hide() {
			invisible = true;
		}
		void show() {
			invisible = false;
		}
		void toggle() {
			invisible = !invisible;
		}
		bool isVisible() {
			return !invisible;
		}
	};
}