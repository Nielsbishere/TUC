#pragma once
#include <GL/glew.h>
#include <SOIL/SOIL2.h>
#include <string>
#include "Shader.h"
using namespace std;
namespace osomi {
	class Skybox {
	private:
		vec3 size, rotation;
		GLuint skybox;
		Shader *s;
		GLuint vertex, vbo, indices;
		unsigned int vert, vertices;
		Skybox(GLuint _skybox, Shader *_s, GLuint _vertex, GLuint _vbo, GLuint _indices, unsigned int _vert, unsigned int _vertices, vec3 _size): skybox(_skybox), s(_s), vertex(_vertex), vbo(_vbo), indices(_indices), vert(_vert), vertices(_vertices), size(_size){}
	public:
		~Skybox() {
			delete s;
			glDeleteBuffers(vert * sizeof(GLfloat), &vbo);
			glDeleteBuffers(vertices * sizeof(GLushort), &indices);
			glDeleteVertexArrays(1, &vertex);
		}
		void render(mat4 v, mat4 p, vec3 fogColor) {
			s->use();
			mat4 view = v;
			view.setTranslate(vec3::zero());
			s->setMVP(mat4::rotateDeg(rotation) * mat4::scale(size), view, p, vec3());
			s->setFog(true, fogColor, NULL);
			glCullFace(GL_FRONT);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBindVertexArray(vertex);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
			glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_SHORT, (char*)0);
			glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			glCullFace(GL_BACK);
			glDepthFunc(GL_LESS);
		}
		void update(float delta) {
			rotation += vec3(0, delta/8, 0);
		}
		static Skybox *load(string path, string vert, string frag, vec3 renderDistance) {
			GLuint map = SOIL_load_OGL_cubemap(
				(path + "_l.png").c_str(),
				(path + "_r.png").c_str(),
				(path + "_u.png").c_str(),
				(path + "_d.png").c_str(),
				(path + "_f.png").c_str(),
				(path + "_b.png").c_str(),
				SOIL_LOAD_RGB,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS
			);
			if (map == 0)return nullptr;
			Shader *s = Shader::createShader(vert, frag);
			if (s == nullptr) return nullptr;
			
			GLfloat vertexData[] = {
				-1, -1, 1,
				1, -1, 1,
				1, 1, 1,
				-1, 1, 1,

				-1, -1, -1,
				1, -1, -1,
				1, 1, -1,
				-1, 1, -1
			};
			GLushort indices[] = {
				0,1,2, 2,3,0,	//Front
				4,7,6, 6,5,4,	//Back
				1,5,6, 6,2,1,	//Right
				0,3,7, 7,4,0,	//Left
				3,2,6, 6,7,3,	//Up
				0,4,5, 5,1,0	//Down
			};
			unsigned int M = 24, N = 36;

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, M * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (char*)0);

			GLuint indexId;
			glGenBuffers(1, &indexId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * N, indices, GL_STATIC_DRAW);

			return new Skybox(map, s, vao, vbo, indexId, M, N, renderDistance);
		}
	};
}