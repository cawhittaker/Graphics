#pragma comment(lib, "nclgl.lib")

#include "../../nclgl/window.h"
#include "Renderer.h"

int main() {

	Window w("scene 1", 1920,1080,true);
	if(!w.HasInitialised()) {
		return -1;
	}
	
		Renderer renderer(w);
		if (!renderer.HasInitialised()) {
			return -1;
		}

		w.LockMouseToWindow(true);
		w.ShowOSPointer(false);

		renderer.currentScene = 1;

		while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
			
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
				renderer.currentScene = 1;

			}

			if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
				renderer.currentScene = 2;
			}

			if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
				renderer.currentScene = 3;
			}
	
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_9)) {
				renderer.snowenable = true;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_8)) {
				renderer.snowenable = false;
			}

			renderer.UpdateScene(w.GetTimer()->GetTimedMS());
			renderer.RenderScene();

		}

	return 0;
}