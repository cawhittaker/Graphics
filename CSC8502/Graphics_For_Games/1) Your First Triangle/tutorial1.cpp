#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("My First OpenGL 3 Triangle!", 800 , 600, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.RenderScene();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_O)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_P)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	return 0;
}