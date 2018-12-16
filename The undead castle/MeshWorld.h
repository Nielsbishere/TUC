#pragma once
#include <GL/glew.h>
#include <vector>

#include "Resources.h"
#include "Shader.h"
#include "vec3.h"
#include "mat4.h"

#include "SimplexNoise.h"
#include "WaterRenderer.h"

#include <thread>

using namespace std;
namespace osomi {
	
	class WorldMesh {
		friend class MeshWorld;
	private:
		GLuint vbo, vao, indices;
		unsigned int M, N;
		vec3 rotation, translation, scale;
		bool started, ready;

		GLushort *ind;
		GLfloat *vertexData;

		void finish() {
			if (!ready && started) {
				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, M * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

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
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * N, ind, GL_STATIC_DRAW);

				delete[] ind;
				delete[] vertexData;

				ready = true;
			}
		}

		mat4 getModel() {
			return mat4::translate(translation) * mat4::rotateDeg(rotation) * mat4::scale(scale);
		}
		vec3 calcNorm(vec3 v0, vec3 v1, vec3 v2, vec3 v3, vec3 v4) {
			vec3 d1 = v1 - v0;
			vec3 d2 = v2 - v0;
			vec3 d3 = v3 - v0;
			vec3 d4 = v4 - v0;

			vec3 t1 = d1.cross(d2).normalize();
			vec3 t2 = d2.cross(d3).normalize();
			vec3 t3 = d3.cross(d4).normalize();
			vec3 t4 = d4.cross(d1).normalize();

			return ((t1 + t2 + t3 + t4) / 4).normalize();
		}
		void init(float *heightMap, unsigned int tilesX, unsigned int tilesY, vec3 size, bool finalize=true) {
			vertexData = new GLfloat[M];
			ind = new GLushort[N];
			for (unsigned int x = 0; x < tilesX; x++)
				for (unsigned int y = 0; y < tilesY; y++) {
					unsigned int i = y * tilesX + x;

					(*(vec3*)(&vertexData[8 * i])) = vec3(((float)x / tilesX * 2 - 1) * size.x, (heightMap[(y + 1)*tilesX + x + 1] * 2 - 1) * size.y, ((float)y / tilesY * 2 - 1) * size.z);
					(*(vec2*)(&vertexData[8 * i + 3])) = vec2(0, 0);
					(*(vec3*)(&vertexData[8 * i + 5])) = vec3(0, 0, 0);

					if (x < tilesX - 1 && y < tilesX - 1) {
						unsigned int j = y * (tilesX - 1) + x;
						ind[j * 6] = i;
						ind[j * 6 + 1] = i + 1;
						ind[j * 6 + 2] = i + 1 + tilesX;
						ind[j * 6 + 3] = i + 1 + tilesX;
						ind[j * 6 + 4] = i + tilesX;
						ind[j * 6 + 5] = i;
					}
				}

			//Calculate normals (foreach triangle)
			for (unsigned int x = 0; x < tilesX - 1; x++)
				for (unsigned int y = 0; y < tilesY - 1; y++) {
					unsigned int j = y * (tilesX - 1) + x;
					vec3 p0 = *(vec3*)(&vertexData[8 * ind[j * 6]]);
					vec3 p1 = *(vec3*)(&vertexData[8 * ind[j * 6 + 1]]);
					vec3 p2 = *(vec3*)(&vertexData[8 * ind[j * 6 + 2]]);
					vec3 p3 = *(vec3*)(&vertexData[8 * ind[j * 6 + 4]]);

					vec3 t0 = (p0 - p1).cross(p2 - p1).normalize();
					vec3 t1 = (p2 - p3).cross(p0 - p3).normalize();

					t0 *= (p0 - p1).magnitude() * (p2 - p1).magnitude() / 2;
					t1 *= (p2 - p3).magnitude() * (p0 - p3).magnitude() / 2;

					vec3 &n0 = *(vec3*)(&vertexData[8 * ind[j * 6] + 5]);
					vec3 &n1 = *(vec3*)(&vertexData[8 * ind[j * 6 + 1] + 5]);
					vec3 &n2 = *(vec3*)(&vertexData[8 * ind[j * 6 + 2] + 5]);
					vec3 &n3 = *(vec3*)(&vertexData[8 * ind[j * 6 + 4] + 5]);

					n0 += t0;
					n1 += t0;
					n2 += t0;
					n2 += t1;
					n3 += t1;
					n0 += t1;
				}

			//Normalizing normals (foreach vertex)
			for (unsigned int x = 0; x < tilesX; x++)
				for (unsigned int y = 0; y < tilesY; y++) {
					unsigned int i = y * tilesX + x;
					vec3 &n = *(vec3*)(&vertexData[8 * i + 5]);
					n = n.normalize();
				}

			started = true;

			//Send mesh
			if(finalize)finish();
		}
	protected:
		WorldMesh(SimplexNoise *sn, unsigned int tilesX, unsigned int tilesY, vec3 size, vec3 translate = vec3()): ind(nullptr), vertexData(nullptr) {
			//Model matrix
			rotation = vec3();
			translation = translate;
			scale = vec3(1, 1, 1);

			//Setting mesh to null
			M = 8 * tilesX *  tilesY;
			N = 2 * 3 * (tilesX - 1) * (tilesY - 1);
			vao = vbo = indices = 0;

			ready = started = false;
		}
		WorldMesh(SimplexNoise *sn, float *heightMap, unsigned int tilesX, unsigned int tilesY, vec3 size, vec3 translate = vec3()): ind(nullptr), vertexData(nullptr) {
			//Model matrix
			rotation = vec3();
			translation = translate;
			scale = vec3(1, 1, 1);

			//Generate mesh
			M = 8 * tilesX *  tilesY;
			N = 2 * 3 * (tilesX-1) * (tilesY-1);
			init(heightMap, tilesX, tilesY, size);
		}
		~WorldMesh() {
			if (started && !ready) {
				delete[] ind;
				delete[] vertexData;
			}
			if (!ready)return;
			glDeleteBuffers(M * sizeof(GLfloat), &vbo);
			glDeleteBuffers(N * sizeof(GLushort), &indices);
			glDeleteVertexArrays(1, &vao);
		}
		void render(mat4 v, mat4 p, vec3 eye, Shader *s) {
			if (!ready)return;
			glCullFace(GL_FRONT);
			s->setMVP(getModel(), v, p, eye);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
			glDrawElements(GL_TRIANGLES, N, GL_UNSIGNED_SHORT, (char*)0);
			glCullFace(GL_BACK);
		}
	};
	class MeshWorld {
	private:
		vector<WorldMesh*> meshes;
		Texture *tm;
		Shader *s;
		SimplexNoise *sn;
		WaterRenderer *wr;
		bool ready;

		float **hm;
		unsigned int tilesX, tilesY;
		vec3 size;

		MeshWorld(Shader *_s, Texture *_tm, unsigned int _meshes) : s(_s), tm(_tm), sn(new SimplexNoise()), meshes(vector<WorldMesh*>(_meshes)), ready(false), hm(nullptr), tilesX(0), tilesY(0), size(vec3()), wr(nullptr) {}

		void init(const unsigned int start, const unsigned int end, bool finalize=false) {
			for (unsigned int i = start; i < end && i < meshes.size(); i++) 
				meshes[i]->init(hm[i], tilesX, tilesY, size, finalize);
		}
	public:
		~MeshWorld() {
			for (unsigned int i = 0; i < meshes.size(); i++)
				delete meshes[i];
			delete s;
			delete sn;
			delete wr;
		}
		void render(mat4 v, mat4 p, vec3 eye, vec3 ambient, bool enableFog, vec3 fogColor, float fogDensity, vector<Light> &lights) {
			if (!ready)return;
			s->use();
			s->setAmbientLight(ambient);
			s->setFog(enableFog, fogColor, fogDensity);
			s->setLights(lights);
			tm->bind(GL_TEXTURE0); 
			GLuint texLoc = glGetUniformLocation(s->getId(), "t");
			glUniform1i(texLoc, 0);
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i]->render(v, p, eye, s);

			wr->render(v, p, eye, ambient, enableFog, fogColor, fogDensity, lights);
		}
		static MeshWorld *generate(string texturePath, string vertPath, string fragPath) {
			Texture *t = Resources::loadTexture(texturePath);
			if (t == nullptr) {
				printf("Couldn't load the world! Couldn't find the tile map!\n");
				return nullptr;
			}
			Shader *s = Shader::createShader(vertPath, fragPath);
			if (s == nullptr) {
				printf("Couldn't load the world! Couldn't load the shader!\n");
				return nullptr;
			}
			WaterRenderer *wr = WaterRenderer::create("watervert.glsl", "waterfrag.glsl");
			if (wr == nullptr) {
				printf("Couldn't load the world! Couldn't load the water!\n");
				return nullptr;
			}
			//Constants
			const unsigned int tilesX = 256, tilesY = 256;
			const float maxSize = 30;
			const float sizeX = 4, sizeY = 4;
			const float noiseScale = .25 / 4;

			const double xscale = 1.0 / tilesX * noiseScale, yscale = 1.f / tilesY * noiseScale * 2;
			const double startX = rand() / 100.0 + rand(), startY = rand() / 100.0 + rand();

			const vec3 size = vec3(sizeX, maxSize, sizeY);

			const unsigned int chunkX = 8, chunkY = 8;

			unsigned int tclock = clock();

			//World with meshes
			MeshWorld *res = new MeshWorld(s, t, chunkX * chunkY);
			res->wr = wr;

			//Without multithreading:
			//Heightmap =	112 ms
			//Mesh		=	304 ms
			//Total		=	416 ms

			//With multithreading (estimation):
			//Heightmap	=	 18 ms
			//Mesh		=	 50 ms
			//Total		=	 68 ms

			//With multithreading (data):
			//Heightmap =	  x ms
			//Mesh		=	 122 ms
			//Total		=	  y ms

			//Don't multi thread OpenGL send! Those are CPU -> GPU and can't go faster

			//Generate heightmap
			float **heightMap = new float*[chunkX*chunkY];
			for (unsigned int i = 0; i < chunkX*chunkY; i++)
				heightMap[i] = new float[(tilesX + 1)*(tilesY + 1)];

			float max = numeric_limits<float>::min();
			float min = numeric_limits<float>::max();

			for (unsigned int xx = 0; xx < (tilesX + 2) * chunkX; xx++)
				for (unsigned int yy = 0; yy < (tilesY + 2) * chunkY; yy++) {
					unsigned int cx = xx / (tilesX + 2);
					unsigned int cy = yy / (tilesY + 2);
					unsigned int x = xx % (tilesX + 2);
					unsigned int y = yy % (tilesY + 2);

					float f;
					f = heightMap[cy * chunkX + cx][y * tilesX + x] = res->sn->tnoise(xx * xscale + startX, yy * yscale + startY, 1, 6, 0.5, 2);
					if (f > max)max = f;
					if (f < min)min = f;
				}

			for (unsigned int xx = 0; xx < (tilesX + 2) * chunkX; xx++)
				for (unsigned int yy = 0; yy < (tilesY + 2) * chunkY; yy++) {
					unsigned int cx = xx / (tilesX + 2);
					unsigned int cy = yy / (tilesY + 2);
					unsigned int x = xx % (tilesX + 2);
					unsigned int y = yy % (tilesY + 2);

					heightMap[cy * chunkX + cx][y * tilesX + x] = (heightMap[cy * chunkX + cx][y * tilesX + x] + min) / (max - min);
				}

			printf("Finished hm within %ums!\n", clock() - tclock);

			//Storing important data in MeshWorld
			res->hm = heightMap;
			res->tilesX = tilesX;
			res->tilesY = tilesY;
			res->size = size;

			//Creating mesh
			unsigned int currency = thread::hardware_concurrency();
			tclock = clock();

			//Pre initializing the meshes
			for (unsigned int i = 0; i < chunkX; i++)
				for (unsigned int j = 0; j < chunkY; j++) 
					res->meshes[j * chunkX + i] = new WorldMesh(res->sn, tilesX, tilesY, size, vec3(sizeX * 2 * i, 0, sizeY * 2 * j));

			//Init threads
			if (currency != 0) {
				vector<thread*> thrds(currency);
				unsigned int toInit = (chunkX * chunkY) / currency;
				const unsigned int left = (chunkX * chunkY) % currency;

				for (unsigned int i = 0; i < currency; i++) {
					const unsigned int startIndex = toInit * i;

					if (i == currency - 1)
						toInit += left;
					const unsigned int endIndex = startIndex + toInit;

					thrds[i] = new thread(&MeshWorld::init, res, startIndex, endIndex, false);
				}

				//Waiting for threads to succeed and cleaning them up afterwards
				for (unsigned int i = 0; i < currency; i++) {
					thrds[i]->join();
					delete thrds[i];
					thrds[i] = nullptr;
				}
				thrds.clear();

				//Finalize all meshes
				for (unsigned int i = 0; i < chunkX*chunkY; i++)
					res->meshes[i]->finish();
			}
			else{
				res->init(0, chunkX * chunkY, true);
			}
			printf("Finalized (%u threads) within %ums!\n", currency, clock()-tclock);

			//Deleting heightmap
			for (unsigned int i = 0; i < chunkX*chunkY; i++)
				delete[] heightMap[i];
			delete[] heightMap;
			res->hm = nullptr;

			//<!! End
			res->ready = true;
			return res;
		}
	};
}