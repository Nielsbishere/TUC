#pragma once
#include <GL/glew.h>
namespace osomi {
	class Texture {
	private:
		GLuint texture;
		string name;
	public:
		Texture(string n, GLuint t) : texture(t), name(n) {}
		void bind() {
			glBindTexture(GL_TEXTURE_2D, texture);
		}
		void bind(GLenum id) {
			glActiveTexture(id);
			glBindTexture(GL_TEXTURE_2D, texture);
		}
		string getName() {
			return name;
		}
	};
}