#pragma once
#include "StringUtils.h"
#include "Resources.h"
#include "Material.h"
#include <string>
#include <vector>
using namespace std;
namespace osomi {
	class MaterialLoader {
	public:
		static bool load(string file, vector<Material> *materials) {
			if (!StringUtils::endsWith(file, ".mtl")) {
				printf("MaterialLoader: Couldn't load \"%s\"! It isn't a .mtl file!\n", file.c_str());
				return false;
			}
			ifstream input(file, ios::in);
			if (!input.good()) {
				printf("MaterialLoader: Mtl file can't be opened or doesn't exist!\n");
				return false;
			}
			string line;
			unsigned int lines = 0;
			Material *current = nullptr;
			while (getline(input, line)) {
				lines++;
				line = StringUtils::trim(line);
				if (line.size() <= 1 || line[0] == '#')continue;
				if (StringUtils::startsWith(line, "newmtl ")) {
					materials->push_back(Material(line.substr(7), nullptr));
					current = &(*materials)[materials->size() - 1];
				}
				else if (StringUtils::startsWith(line, "map_Kd ")) {
					if (current == nullptr) {
						printf("MaterialLoader: Can't bind a texture to an unassigned object!\n");
						materials->clear();
						return false;
					}
					current->setTexture(Resources::loadTexture(StringUtils::grabPath(file) + line.substr(7)));
				}
				else if (StringUtils::startsWith(line, "map_Ka ")) {}
				else if (StringUtils::startsWith(line, "map_Ks ")) {}
				else if (StringUtils::startsWith(line, "map_Ke ")) {}
				else if (StringUtils::startsWith(line, "map_Ns ")) {}
				else if (StringUtils::startsWith(line, "map_d ")) {}
				else if (StringUtils::startsWith(line, "Ka ")) {}
				else if (StringUtils::startsWith(line, "Kd ")) {}
				else if (StringUtils::startsWith(line, "Ke ")) {}
				else if (StringUtils::startsWith(line, "Ks ")) {}
				else if (StringUtils::startsWith(line, "Ns ")) {}
				else if (StringUtils::startsWith(line, "Ni ")) {}
				else if (StringUtils::startsWith(line, "Tf ")) {}
				else if (StringUtils::startsWith(line, "d ") || StringUtils::startsWith(line, "Tr ")) {}
				else if (StringUtils::startsWith(line, "map_bump ") || StringUtils::startsWith(line, "bump ")) {}
				else if (StringUtils::startsWith(line, "illum ")) {}
				else if (StringUtils::startsWith(line, "disp ")) {}
				else if (StringUtils::startsWith(line, "decal ")) {}
				else {
					printf("MaterialLoader: The intergraded loader doesn't support the following expresion: %s!\n", line.c_str());
					materials->clear();
					return false;
				}
			}
			//Do things and stuff
			//newmtl (create material)
			///Ka (ambient color)
			///	map_Ka (ambient texture)
			///Kd (diffuse color)
			//	map_Kd (diffuse texture)	(OFTEN USED AS ONLY TEXTURE!)
			///Ks (specular color)
			///  Ns (weight)
			///	map_Ks (specular texture)
			///	map_Ns (specular high light texture)
			///d (dissolved)
			///  Tr (transparency (1 - d))
			///	map_d (alpha texture)
			///illum (illumination type:
			/*	0. Color on and Ambient off
				1. Color on and Ambient on
				2. Highlight on
				3. Reflection on and Ray trace on
				4. Transparency: Glass on, Reflection : Ray trace on
				5. Reflection : Fresnel on and Ray trace on
				6. Transparency : Refraction on, Reflection : Fresnel off and Ray trace on
				7. Transparency : Refraction on, Reflection : Fresnel on and Ray trace on
				8. Reflection on and Ray trace off
				9. Transparency : Glass on, Reflection : Ray trace off
				10. Casts shadows onto invisible surfaces*/
			///map_bump/bump (bump map)
			///disp (displacement map)
			///decal (stencil decal texture)
			

			//There can also be options for map_Kx
			return true;
		}
	};
}