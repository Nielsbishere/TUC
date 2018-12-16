#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include "mat4.h"
#include "vec2.h"
#include "StringUtils.h"
#include "Light.h"

namespace osomi {
	class GUIButton;
	class Shader {
		friend class GUIButton;
		friend class Font;
		friend class Text;
		friend class MeshWorld;
	private:
		GLuint shader;
		Shader(GLuint _shader) {
			shader = _shader;
		}
		static void printErr(GLuint shader) {
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

			printf("Error: ");
			for (unsigned int i = 0; i < errorLog.size(); i++)printf("%c", errorLog[i]);
			printf("\n");
		}
	public:
		~Shader() {
			glDeleteProgram(shader);
		}
		static Shader *createShader(string vertPath, string fragPath) {
			if (vertPath == "" || fragPath == "" || !StringUtils::endsWith(vertPath, ".glsl") || !StringUtils::endsWith(fragPath, ".glsl")) {
				printf("The file wasn't a .glsl file!\n");
				return nullptr;
			}
			string vert = StringUtils::fromFile(vertPath), frag = StringUtils::fromFile(fragPath);
			if (vert == "" || frag == "") {
				printf("Couldn't read the file!\n");
				return nullptr;
			}
			const char *vertexShader = vert.c_str(), *fragmentShader = frag.c_str();

			GLuint vs = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vs, 1, &vertexShader, NULL);
			glCompileShader(vs);
			GLint vsCompile;
			glGetShaderiv(vs, GL_COMPILE_STATUS, &vsCompile);

			GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fs, 1, &fragmentShader, NULL);
			glCompileShader(fs);
			GLint fsCompile;
			glGetShaderiv(fs, GL_COMPILE_STATUS, &fsCompile);

			if (vsCompile != GL_TRUE || fsCompile != GL_TRUE) {
				printf("Couldn't compile the %s shader!\n", vsCompile != GL_TRUE && fsCompile != GL_TRUE ? "vertex and fragment" : (vsCompile != GL_TRUE ? "vertex" : "fragment"));
				if (vsCompile != GL_TRUE) {
					printf("Vert info:\n%s\n", vertexShader);
					printErr(vs);
				}
				if (fsCompile != GL_TRUE) {
					printf("Frag info:\n%s\n", fragmentShader);
					printErr(fs);
				}
				glDeleteShader(vs);
				glDeleteShader(fs);
				return nullptr;
			}
			GLuint shader = glCreateProgram();
			glAttachShader(shader, fs);
			glAttachShader(shader, vs);
			glLinkProgram(shader);
			glDeleteShader(vs);
			glDeleteShader(fs);
			glUseProgram(shader);
			printf("Successfully added a shader!\n");

			return new Shader(shader);
		}
		void use() {
			glUseProgram(shader);
		}
		void setMVP(mat4 m, mat4 v, mat4 p, vec3 eye) {
			mat4 matrix = p * v * m;
			GLuint mvp = glGetUniformLocation(shader, "MVP");
			glUniformMatrix4fv(mvp, 1, GL_FALSE, &matrix[0][0]);
			GLuint model = glGetUniformLocation(shader, "model");
			glUniformMatrix4fv(model, 1, GL_FALSE, &m[0][0]);
			GLuint view = glGetUniformLocation(shader, "view");
			glUniformMatrix4fv(view, 1, GL_FALSE, &v[0][0]);
			GLuint projection = glGetUniformLocation(shader, "projection");
			glUniformMatrix4fv(projection, 1, GL_FALSE, &p[0][0]);
			GLuint y = glGetUniformLocation(shader, "eye");
			glUniform3fv(y, 1, &eye[0]);
		}
		void setAmbientLight(vec3 v) {
			GLuint ambient = glGetUniformLocation(shader, "ambient");
			glUniform3fv(ambient, 1, &v[0]);
		}
		void setLights(vector<Light> &lights) {
			unsigned int MAX = 16;
			GLuint pos = glGetUniformLocation(shader, "actualLights");
			glUniform1i(pos, lights.size() < MAX ? lights.size() : MAX);
			for (unsigned int i = 0; i < MAX && i < lights.size(); i++) {
				Light &l = lights[i];
				string position = StringUtils::numString(i, "lights[", "].position");
				string color = StringUtils::numString(i, "lights[", "].color");
				string power = StringUtils::numString(i, "lights[", "].power");
				GLuint pos = glGetUniformLocation(shader, position.c_str());
				glUniform3fv(pos, 1, l.getPos());
				GLuint col = glGetUniformLocation(shader, color.c_str());
				glUniform3fv(col, 1, l.getCol());
				GLuint pow = glGetUniformLocation(shader, power.c_str());
				glUniform1f(pow, l.getPower());
			}
		}
		void setFog(bool enable, vec3 col, float density) {
			GLuint pos = glGetUniformLocation(shader, "isFogEnabled");
			glUniform1i(pos, enable);
			if (enable) {
				pos = glGetUniformLocation(shader, "fogColor");
				glUniform3fv(pos, 1, &col[0]);
				pos = glGetUniformLocation(shader, "fogDensity");
				glUniform1f(pos, density);
			}
		}
	protected:
		GLuint getId() { return shader; }
	};
};