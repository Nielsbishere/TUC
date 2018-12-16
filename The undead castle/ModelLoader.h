#pragma once
#include "StringUtils.h"
#include "Model.h"
#include "MaterialLoader.h"
#include <fstream>
#include <vector>

using namespace std;

namespace osomi {
	class ModelLoader {
	private:
		struct ObjectInfo {
			string name;
			unsigned int vertices, normals, uvs, triangles;
			vector<ModelObjectPart> objs;
			ObjectInfo(string n) : name(n), vertices(0), normals(0), uvs(0), triangles(0) {}
		};
		inline Material findMaterial(vector<Material> &materials, string s) {
			for (unsigned int i = 0; i < materials.size(); i++)
				if (materials[i].getName() == s)return materials[i];
			return Material();
		}
	public:
		enum EScalingType {
			NONE, XZ, XY, XYZ, XZ_CENTER, XY_CENTER, XYZ_CENTER
		};
		~ModelLoader() {
			Model::destroyModels();
		}
		static Model *exists(string file) {
			Model *cache;
			if ((cache = Model::get(file)) != nullptr) return cache;
			return nullptr;
		}
		Model *loadObj(string file, EScalingType scaling, int log=1) {
			#ifdef _TEST
			log = 2;
			#endif
			if (!StringUtils::endsWith(file, ".obj")) {
				printf("ModelLoader: Couldn't load \"%s\"! It isn't an .obj file!\n", file.c_str());
				return nullptr;
			}
			Model *cache;
			if ((cache = Model::get(file)) != nullptr) return cache;
			ifstream input(file, ios::in);
			if (!input.good()) {
				printf("ModelLoader: Obj file can't be opened or doesn't exist!\n");
				return nullptr;
			}
			vector<ObjectInfo> info;
			string line;
			unsigned int lines=0;

			unsigned int vertices=0, uvs=0, normals=0, triangles=0;

			vector<Material> materials;

			GLfloat minX = numeric_limits<GLfloat>::max(), minY = minX, minZ = minX;
			GLfloat maxX = numeric_limits<GLfloat>::min(), maxY = maxX, maxZ = maxX;

			unsigned int prevTri = 0, currTri=0;
			Material currm;
			ObjectInfo *current=nullptr;
			while (getline(input, line)) {
				lines++;
				line = StringUtils::trim(line);
				if (line.size() <= 1 || line[0] == '#')continue;
				if (StringUtils::startsWith(line, "mtllib ")) {
					if (!MaterialLoader::load(StringUtils::grabPath(file) + line.substr(7), &materials)) {
						printf("ModelLoader: Couldn't load the materials!\n");
						return nullptr;
					}
				}
				else if ((line[0] == 'o' || line[0] == 'g') && line[1] == ' ') {
					ObjectInfo *prev = info.size() == 0 ? nullptr : &info[info.size() - 1];
					if (prev != nullptr) 
						prev->objs.push_back(ModelObjectPart(prevTri * 3, currTri * 3, currm));
					info.push_back(ObjectInfo(line.substr(2)));
					current = &info[info.size() - 1];
					currTri = 0;
					prevTri = 0;
				}else if (line[0] == 'v' && line[1] == ' ') {
					if (current == nullptr) {
						printf("ModelLoader: Couldn't count the vertices of an unassigned object!\n");
						return nullptr;
					}
					if (count(line.begin(), line.end(), ' ') != 3) {
						printf("ModelLoader: Couldn't load the obj file! There are more or less than 3 coordinates for a vertex! \"%s\" (%u)\n", line.c_str(), lines);
						return nullptr;
					}
					vector<string> split = StringUtils::split(line, ' ');
					GLfloat x = (GLfloat)atof(split[1].c_str()), y = (GLfloat)atof(split[2].c_str()), z = (GLfloat)atof(split[3].c_str());
					if (x < minX)minX = x;
					if (x > maxX)maxX = x;
					if (y < minY)minY = y;
					if (y > maxY)maxY = y;
					if (z < minZ)minZ = z;
					if (z > maxZ)maxZ = z;
					current->vertices++;
					vertices++;
				}else if (line.size() >= 3 && line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
					if (current == nullptr) {
						printf("ModelLoader: Couldn't count the vertex uvs of an unassigned object!\n");
						return nullptr;
					}
					if (count(line.begin(), line.end(), ' ') < 2) {	//TODO: Probably != 2; but sometimes people use 3 uv coordinates? Idek
						printf("ModelLoader: Couldn't load the obj file! There are less than 2 coordinates for a uv coordinate \"%s\" (%u)\n", line.c_str(), lines);
						return nullptr;
					}
					current->uvs++;
					uvs++;
				}else if (line.size() >= 3 && line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
					if (current == nullptr) {
						printf("ModelLoader: Couldn't count the vertex normals of an unassigned object!\n");
						return nullptr;
					}
					if (count(line.begin(), line.end(), ' ') != 3) {
						printf("ModelLoader: Couldn't load the obj file! There are more or less than 3 coordinates for a vertex normal \"%s\" (%u)\n", line.c_str(), lines);
						return nullptr;
					}
					current->normals++;
					normals++;
				}else if (StringUtils::startsWith(line, "usemtl ")) {	//Bind it until it is mentioned again?
					Material m = findMaterial(materials, line.substr(7));
					if (m.null()) {
						printf("ModelLoader: Couldn't find the material mentioned at line %u!\n", lines);
						return nullptr;
					}
					if (!currm.null() && currTri != 0) {
						current->objs.push_back(ModelObjectPart(prevTri * 3, currTri * 3, currm));
					}
					currm = m;
					prevTri = currTri;
				}else if (line[0] == 's' && line[1] == ' ') {
					//What the actual hell is an 's off' or 's on' I don't know...
					//Smooth shading? What?
				}else if (line[0] == 'f' && line[1] == ' ') {
					if (current == nullptr) {
						printf("ModelLoader: Couldn't count the model faces of an unassigned object!\n");
						return nullptr;
					}
					if (count(line.begin(), line.end(), ' ') != 3) {
						printf("ModelLoader: Couldn't load the obj file! The parser only works with triangulated faces!\n");
						return nullptr;
					}
					if (count(line.begin(), line.end(), '/') != 6) {
						printf("ModelLoader: Couldn't load the obj file! Incorrect usage of the 'f' attribute!\n");
						return nullptr;
					}
					current->triangles++;
					triangles++;
					currTri++;
				}else {
					printf("ModelLoader: Couldn't load the obj file! There was an unrecognized symbol at line %u (%s)\n", lines, line.c_str());
					return nullptr;
				}
			}
			if (current != nullptr)
				current->objs.push_back(ModelObjectPart(prevTri * 3, currTri * 3, currm));
			if (log>0) {
				printf("ModelLoader: Loaded %u objects from (%s); resulting in %u vertices, %u uv coordinates, %u normals and %u triangles. (%u lines)\n", info.size(), file.c_str(), vertices, uvs, normals, triangles, lines);
				printf("ModelLoader: With bounds %f %f %f to %f %f %f\n", minX, minY, minZ, maxX, maxY, maxZ);
				if (log > 1) {
					for (unsigned int i = 0; i < info.size(); i++)
						printf("ModelLoader: Object #%u: %u vertices, %u uv coordinates, %u normals and %u triangels.\n", i, info[i].vertices, info[i].uvs, info[i].normals, info[i].triangles);
				}
			}

			if (scaling == XZ && (minX != minZ || maxX != maxZ || abs(minX) != abs(maxX))) 
				printf("ModelLoader warning: Model requested 'XZ' scaling, but the x bounds aren't equal to z bounds; resulting in a non-connecting tile!\n");
			if (scaling == XY && (minX != minY || maxX != maxY || abs(minX) != abs(maxX)))
				printf("ModelLoader warning: Model requested 'XY' scaling, but the x bounds aren't equal to y bounds; resulting in a non-connecting tile!\n");

			vertices = uvs = normals = triangles = 0;
			current = nullptr;
			unsigned int i = 0;
			lines = 0;

			vector<ModelObject> objects;
			GLfloat *vertexData = nullptr;
			GLushort *indexData = nullptr;

			GLfloat *vd = nullptr, *nd = nullptr, *ud = nullptr;
			
			unsigned int voff=1, uoff=1, noff=1;

			float mmiX, mmiY, mmiZ, mmaX, mmaY, mmaZ;
			mmiX = mmiY = mmiZ = numeric_limits<float>::max();
			mmaX = mmaY = mmaZ = numeric_limits<float>::min();

			input.close();
			input = ifstream(file, ios::in);
			if (!input.good()) {
				printf("ModelLoader: Obj file can't be re-opened or doesn't exist!\n");
				return nullptr;
			}
			while (getline(input, line)) {
				lines++;
				line = StringUtils::trim(line);
				if (line.size() <= 1 || line[0] == '#')continue;
				if (line[0] == 'o') {
					if (current != nullptr) {
						ModelObject mo = ModelObject::glTriangles(vertexData, current->triangles * 3 * 8, indexData, current->triangles * 3, current->name, mmiX, mmiY, mmiZ, mmaX, mmaY, mmaZ);
						for (unsigned int i = 0; i < current->objs.size(); i++) 
							mo.addPart(current->objs[i]);
						objects.push_back(mo);
						voff += current->vertices;
						uoff += current->uvs;
						noff += current->normals;
						delete[] vd;
						delete[] nd;
						delete[] ud;
						delete[] vertexData;
						delete[] indexData;
						vertices = uvs = normals = triangles = 0;
						mmiX = mmiY = mmiZ = numeric_limits<float>::max();
						mmaX = mmaY = mmaZ = numeric_limits<float>::min();
					}
					current = &info[i];
					vertexData = new GLfloat[current->triangles * 3 * 8];
					memset(vertexData, 0, (current->triangles * 3 * 8) * sizeof(GLfloat));
					indexData = new GLushort[current->triangles * 3];
					memset(vertexData, 0, (current->triangles * 3) * sizeof(GLushort));
					vd = new GLfloat[current->vertices*3];
					memset(vd, 0, (current->vertices * 3) * sizeof(GLfloat));
					nd = new GLfloat[current->normals * 3];
					memset(nd, 0, (current->normals * 3) * sizeof(GLfloat));
					ud = new GLfloat[current->uvs * 2];
					memset(ud, 0, (current->uvs * 2) * sizeof(GLfloat));
					i++;
				}
				else if (line[0] == 'v' && line[1] == ' ') {
					vector<string> split = StringUtils::split(line, ' ');
					GLfloat x = (GLfloat)atof(split[1].c_str()), y = (GLfloat)atof(split[2].c_str()), z = (GLfloat)atof(split[3].c_str());
					if (scaling == XZ) {
						GLfloat big = (maxX - minX) >= (maxZ - minZ) ? (maxX - minX) : (maxZ - minZ);
						x = (x - minX) / big * 2 - 1;
						y = (y - minY) / big * 2 - 1;
						z = (z - minZ) / big * 2 - 1;
					}else if (scaling == XY) {
						GLfloat big = (maxX - minX) >= (maxY - minY) ? (maxX - minX) : (maxY - minY);
						x = (x - minX) / big * 2 - 1;
						y = (y - minY) / big * 2 - 1;
						z = (z - minZ) / big * 2 - 1;
					}else if (scaling == XZ_CENTER) {
						GLfloat big = (maxX - minX) >= (maxZ - minZ) ? (maxX - minX) : (maxZ - minZ);
						float bigX = (maxX - minX) / big;
						float bigY = (maxY - minY) / big;
						float bigZ = (maxZ - minZ) / big;
						x = (x - minX) / big * 2 - bigX;
						y = (y - minY) / big * 2 - bigY;
						z = (z - minZ) / big * 2 - bigZ;
					}
					vd[vertices * 3] = x;
					vd[vertices * 3 + 1] = y;
					vd[vertices * 3 + 2] = z;
					if (x < mmiX)mmiX = x;
					if (y < mmiY)mmiY = y;
					if (z < mmiZ)mmiZ = z;
					if (x > mmaX)mmaX = x;
					if (y > mmaY)mmaY = y;
					if (z > mmaZ)mmaZ = z;
					vertices++;
				}
				else if (line[0] == 'v' && line[1] == 't') {
					vector<string> split = StringUtils::split(line, ' ');
					GLfloat x = (GLfloat)atof(split[1].c_str()), y = (GLfloat)atof(split[2].c_str());
					ud[uvs * 2] = x;
					ud[uvs * 2 + 1] = y;
					uvs++;
				}
				else if (line[0] == 'v' && line[1] == 'n') {
					vector<string> split = StringUtils::split(line, ' ');
					GLfloat x = (GLfloat)atof(split[1].c_str()), y = (GLfloat)atof(split[2].c_str()), z = (GLfloat)atof(split[3].c_str());
					nd[normals * 3] = x;
					nd[normals * 3 + 1] = y;
					nd[normals * 3 + 2] = z;
					normals++;
				}
				else if (line[0] == 'f') {
					vector<string> split = StringUtils::split(line, ' ');
					for (unsigned int ii = 1; ii < split.size(); ii++) {
						vector<string> splitt = StringUtils::split(split[ii], '/');
						unsigned int ll = ii - 1;
						unsigned int v = atoi(splitt[0].c_str()) - voff, u = atoi(splitt[1].c_str()) - uoff, n = atoi(splitt[2].c_str()) - noff;
						if (v >= current->vertices || u >= current->uvs || n >= current->normals) {
							delete[] vd;
							delete[] nd;
							delete[] ud;
							delete[] vertexData;
							delete[] indexData;
							for (unsigned int kk = 0; kk < objects.size(); kk++)
								objects[kk].destroy();
							printf("ModelLoader: Couldn't index out of range at line %u (%s)\n", lines, line.c_str());
							return nullptr;
						}
						vertexData[triangles * 3 * 8 + ll * 8] = vd[v * 3];
						vertexData[triangles * 3 * 8 + ll * 8 + 1] = vd[v * 3 + 1];
						vertexData[triangles * 3 * 8 + ll * 8 + 2] = vd[v * 3 + 2];
						vertexData[triangles * 3 * 8 + ll *8 + 3] = ud[u * 2];
						vertexData[triangles * 3 * 8 + ll * 8 + 4] = ud[u * 2 + 1];
						vertexData[triangles * 3 * 8 + ll * 8 + 5] = nd[n * 3];
						vertexData[triangles * 3 * 8 + ll * 8 + 6] = nd[n * 3 + 1];
						vertexData[triangles * 3 * 8 + ll * 8 + 7] = nd[n * 3 + 2];
					}
					indexData[triangles * 3] = triangles * 3;
					indexData[triangles * 3 + 1] = triangles * 3 + 1;
					indexData[triangles * 3 + 2] = triangles * 3 + 2;
					triangles++;
				}
			}
			input.close();
			if (current != nullptr) {
				ModelObject mo = ModelObject::glTriangles(vertexData, current->triangles * 3 * 8, indexData, current->triangles * 3, current->name, mmiX, mmiY, mmiZ, mmaX, mmaY, mmaZ);
				for (unsigned int i = 0; i < current->objs.size(); i++)
					mo.addPart(current->objs[i]);
				objects.push_back(mo);
				delete[] vd;
				delete[] nd;
				delete[] ud;
				delete[] vertexData;
				delete[] indexData;
			}
			else {
				printf("ModelLoader: The last object couldn't be pushed!\n");
				return nullptr;
			}
			if (scaling == EScalingType::XZ)maxX = maxZ = abs(minX = minZ = -1);
			if (scaling == EScalingType::XY)maxX = maxY = abs(minX = minY = -1);
			Model *m = Model::load(objects, file, minX, minY, minZ, maxX, maxY, maxZ);
			if (m == nullptr) 
				printf("ModelLoader: Failed to load a model!\n");
			else if (log > 1)printf("ModelLoader: Successfully loaded a model!\n");
			return m;
		}
	};
}