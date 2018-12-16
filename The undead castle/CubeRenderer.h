#pragma once
#include "TileRenderer.h"
namespace osomi {
	class CubeRenderer : public TileRenderer{
	public:
		CubeRenderer(Model *_m = nullptr) : TileRenderer(_m) {}
		void render(unsigned int sides) {
			//  v 1.000000 - 1.000000 - 1.000000
			//	v 1.000000 - 1.000000 1.000000
			//	v - 1.000000 - 1.000000 1.000000
			//	v - 1.000000 - 1.000000 - 1.000000
			//	v 1.000000 1.000000 - 0.999999
			//	v 0.999999 1.000000 1.000001
			//	v - 1.000000 1.000000 1.000000
			//	v - 1.000000 1.000000 - 1.000000

			//	f 8 / 4 / 2 7 / 17 / 2 6 / 5 / 2

			//	f 5 / 7 / 3 6 / 18 / 3 2 / 8 / 3
			//	f 6 / 9 / 4 7 / 17 / 4 3 / 10 / 4

			//	f 3 / 12 / 5 7 / 19 / 5 8 / 13 / 5
			//	f 1 / 14 / 6 4 / 20 / 6 8 / 15 / 6

			if ((sides & Model::BOTTOM) != 0) {
				Renderer::render(0, 3);
				Renderer::render(18, 21);
			}
			if ((sides & Model::TOP) != 0) {
				Renderer::render(3, 6);
				Renderer::render(6, 9);
			}
			if ((sides & Model::FRONT) != 0) {
				Renderer::render(9, 12);

			}
			if ((sides & Model::LEFT) != 0) {
				Renderer::render(12, 15);

			}
			if ((sides & Model::BACK) != 0) {
				Renderer::render(15, 18);

			}
		}
	};
}