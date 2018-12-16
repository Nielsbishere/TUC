#pragma once
#include <GL/glew.h>
#include "ModelObjectPart.h"
#include "vec4.h"
#include <string>
using namespace std;
namespace osomi {
	class Model;
	class ModelLoader;
	class ModelObject {
		friend class Model;
		friend class ModelLoader;
	private:
		GLuint vertex, vbo, indices;
		unsigned int vert, vertices;
		vector<ModelObjectPart> part;
		string name;
		float miX, miY, miZ, maX, maY, maZ;
		ModelObject(GLuint _vertex, GLuint _vbo, GLuint _indices, unsigned int _vert, unsigned int _vertices, string _name, float _miX, float _miY, float _miZ, float _maX, float _maY, float _maZ) {
			vertex = _vertex;
			vbo = _vbo;
			indices = _indices;
			vertices = _vertices;
			vert = _vert;
			name = _name;
			miX = _miX;
			miY = _miY;
			miZ = _miZ;
			maX = _maX;
			maY = _maY;
			maZ = _maZ;
		}
		void addPart(ModelObjectPart p) {
			part.push_back(p);
		}
		void render(unsigned int indexStart, unsigned int indexEnd, Texture *rep = nullptr) {
			if ((vertex == 0 && vbo == 0 && indices == 0 && vert == 0 && vertices == 0 && name == "") || rep == nullptr)return;
			rep->bind();
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBindVertexArray(vertex);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
			glDrawElements(GL_TRIANGLES, indexEnd - indexStart, GL_UNSIGNED_SHORT, (char*)(indexStart * sizeof(GLushort)));
		}
	protected:
		void render(Texture *rep=nullptr) {
			if (part.size() == 0 && vertex == 0 && vbo == 0 && indices == 0 && vert == 0 && vertices == 0 && name == "")return;
			if(rep != nullptr)render(0, vertices, rep);
			else {
				for (unsigned int i = 0; i < part.size(); i++) 
					render(part[i].getStart(), part[i].getEnd(), part[i].getTexture());
			}
		}
		void destroy() {
			if (part.size() == 0 && vertex == 0 && vbo == 0 && indices == 0 && vert == 0 && vertices == 0 && name == "")return;
			glDeleteBuffers(vert * sizeof(GLfloat), &vbo);
			glDeleteBuffers(vertices * sizeof(GLushort), &indices);
			glDeleteVertexArrays(1, &vertex);
		}
		static ModelObject glTriangles(GLfloat *triangle, unsigned int M, GLushort *indices, unsigned int N, string name, float miX, float miY, float miZ, float maX, float maY, float maZ) {
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, M * sizeof(GLfloat), triangle, GL_STATIC_DRAW);

			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			GLsizei posl = sizeof(GLfloat) * 3;
			GLsizei coll = sizeof(GLfloat) * 2;
			GLsizei normall = sizeof(GLfloat) * 3;
			GLsizei stride = posl + coll + normall;

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (char*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (char*)posl);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (char*)(posl + coll));

			GLuint indexId;
			glGenBuffers(1, &indexId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * N, indices, GL_STATIC_DRAW);

			ModelObject mo = ModelObject(vao, vbo, indexId, M, N, name, miX, miY, miZ, maX, maY, maZ);
			return mo;
		}
		template<unsigned int M, unsigned int N> static ModelObject glTriangles(GLfloat(&triangle)[M], GLushort(&indices)[N], Texture *t, string name) {
			return glTriangles(triangle, M, indices, N, t, name);
		}
	public:
		string getName() { return name; }
		ModelObject() : ModelObject(0, 0, 0, 0, 0, "", 0, 0, 0, 0, 0, 0) {}
		vec4 getMin() { return vec4(miX, miY, miZ, 1); }
		vec4 getMax() { return vec4(maX, maY, maZ, 1); }
	};
}