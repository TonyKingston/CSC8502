#include "../NCLGL/window.h"
#include "Renderer.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/Shader.h"
#include "Renderer.h"



int main()	{
	Window w("Island Scene", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}
	w.LockMouseToWindow(true);
	w.ShowOSPointer(true);
	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		float dt = w.GetTimer()->GetTimeDeltaSeconds();
		renderer.UpdateScene(dt);
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		w.SetTitle("FPS: " + std::to_string(1/dt));
	}
	return 0;
}