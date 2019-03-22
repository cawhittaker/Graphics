#include "Renderer.h"

bool enablePP = false;

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	CubeRobot::CreateCube(); 
	CubeFish::CreateCube();
	camera = new Camera();
	heightmap = new HeightMap(TEXTUREDIR"/terrain.raw");
	quad = Mesh::GenerateQuad();

	camera->SetPosition(Vector3(RAW_HEIGHT*HEIGHTMAP_X / 2.0f, 200.0f, RAW_WIDTH*HEIGHTMAP_X));

	light = new Light(Vector3((RAW_HEIGHT*HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f)), Vector4(0.9f, 0.9f, 1.0f, 1), (RAW_WIDTH*HEIGHTMAP_X) / 2.0f);

	reflectShader = new Shader(SHADERDIR "PerPixelVertexWater.glsl", SHADERDIR "reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR "skyboxVertex.glsl", SHADERDIR "skyboxFragment.glsl");
	lightShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "PerPixelFragment.glsl");
	roboShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl"); 
	sceneShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl"); // added
	processShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"processfrag.glsl"); // added

	if (!reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram() || !roboShader->LinkProgram() || !processShader->LinkProgram() || !sceneShader->LinkProgram()) {
		return; // added end 2
	}

	waterMap = new HeightMap(TEXTUREDIR"/waterMap.raw");
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterHeight = SOIL_load_OGL_texture(TEXTUREDIR"waterHeight2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(waterHeight, true);
	quad->SetTexture(waterTex);
	heightmap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Alien_flesh_002_COLOR.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightmap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Alien_flesh_002_NRM.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightmap->SetTexture2(SOIL_load_OGL_texture(TEXTUREDIR"Sand 002_COLOR.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "sgod_ft.tga", TEXTUREDIR "sgod_bk.tga", TEXTUREDIR "sgod_up.tga", TEXTUREDIR "sgod_dn.tga", TEXTUREDIR "sgod_rt.tga", TEXTUREDIR "sgod_lf.tga",  SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap || !quad->GetTexture() || !heightmap->GetTexture() || !heightmap->GetBumpMap() || !heightmap->GetTexture2()) { return; }

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightmap->GetTexture(), true);
	SetTextureRepeating(heightmap->GetBumpMap(), true);
	SetTextureRepeating(heightmap->GetTexture2(), true);

	init = true;
	waterRotate = 0.0f;

	projMatrix = Matrix4::Perspective(1.0f, 25000.0f, (float)width / (float)height, 45.0f);

	root = new SceneNode(); 

	for (int i = -1; i < 1;i++)
	{
		SceneNode* r = new CubeRobot();
		r->SetTransform(Matrix4::Translation(Vector3((RAW_HEIGHT*HEIGHTMAP_X / 2.0f)+ (i*200.0f), 256 * HEIGHTMAP_Y / 1.5f, RAW_HEIGHT*HEIGHTMAP_Z / 2.0f)) * Matrix4::Rotation(90.0, Vector3(1, 0, 0)));
		root->AddChild(r); 
		r->robot = true;
	}
	
	for (int i = -5; i < 1;i++) 
		for (int j = 0; j < 3; j++) 
			for (int k=0; k<4; k++) {
			SceneNode* f = new CubeFish();
			f->SetTransform(Matrix4::Translation(Vector3((RAW_HEIGHT*HEIGHTMAP_X / 1.8f) + (i*20.0f), 256 * HEIGHTMAP_Y / 3.0f + (j*20.0f), RAW_HEIGHT*HEIGHTMAP_Z / 1.7f + (k*20.0f))) * Matrix4::Rotation(90.0, Vector3(0, 0, -1)));
			root->AddChild(f);
			f->fish = true;
		}
	
	glGenTextures(1, &bufferDepthTex); // added start
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &processFBO);


	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) { return; }

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // added stop

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);




	// SCENE 2

	light2 = new Light(Vector3(-450.0f, 200.0f, 280.0f), Vector4(1, 1, 1, 1), 10000.0f);

	hellData = new MD5FileData(MESHDIR "hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);

	hellData->AddAnim(MESHDIR "idle2.md5anim");
	hellNode->PlayAnim(MESHDIR "idle2.md5anim");

	sceneShader2 = new Shader(SHADERDIR "shadowscenevert.glsl", SHADERDIR "shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR "shadowVert.glsl", SHADERDIR "shadowFrag.glsl");

	if (!sceneShader2->LinkProgram() || !shadowShader->LinkProgram()) {
		return;
	}

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	floor = Mesh::GenerateQuad();
	floor->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	floor->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));


	//SCENE 3


	quad3 = Mesh::GenerateQuad();
	quad3->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	if (!roboShader->LinkProgram() || !quad3->GetTexture()) {
		return;
	}

	root3 = new SceneNode();


	for (int i = 0; i < 5; ++i) {
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(Vector3(0, 100.0f, -300.0f + 100.0f + 100 * i)));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetBoundingRadius(100.0f);
		s->SetMesh(quad3);
		root3->AddChild(s);

		
		MD5Node *temp = new MD5Node(*hellData);

		temp->PlayAnim(MESHDIR "idle2.md5anim");
		temp->SetTransform(Matrix4::Translation(Vector3(0, 100.0f, -300.0f + 150.0f + 100 * i)));
		root3->AddChild(temp);
	}
	//root3->AddChild(new CubeRobot());

	for (int i = -1; i < 1;i++)
	{
		SceneNode* t = new CubeRobot();
		t->SetTransform(Matrix4::Translation(Vector3((0.0), 10.0 + (i*200.0f), 0.0)) * Matrix4::Rotation(90.0, Vector3(1, 0, 0)));
		root3->AddChild(t);
		
	}

	for (int i = -1; i < 1;i++) 
		{
			SceneNode* p = new SceneNode();
			p->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
			p->SetTransform(Matrix4::Translation(Vector3(0, 100.0f, -300.0f + 100.0f + 100)) *Matrix4::Rotation(90.0, Vector3(1, 0, 0)) * Matrix4::Translation(Vector3(0, 0, 100.0f + 200 * i)));
			p->SetModelScale(Vector3(200.0f, 200.0f, 200.0f));
			p->SetBoundingRadius(100.0f);
			p->SetMesh(floor);
			root3->AddChild(p);
		}
	
	for (int i = -1; i < 1;i++)
	{
		SceneNode* l = new SceneNode();
		l->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		l->SetTransform(Matrix4::Translation(Vector3(0, 100.0f, -300.0f + 100.0f + 100)) *Matrix4::Rotation(90.0, Vector3(0, 1, 0)) * Matrix4::Translation(Vector3(0, 0, 200.0f + 400 * i)));
		l->SetModelScale(Vector3(200.0f, 200.0f, 200.0f));
		l->SetBoundingRadius(100.0f);
		l->SetMesh(floor);
		root3->AddChild(l);
	}

	//snow

	snowShader = new Shader(SHADERDIR"vertex.glsl", SHADERDIR"fragment.glsl", SHADERDIR"geometry.glsl");

	if (!snowShader->LinkProgram()) {
		return;
	}
	//A new particle emitter!
	emitter = new ParticleEmitter();
	emitter2 = new ParticleEmitter();

	// Text
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	

}


Renderer::~Renderer(void) {
	delete camera;
	delete heightmap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;
	currentShader = 0;
	delete root; 
	CubeRobot::DeleteCube(); 
	CubeFish::DeleteCube();
	delete waterMap;

	delete sceneShader; // added
	delete processShader; // added
	currentShader = NULL; // added
	glDeleteTextures(2, bufferColourTex); // added
	glDeleteTextures(1, &bufferDepthTex); // added
	glDeleteFramebuffers(1, &bufferFBO); // added
	glDeleteFramebuffers(1, &processFBO); // added

	// scene 2

	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	delete hellData;
	delete hellNode;
	delete floor;

	delete sceneShader2;
	delete shadowShader;
	delete light2;

	//text
	delete basicFont;

	//snow
	delete emitter;
	delete emitter2;

}


void Renderer::UpdateScene(float msec) {
	DeltaTime += msec;
	camera->UpdateCamera(msec);
	if (camera->GetPosition().y <= (256 * HEIGHTMAP_Y / 1.5f))
		enablePP = true;
	else
		enablePP = false;

	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;
	root->Update(msec); 
	hellNode->Update(msec);
	textureMatrix.ToIdentity();
	SunTime += msec/100.0f;
	light2->SetPosition(Vector3((0.0f + (1000 * cos((SunTime*PI) / 180))), 300.0f, (0.0f + ((1000 / 2) * sin((SunTime*PI) / 180)))));
	fps_acc += msec;
	counter++;
	if (fps_acc >= 1000.f) {
		fps_string = to_string(counter);
		counter = 0;
		fps_acc = 0.f;
	}
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	frameFrustum.FromMatrix(projMatrix*viewMatrix);
	root3->Update(msec);
	emitter->Update(msec);
	emitter2->Update(msec);
	snowenable;
}

void Renderer::RenderScene() {
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	if (currentScene == 1) {
		
		glClearColor(0, 0, 0, 1);
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		DrawSkybox();
		DrawHeightmap();
		DrawWater();

		SetCurrentShader(roboShader);
		UpdateShaderMatrices();

		if (snowenable == true) {
			glUniform1i(glGetUniformLocation(snowShader->GetProgram(), "diffuseTex"), 0);

			SetShaderParticleSize(emitter->GetParticleSize());
			emitter->SetParticleSize(8.0f);
			emitter->SetParticleVariance(1.0f);
			emitter->SetLaunchParticles(16.0f);
			emitter->SetParticleLifetime(2000.0f);
			emitter->SetParticleSpeed(0.1f);
			UpdateShaderMatrices();

			emitter->Draw();
			emitter2->Draw();
		}
		
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 1);

		DrawNode(root);

		glUseProgram(0);

		if (enablePP) {
			DrawPostProcess(); // added
		}
		PresentScene(); // added

	}

	if (currentScene == 2) {

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		DrawSkybox();
		DrawShadowScene(); // First render pass ...
		DrawCombinedScene(); // Second render pass ...


	}

	if (currentScene == 3) {
		
		BuildNodeLists(root3);
		SortNodeLists();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		DrawSkybox();
	
		SetCurrentShader(roboShader);
	
		UpdateShaderMatrices();

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

		DrawNodes3();
		
	}

	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	SetCurrentShader(sceneShader);
	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	DrawText(fps_string, Vector3(0, 0, 0), 16.0f);
	DrawText("   FPS", Vector3(0, 0, 0), 16.0f);

	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SwapBuffers();
	ClearNodeLists();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap() {
	SetCurrentShader(lightShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex2"), 2);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightmap->Draw();

	glUseProgram(0);
}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "waterHeightTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH*HEIGHTMAP_X / 2.0f);
	float heightY = 256 * HEIGHTMAP_Y / 1.5f;
	float heightZ = (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f);

	//modelMatrix =
	//	Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
	//	Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
	//	Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3(0, 190.0f, 0));
	//textureMatrix.ToIdentity();
	textureMatrix = Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, waterHeight);
	waterMap->SetTexture(waterTex);
	waterMap->Draw();
	//quad->SetTexture(waterTex);
	//quad->Draw();

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Renderer::DrawNode(SceneNode*n) { 
	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform()* Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&transform);
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());

		n->Draw(*this);
	}

	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}


void Renderer::DrawPostProcess() { // added
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

		for (int i = 0; i < POST_PASSES; ++i) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

			quad->SetTexture(bufferColourTex[0]);
			quad->Draw();

			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

			quad->SetTexture(bufferColourTex[1]);
			quad->Draw();
		}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() {  // added
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	quad->SetTexture(bufferColourTex[0]);
	quad->Draw();
	glUseProgram(0);
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(
		light2->GetPosition(), Vector3(0, 0, 0));
	textureMatrix = biasMatrix *(projMatrix * viewMatrix);

	UpdateShaderMatrices();

	DrawFloor();

	DrawMeshM();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 8);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	SetShaderLight(*light2);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	DrawFloor();
	DrawMeshM();

	glUseProgram(0);
}

void Renderer::DrawMesh() {
	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3(100, 0, -150));
	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *& tempMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);


	hellNode->Draw(*this);

}

void Renderer::DrawMeshM() {
	modelMatrix.ToIdentity();
	for (int i = 0; i < 4;i++) {
		switch (i) {
		case 0: tx = 100, ty = 0, tz = 50, sx = 1, sy, 2, sz = 1, Ro = 0, rx = 1, ry = 1, rz = 1;
			break;
		case 1: tx = 200, ty = 0, tz = 100, sx = 2, sy, 1, sz = 2, Ro = 45, rx = 0, ry = 1, rz = 0;
			break;
		case 2: tx = 300, ty = 0, tz = 0, sx = 1, sy, 1, sz = 1, Ro = 90, rx = 0, ry = 1, rz = 0;
			break;
		case 3: tx = 30, ty = 0, tz = 200, sx = 1, sy, 3, sz = 1, Ro = -45, rx = 0, ry = 1, rz = 0;
			break;
		}
		
		modelMatrix = Matrix4::Translation(Vector3(tx, ty, tz)) /** Matrix4::Scale(Vector3(sx, sy, sz))*/ * Matrix4::Rotation(Ro, Vector3(rx, ry, rz));
			Matrix4 tempMatrix = textureMatrix * modelMatrix;

			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *& tempMatrix.values);

			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);



			hellNode->Draw(*this);
		}
}

void Renderer::DrawFloor() {
	modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) *
		Matrix4::Scale(Vector3(450, 450, 1));
	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	floor->Draw();
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	textureMatrix.ToIdentity();
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else { nodeList.push_back(from); }
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes3() {
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode3((*i));
	}

	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		glEnable(GL_BLEND);
		DrawNode3((*i));
		glDisable(GL_BLEND);
	}
}

void Renderer::DrawNode3(SceneNode*n) {
	if (n->GetMesh()) {
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(n->GetWorldTransform()* Matrix4::Scale(n->GetModelScale())));
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)1);

		n->Draw(*this);
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void	Renderer::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}