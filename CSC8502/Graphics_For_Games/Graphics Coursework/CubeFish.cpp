#include "CubeFish.h"

Mesh* CubeFish::cube = NULL;

CubeFish::CubeFish(void)
{
	SceneNode * head = new SceneNode(cube, Vector4(0, 1, 0, 1)); //Green
	head->SetModelScale(Vector3(2,2, 1));
	head->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	AddChild(head);
	
	
	body = new SceneNode(cube, Vector4(1, 0, 0, 1)); // RED
	body->SetModelScale(Vector3(2, 6, 1));
	body->SetTransform(Matrix4::Translation(Vector3(0,3, 0)));
	head->AddChild(body);

	tail_up = new SceneNode(cube, Vector4(1, 1, 0, 1)); // RED
	tail_up->SetModelScale(Vector3(1, 2, 1));
	tail_up->SetTransform(Matrix4::Translation(Vector3(1, 14, 0)));
	head->AddChild(tail_up);

	tail_dn = new SceneNode(cube, Vector4(1, 1, 0, 1)); // RED
	tail_dn->SetModelScale(Vector3(1, 2, 1));
	tail_dn->SetTransform(Matrix4::Translation(Vector3(-1, 14, 0)));
	head->AddChild(tail_dn);

}


void CubeFish::Update(float msec) {

	currentheadrot += (msec / 100.0);
	currenttailrot += (msec / 100.0);
	

	Vector3 headpos = body->GetTransform().GetPositionVector();
	body->SetTransform(Matrix4::Translation(headpos) * Matrix4::Rotation(sin(currentheadrot) * 20.0f, Vector3(0, 1, 0)));

	Vector3 tail_uppos = tail_up->GetTransform().GetPositionVector();
	tail_up->SetTransform(Matrix4::Translation(tail_uppos) * Matrix4::Rotation(sin(currenttailrot) * 45.0f, Vector3(1, 0, 0))* Matrix4::Rotation(45.0, Vector3(0,0,1)));

	Vector3 tail_pos = tail_dn->GetTransform().GetPositionVector();
	tail_dn->SetTransform(Matrix4::Translation(tail_pos) * Matrix4::Rotation(sin(currenttailrot) * 45.0f, Vector3(1, 0, 0)) * Matrix4::Rotation(-45.0, Vector3(0, 0, 1)));

	SceneNode::Update(msec);
}