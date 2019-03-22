#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {

public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void SwitchToPerspective();
	void SwitchToOrthographic();

	inline void SetScale(float s) { scale = s; }
	inline void SetRotationx(float xr) { rotationx = xr; }
	inline void SetRotationy(float yr) { rotationy = yr; }
	inline void SetRotationz(float zr) { rotationz = zr; }
	inline void Setfield_of_view(float fov) { field_of_view = fov; }
	inline void SetPosition(Vector3 p) { position = p; }

protected:
	Mesh* triangle;
	Camera* camera;

	float scale;
	float rotationx;
	float rotationy;
	float rotationz;
	float field_of_view;
	Vector3 position;

};

