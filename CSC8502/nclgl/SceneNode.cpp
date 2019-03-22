#include "SceneNode.h"



SceneNode::SceneNode(Mesh* mesh, Vector4 colour) {
	this->mesh = mesh;
	this->colour = colour;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);

	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
}


SceneNode::~SceneNode(void)
{
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s->parent = this;
}

void SceneNode::Draw(const OGLRenderer &r) {
	if (mesh) { mesh->Draw(); }
}

void SceneNode::Update(float msec) {
	if (robot) SetTransform(Matrix4::Translation(Vector3(transform.GetPositionVector().x, transform.GetPositionVector().y, transform.GetPositionVector().z + (msec/50.0f))) *  Matrix4::Rotation(90.0, Vector3(1, 0, 0)));
	if (fish) SetTransform(Matrix4::Translation(Vector3(transform.GetPositionVector().x - (msec / 50.0f), transform.GetPositionVector().y, transform.GetPositionVector().z)) * Matrix4::Rotation(90.0, Vector3(0, 0, -1)));

	if (parent) {
		worldTransform = parent->worldTransform * transform;
	}
	else { worldTransform = transform; }


	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(msec);
}
}