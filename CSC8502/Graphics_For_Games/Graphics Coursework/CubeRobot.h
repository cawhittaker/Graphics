#pragma once
#include "..\nclgl\scenenode.h"
#include "..\nclgl\OBJMesh.h"

class CubeRobot : public SceneNode
{
public:
	CubeRobot(void);
	~CubeRobot(void) {};
	virtual void Update(float msec);

	static void CreateCube() {
		OBJMesh*m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"cube.obj");
		cube = m;
	}

	static void DeleteCube() { delete cube; }

protected:
	static Mesh* cube;
	SceneNode* leftArm;
	SceneNode* rightArm;
	SceneNode* leftLeg;
	SceneNode* rightLeg;
	float currentleftLegrot = 0;
	float currentrightLegrot = 0;
};

