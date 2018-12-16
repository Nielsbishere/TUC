#pragma once
#include "Texture.h"
#include <SOIL/SOIL2.h>

#include <string>
using namespace std;
namespace osomi {
	class Resources {
	private:
		static Resources *instance;
		vector<Texture*> textures;
	public:
		~Resources() {
			for (unsigned int i = 0; i < textures.size(); i++)delete textures[i];
		}
		static Texture *getTexture(string str) {
			for (unsigned int i = 0; i < instance->textures.size(); i++)
				if (instance->textures[i]->getName() == str)return instance->textures[i];
			return nullptr;
		}
		static bool hasTexture(string str) {
			return instance->getTexture(str) != nullptr;
		}
		static Texture *loadTexture(string str) {
			if (hasTexture(str)) return getTexture(str);
			GLuint texture = SOIL_load_OGL_texture(str.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_TEXTURE_REPEATS);
			if (texture == 0) {
				printf("Couldn't load the texture (%s)!\n", str.c_str());
				return nullptr;
			}

			glBindTexture(GL_TEXTURE_2D, texture);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			float ani = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ani);

			if (ani > 1) {
				ani = Math::max(4, ani);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ani);
			}

			instance->textures.push_back(new Texture(str, texture));
			return instance->textures[instance->textures.size() - 1];
		}
	};
}