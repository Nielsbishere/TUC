#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "StateManager.h"

using namespace std;

namespace osomi {
	class Window {
	private:
		static vector<Window*> windows;
		static void exit(GLFWwindow *w) {
			for (unsigned int i = 0; i < windows.size(); i++)
				if (windows[i]->w == w) {
					windows[i]->clean();
					windows.erase(windows.begin() + i);
					break;
				}
		}
		static void resize(GLFWwindow *w, int wi, int he) {
			for (unsigned int i = 0; i < windows.size(); i++)
				if (windows[i]->w == w) {
					windows[i]->resize(wi, he);
					break;
				}
		}
		static void key(GLFWwindow *w, int k, int scancode, int a, int mods) {
			for (unsigned int i = 0; i < windows.size(); i++)
				if (windows[i]->w == w) {
					windows[i]->keypress(k, a);
					break;
				}
		}
		static void mousepress(GLFWwindow *w, int button, int a, int mods) {
			for (unsigned int i = 0; i < windows.size(); i++)
				if (windows[i]->w == w) {
					windows[i]->mousepress(button, a);
					break;
				}
		}
		static void cursor(GLFWwindow *w, double x, double y) {
			for (unsigned int i = 0; i < windows.size(); i++)
				if (windows[i]->w == w) {
					windows[i]->setPosition(x, y);
					break;
				}
		}
		static void err(int error, const char *description) {
			printf("GLFW error %i: %s\n", error, description);
		}
	public:
		Window(const char *title, const char *logo) {
			glfwSetErrorCallback(err);

			if (!glfwInit()) {
				printf("Couldn't init glfw!\n");
				w = nullptr;
				return;
			}
			GLFWmonitor *mon = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(mon);

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

			GLFWwindow *w = glfwCreateWindow(mode->width, mode->height - 30, title, NULL, NULL);
			if (w == nullptr) {
				printf("Couldn't start the main window!\n");
				w = nullptr;
				return;
			}
			glfwSetWindowPos(w, 0, 30);
			glfwMakeContextCurrent(w);
			glfwSetWindowCloseCallback(w, exit);
			glfwSetWindowSizeCallback(w, resize);
			glfwSetMouseButtonCallback(w, mousepress);
			glfwSetCursorPosCallback(w, cursor);
			glfwSetKeyCallback(w, key);

			int wi, he, ch;
			unsigned char *da = SOIL_load_image(logo, &wi, &he, &ch, SOIL_LOAD_L);

			GLFWimage img;
			img.width = wi;
			img.height = he;
			img.pixels = da;
			glfwSetWindowIcon(w, 1, &img);

			delete[] da;

			GLFWcursor *c = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
			glfwSetCursor(w, c);

			if (glewInit()) {
				printf("Couldn't init glew!\n");
				w = nullptr;
				return;
			}
			printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
			glfwSwapInterval(1);

			glEnable(GL_DEPTH_TEST);

			glEnable(GL_CULL_FACE);

			glDepthFunc(GL_LESS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glfwWindowHint(GLFW_SAMPLES, 4);
			glEnable(GL_MULTISAMPLE);

			glEnable(GL_TEXTURE_2D);

			this->w = w;
			windows.push_back(this);

			sm = new StateManager();
		}
		~Window() {
			for (unsigned int i = 0; i < windows.size(); i++)
				if (windows[i] == this) {
					destroy();
					windows.erase(windows.begin() + i);
					break;
				}
			delete sm;
		}
		static void update() {
			while (windows.size() != 0) {
				for (int i = windows.size() - 1; i >= 0; i--) {
					Window *win = windows[i];
					if (win == nullptr || win->w == nullptr) {
						printf("Removing due to nullptr\n");
						windows.erase(windows.begin() + i);
						continue;
					}
					if (glfwWindowShouldClose(win->w)) {
						printf("Removing due to window closing\n");
						windows.erase(windows.begin() + i);
						continue;
					}
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					win->run();

					float x = 0, y = 0;
					win->sm->getMouse(x, y);
					int w, h;
					glfwGetWindowSize(win->w, &w, &h);
					if(!(x > w || y > h || x < 0 || y < 0))glfwSetCursorPos(win->w, x, y);

					glfwSwapBuffers(win->w);
					glfwPollEvents();
				}
			}
		}
		void updateState(State *st) {
			int w, h;
			glfwGetWindowSize(this->w, &w, &h);
			st->resize(w, h);
			if(sm!=nullptr)sm->setState(st);
		}
		void getSize(int *wi, int *he) {
			if (w == nullptr)return;
			glfwGetWindowSize(w, wi, he);
		}
		GLFWwindow *val() {
			return w;
		}
	protected:
		GLFWwindow *w;
		double time;
		StateManager *sm;

		void destroy() {
			glfwDestroyWindow(w);
			w = nullptr;
			clean();
		}
		void clean() {
			glfwTerminate();
			if (sm != nullptr)delete sm;
			sm = nullptr;
		}
		void update(double delta) {
			if (sm != nullptr)sm->update(delta);
		}
		void render(int w, int h) {
			if (sm != nullptr)sm->render(w, h);
		}
		void keypress(int key, int state) {
			if (sm != nullptr)sm->keypress(key, state);
		}
		void resize(int w, int h) {
			if (sm != nullptr)sm->resize(w, h);
		}
		void mousepress(int key, int state) {
			if (sm != nullptr)sm->mousepress(key, state);
		}
		void setPosition(float x, float y) {
			if (sm != nullptr)sm->setMousePosition(x, y);
		}
		void run() {
			double delta = glfwGetTime() - time;
			update(delta);
			time = glfwGetTime();
			int width, height;
			glfwGetWindowSize(w, &width, &height);
			glViewport(0, 0, width, height);
			render(width, height);
		}
	};
}