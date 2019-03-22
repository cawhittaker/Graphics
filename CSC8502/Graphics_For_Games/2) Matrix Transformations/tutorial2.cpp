#pragma comment(lib, "nclgl.lib")

#include "../../nclGL/window.h"
#include "Renderer.h"

int main() {

	Window w("Vertex Transformation!",800,600,false);
	if(!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	float scale		= 100.0f;
	float rotationx	= 0.0f;
	float rotationy = 0.0f;
	float rotationz = 0.0f;
	float field_of_view = 45.0f;
	Vector3 position(0, 0, -1500.0f);

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_1)) 
			renderer.SwitchToOrthographic();
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_2))
			renderer.SwitchToPerspective();

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_PLUS))
			++scale;
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_MINUS))
			--scale;

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))
			++rotationy;
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT))
			--rotationy;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))
			++rotationx;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))
			--rotationx;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_Z))
			++rotationz;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_X))
			--rotationz;

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_K))
			position.y -= 1.0f;
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_I))
			position.y+= 1.0f;

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_J))
			position.x-= 1.0f;
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_L))
			position.x+= 1.0f;

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_O))
			position.z-= 1.0f;
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_P))
			position.z+= 1.0f;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_V))
		{
			renderer.SwitchToPerspective();
			++field_of_view;
		}
		
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_B))
		{
			renderer.SwitchToPerspective();
			--field_of_view;
		}

		renderer.SetRotationx(rotationx);
		renderer.SetRotationy(rotationy);
		renderer.SetRotationz(rotationz);
		renderer.SetScale(scale);
		renderer.SetPosition(position);
		renderer.Setfield_of_view(field_of_view);
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

		return 0;
}