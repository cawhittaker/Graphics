#pragma once
#include "..\nclgl\scenenode.h"
#include "..\nclgl\OBJMesh.h"

class CubeFish : public SceneNode
{
public:
	CubeFish(void);
	~CubeFish(void) {};
	virtual void Update(float msec);

	static void CreateCube() {
		OBJMesh*m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"cube.obj");
		cube = m;
	}

	static void DeleteCube() { delete cube; }

protected:
	static Mesh* cube;
	SceneNode* body;
	SceneNode* tail_up;
	SceneNode* tail_dn;
	float currentheadrot = 0;
	float currentbodyrot = 0;
	float currenttailrot = 0;
};

