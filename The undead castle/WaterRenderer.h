#pragma once
#include "Shader.h"
#include "mat4.h"

//TODO: Make it work
namespace osomi {
	class WaterRenderer {
	private:
		Shader *s;
		GLuint vbo, vao, indices;
		vec3 translate, scale;
		WaterRenderer(Shader *_s, GLuint _vbo, GLuint _vao, GLuint _indices) : s(_s), vao(_vao), vbo(_vbo), indices(_indices), translate(vec3()), scale(vec3()){}
		mat4 getModel() {
			return mat4::translate(translate) * mat4::scale(scale);
		}
	public:
		~WaterRenderer() {
			glDeleteBuffers(8 * 4 * sizeof(GLfloat), &vbo);
			glDeleteBuffers(6 * sizeof(GLushort), &indices);
			glDeleteVertexArrays(1, &vao);
		}
		void render(mat4 v, mat4 p, vec3 eye, vec3 ambient, bool enableFog, vec3 fogColor, float fogDensity, vector<Light> &lights) {
			s->use();
			s->setAmbientLight(ambient);
			s->setFog(enableFog, fogColor, fogDensity);
			s->setLights(lights);
			s->setMVP(getModel(), v, p, eye);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (char*)0);
			printf("Rendering!\n");
		}
		static WaterRenderer *create(string vert, string frag) {
			Shader *s = Shader::createShader(vert, frag);
			if (s == nullptr) {
				printf("Couldn't load the water! Couldn't load the shader!\n");
				return nullptr;
			}
			GLfloat vertexData[] = {
				-1, 0, -1,
				0, 0,
				0, 1, 0,

				1, 0, -1,
				0, 0,
				0, 1, 0,

				1, 0, 1,
				0, 0,
				0, 1, 0,

				-1, 0, 1,
				0, 0,
				0, 1, 0
			};
			GLushort ind[] = {
				0,1,2, 2,3,0
			};
			GLuint vbo, vao, indices;

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 8 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			GLsizei posl = sizeof(GLfloat) * 3;
			GLsizei uvl = sizeof(GLfloat) * 2;
			GLsizei norml = sizeof(GLfloat) * 3;
			GLsizei stride = posl + uvl + norml;

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (char*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (char*)posl);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (char*)(posl + uvl));

			glGenBuffers(1, &indices);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6, ind, GL_STATIC_DRAW);

			WaterRenderer *wr = new WaterRenderer(s, vbo, vao, indices);

			wr->scale = vec3(20, 20, 20);
			return wr;
		}
	};
}