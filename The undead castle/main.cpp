#include "ExtendedCPP.h"
#include "Window.h"
#include "Model.h"
#include "GameState.h"
#include "Resources.h"

//Including windows without min max and near defines
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#undef near
#endif

using namespace osomi;
vector<Window*> Window::windows = vector<Window*>();
vector<Model*> Model::models = vector<Model*>();
Resources r = Resources();
Resources *Resources::instance = &r;

Window *mainWindow = new Window("The undead castle", "Logo/TUC.ico");

using namespace tuc;

int main() {
	//Removing console for release app
	#ifndef _REMCONSOLE
	#else
		#ifdef _WIN32
			FreeConsole();
		#else
		#endif
	#endif
	mainWindow->updateState(new GameState());
	Window::update();
	printf("Terminated!\n");
	return 0;
}