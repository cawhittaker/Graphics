#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/SceneNode.h"
#include "CubeRobot.h"
#include "CubeFish.h"
#include "../nclgl/MD5Mesh.h"
#include "../nclgl/MD5Node.h"
#include <Math.h>
#include "TextMesh.h"
#include <algorithm>
#include "../nclgl/Frustum.h"
#include "../nclgl/ParticleEmitter.h"	

#define POST_PASSES 10
#define SHADOWSIZE 2018 

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	int currentScene;
	float fps_acc = 0.f;
	int counter = 0;
	std::string fps_string = "";
	bool snowenable;

	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader * roboShader;
	HeightMap* heightmap;
	Mesh* quad;

	Light* light;
	Camera* camera;

	GLuint cubeMap;

	float waterRotate;
	GLuint waterTex;
	GLuint waterHeight;
	HeightMap* waterMap;

	void DrawNode(SceneNode*n); // added
	SceneNode* root; // added
	
	void PresentScene();
	void DrawPostProcess();

	Shader* sceneShader;
	Shader* processShader;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;

	//Scene 2

	void DrawMesh();
	void DrawFloor();
	void DrawShadowScene();
	void DrawCombinedScene();

	Shader * sceneShader2;
	Shader * shadowShader;

	GLuint shadowTex;
	GLuint shadowFBO;

	

	MD5FileData * hellData;
	MD5Node * hellNode;
	Mesh * floor;
	Light * light2;
	
	float SunTime =0;
	

	Font*	basicFont;


	//Scene 3

	float tx, ty, tz, sx, sy, sz, Ro, rx, ry, rz;
	float hellnight[6];
	void DrawMeshM();
	Mesh* quad3;
	Frustum frameFrustum;
	SceneNode* root3;
	

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes3();
	void DrawNode3(SceneNode*n);


	// snow

	Shader * snowShader;
	void	SetShaderParticleSize(float f);	//And a new setter

	ParticleEmitter*	emitter;	//A single particle emitter
	ParticleEmitter*	emitter2;
};

