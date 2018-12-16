#pragma once
#include <GL/glew.h>
#include <SOIL/SOIL2.h>
#include "StringUtils.h"
#include <vector>
#include "Shader.h"
#include "vec2.h"
#include "vec3.h"
#include "Resources.h"

using namespace std;
namespace osomi {
	class Font {
		friend class TextHandler;
	public:
		class Character {
			friend class Font;
			friend class CharMesh;
		protected:
			char c;
			float u1, u2, v1, v2;
			float offX, offY, advX;
			Character(char _c, unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int twidth, unsigned int theight, int offsetX, int offsetY, int advanceX) : c(_c), u1((float)x / twidth), v1(1 - (float)(y + height) / theight), u2((float)(x + width) / twidth), v2(1 - (float)y / theight), offX((float)offsetX/twidth), offY((float)offsetY/theight), advX((float)advanceX/twidth) {}
		public:
			Character() : c(0), u1(0), u2(0), v1(0), v2(0), offX(0), offY(0), advX(0){}
		};
		class CharMesh {
			friend class Font;
		private:
			Character c;
			GLuint vao, vbo, indices;
		protected:
			CharMesh(Character _c) : c(_c) {
				GLfloat vertexData[] = {
					c.offX, 0,
					c.u1, c.v1,

					c.offX + c.advX, 0,
					c.u2, c.v1,

					c.offX + c.advX, c.advX / 16 * 9,
					c.u2, c.v2,

					c.offX, c.advX / 16 * 9,
					c.u1, c.v2
				};
				GLushort ind[] = {
					0,1,2, 2,3,0
				};
				unsigned int M = 4*4, N = 6;

				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, M * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

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

				glGenBuffers(1, &indices);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * N, ind, GL_STATIC_DRAW);
			}
			void draw() {
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBindVertexArray(vao);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (char*)0);
			}
		public:
			CharMesh() : CharMesh(Character()) {}
			void clean() {
				unsigned int M = 4 * 4, N = 6;
				glDeleteBuffers(M * sizeof(GLfloat), &vbo);
				glDeleteBuffers(N * sizeof(GLushort), &indices);
				glDeleteVertexArrays(1, &vao);
			}
		};
	private:
		Texture *t;
		vector<CharMesh> chars;
		string name;
		Font(string _name): name(_name) {}
		CharMesh *findCharMesh(char c) {
			for (unsigned int i = 0; i < chars.size(); i++) {
				if (chars[i].c.c == c)
					return &chars[i];
			}
			return nullptr;
		}
	protected:
		static Font *loadFont(string atlas) {
			Texture *t = Resources::loadTexture(StringUtils::grabFileNoExt(atlas) + ".png");
			if (t == nullptr) {
				printf("Couldn't load the texture!\n");
				return nullptr;
			}
			t->bind();
			GLint width, height;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

			ifstream str(atlas.c_str(), ios::binary | ios::in);
			if (!str.good()) {
				printf("Couldn't open the font file!\n");
				return nullptr;
			}
			vector<CharMesh> chrs;
			string val;
			unsigned int j = 0;
			while (getline(str, val)) {
				if (StringUtils::startsWith(val, "chars count=")) {
					if (chrs.size() != 0) {
						printf("Font asked to redefine the character count! Undefined behaviour!\n");
						return nullptr;
					}
					unsigned int i = StringUtils::parseInt(StringUtils::substring(val, 12));
					if (i == 0 || i > 256) {
						printf("Osomi's font parser doesn't support NULL fonts or UTF-16 fonts, just UTF-8 fonts!\n");
						return nullptr;
					}
					chrs = vector<CharMesh>(i);
				}
				else if (StringUtils::startsWith(val, "char id=")) {
					if (chrs.size() == 0) {
						printf("Tried to init the character array without a max size!\n");
						return nullptr;
					}
					vector<string> arguments = ArrayHelper::selectStrings(StringUtils::split(StringUtils::substring(val, 5), ' '), '=');
					if (arguments.size() < 8) {
						printf("Font character doesn't have at least 8 arguments!\n");
						return nullptr;
					}
					if (!StringUtils::startsWith(arguments[0], "id=")) {
						printf("Incorrect font character! Doesn't start with id argument!\n");
						return nullptr;
					}
					if (!StringUtils::startsWith(arguments[1], "x=")) {
						printf("Incorrect font character! Argument 2 != x argument!\n");
						return nullptr;
					}
					if (!StringUtils::startsWith(arguments[2], "y=")) {
						printf("Incorrect font character! Argument 3 != y argument!\n");
						return nullptr;
					}
					if (!StringUtils::startsWith(arguments[3], "width=")) {
						printf("Incorrect font character! Argument 4 != width argument!\n");
						return nullptr;
					}
					if (!StringUtils::startsWith(arguments[4], "height=")) {
						printf("Incorrect font character! Argument 5 != height argument!\n");
						return nullptr;
					}
					if (j >= chrs.size()) {
						printf("Reached the limit of the atlas! But continued pushing! Aborting!\n");
						return nullptr;
					}
					unsigned int ci = StringUtils::parseInt(StringUtils::split(arguments[0], '=')[1]);
					if (ci > 0xFF) {
						printf("Couldn't parse the id to UTF-8!\n");
						return nullptr;
					}
					unsigned char uc = ci & 0xFF;
					char c = *((char*)&uc);

					unsigned int x = StringUtils::parseInt(StringUtils::split(arguments[1], '=')[1]);
					unsigned int y = StringUtils::parseInt(StringUtils::split(arguments[2], '=')[1]);
					unsigned int w = StringUtils::parseInt(StringUtils::split(arguments[3], '=')[1]);
					unsigned int h = StringUtils::parseInt(StringUtils::split(arguments[4], '=')[1]);

					int offX = StringUtils::parseSInt(StringUtils::split(arguments[5], '=')[1]);
					int offY = StringUtils::parseSInt(StringUtils::split(arguments[6], '=')[1]);
					int advX = StringUtils::parseSInt(StringUtils::split(arguments[7], '=')[1]);

					Character character = Character(c, x, y, w, h, width, height, offX, offY, advX);
					/*if (c == '[') {
						printf("Found [ with x y w h: %u %u %u %u (%f %f %f %f); %i %i %i\n", x, y, w, h, character.u1, character.u2, character.v1, character.v2, offX, offY, advX);
					}*/
					chrs[j] = CharMesh(character);
					j++;
				}
			}
			Font *f = new Font(StringUtils::grabFileNoExt(atlas));
			f->t = t;
			f->chars = chrs;
			printf("Loaded font %s\n", f->name.c_str());
			return f;
		}
		~Font() {
			for (unsigned int i = 0; i < chars.size(); i++)
				chars[i].clean();
		}
	public:
		void draw(vec2 offset, vec2 size, string s, Shader *shader) {
			t->bind();
			float x = 0;
			for (unsigned int i = 0; i < s.size(); i++) {
				CharMesh *cm = findCharMesh(s[i]);
				if (cm == nullptr)cm = findCharMesh(' ');
				if (cm == nullptr)continue;

				vec2 pos = vec2(offset.x + x * size.x, offset.y - cm->c.offY / 16 * 9);

				GLuint upos = glGetUniformLocation(shader->getId(), "pos");
				glUniform2fv(upos, 1, &pos.x);
				GLuint uscale = glGetUniformLocation(shader->getId(), "scale");
				glUniform2fv(uscale, 1, &size.x);

				cm->draw();
				x += cm->c.advX;
			}
		}
	};
	class Text {
		friend class TextHandler;
	private:
		Font *currFont;
		vec2 size, offset;
		vec3 color, borderColor;
		string contents;

		Text(Font *f, vec2 _size, vec2 _offset, vec3 col, vec3 bcol, string _contents) : currFont(f), size(_size), offset(_offset), color(col), borderColor(bcol), contents(_contents) {}
	protected:
		void renderText(Shader *s) {
			GLuint col = glGetUniformLocation(s->getId(), "col");
			glUniform3fv(col, 1, &color.x);
			GLuint bcol = glGetUniformLocation(s->getId(), "border");
			glUniform3fv(bcol, 1, &borderColor.x);

			currFont->draw(offset, size, contents, s);
		}
	public:
		void setContents(string s) {
			contents = s;
		}
		void setColor(vec3 col) {
			color = col;
		}
		void setBorderColor(vec3 col) {
			borderColor = col;
		}
		void setSize(vec2 _size) {
			size = _size;
		}
		void setPosition(vec2 _pos) {
			offset = _pos;
		}
		string getContents() { return contents; }
		vec3 getColor() { return color; }
		vec3 getBorderColor() { return borderColor; }
		vec2 getSize() { return size; }
		vec2 getPosition() { return offset; }
	};
	class TextHandler {
	private:
		vector<Font*> fonts;
		vector<Text> texts;
		Font *def;
		vec2 defSize;
		vec3 defColor;
		Shader *shader;
		TextHandler(Shader *s): shader(s), defColor(vec3(0,0,0)), defSize(vec2(1,1)), def(nullptr) {}
	public:
		~TextHandler() {
			for (unsigned int i = 0; i < fonts.size(); i++)
				delete fonts[i];
		}
		static TextHandler *load(string mainAtlas) {
			Shader *shader = Shader::createShader("TextVert.glsl", "TextFrag.glsl");
			if (shader == nullptr) {
				printf("Couldn't load the text handler! Shaders invalid!\n");
				return nullptr;
			}
			Font *f = Font::loadFont(mainAtlas);
			if (f == nullptr) {
				printf("Couldn't load the font!\n");
				return nullptr;
			}
			TextHandler *handle = new TextHandler(shader);
			handle->def = f;
			handle->fonts.push_back(f);
			return handle;
		}

		void render() {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			shader->use();
			for (unsigned int i = 0; i < texts.size(); i++)
				texts[i].renderText(shader);
			glDisable(GL_BLEND);
		}

		unsigned int addText(string contents, vec2 offset=vec2(), vec2 size=vec2(1,1), vec3 col=vec3(), vec3 bcol=vec3(), unsigned int fid=1) {
			if (fid > fonts.size() || fid == 0) {
				printf("Couldn't add text! Invalid font!\n");
				return 0;
			}
			Font *f = fonts[fid - 1];
			texts.push_back(Text(f, size, offset, col, bcol, contents));
			return texts.size();
		}

		unsigned int addFont(string font) {
			Font *f = Font::loadFont(font);
			if (f == nullptr) {
				printf("Couldn't load the font!\n");
				return 0;
			}
			fonts.push_back(f);
			return fonts.size();
		}
		Text *getText(unsigned int i) {
			if (i > texts.size() || i == 0)return nullptr;
			return &texts[i - 1];
		}
		

		//Remove font (Set text to default; if default = nullptr, remove text
		//Remove text
	};
}