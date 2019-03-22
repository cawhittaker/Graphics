#include "CubeRobot.h"

Mesh* CubeRobot::cube = NULL;

CubeRobot::CubeRobot(void)
{
	//SceneNode*body = new SceneNode(cube, Vector4(1, 0, 0, 1)); // RED
	//body->SetModelScale(Vector3(10, 15, 5));
	//body->SetTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	//AddChild(body);

	//head = new SceneNode(cube, Vector4(0, 1, 0, 1)); //Green
	//head->SetModelScale(Vector3(5, 5, 5));
	//head->SetTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	//body->AddChild(head);

	//leftArm= new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	//leftArm->SetModelScale(Vector3(3, -18, 3));
	//leftArm->SetTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	//body->AddChild(leftArm);

	//rightArm = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	//rightArm->SetModelScale(Vector3(3, -18, 3));
	//rightArm->SetTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	//body->AddChild(rightArm);

	//SceneNode* leftLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	//leftLeg->SetModelScale(Vector3(3, -17.5, 3));
	//leftLeg->SetTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	//body->AddChild(leftLeg);

	//SceneNode* rightLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	//rightLeg->SetModelScale(Vector3(3, -17.5, 3));
	//rightLeg->SetTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	//body->AddChild(rightLeg);

	head = new SceneNode(cube, Vector4(0, 1, 0, 1)); //Green
	head->SetModelScale(Vector3(4, 4, 2));
	head->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	head->AddChild(head);


	SceneNode * body = new SceneNode(cube, Vector4(1, 0, 0, 1)); // RED
	body->SetModelScale(Vector3(12, 6, 2));
	body->SetTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	head->AddChild(body);

	tail_up = new SceneNode(cube, Vector4(1, 0, 0, 1)); // RED
	tail_up->SetModelScale(Vector3(6, 2, 2));
	tail_up->SetTransform(Matrix4::Translation(Vector3(32, 10, 0)));
	head->AddChild(tail_up);

	tail_dn = new SceneNode(cube, Vector4(1, 0, 0, 1)); // RED
	tail_dn->SetModelScale(Vector3(6, 2, 2));
	tail_dn->SetTransform(Matrix4::Translation(Vector3(32, -10, 0)));
	head->AddChild(tail_dn);
}


void CubeRobot::Update(float msec) {
	/*transform = transform* Matrix4::Rotation(msec / 10.0f, Vector3(0, 1, 0));

	head->SetTransform(head->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(0, 1, 0)));
	leftArm->SetTransform(leftArm->GetTransform() * Matrix4::Rotation(-msec / 10.0f, Vector3(1, 0, 0)));
	rightArm->SetTransform(rightArm->GetTransform() * Matrix4::Rotation(msec / 10.0f, Vector3(1, 0, 0)));

	SceneNode::Update(msec);*/
	currentheadrot += (msec / 100.0);
	currenttailrot = (msec / 100.0);


	Vector3 headpos = head->GetTransform().GetPositionVector();
	head->SetTransform(Matrix4::Translation(headpos) * Matrix4::Rotation(sin(currentheadrot) * 45.0f, Vector3(0, 1, 0)));

	Vector3 tail_uppos = tail_up->GetTransform().GetPositionVector();
	tail_up->SetTransform(Matrix4::Translation(tail_uppos) * Matrix4::Rotation(sin(currenttailrot) * 45.0f, Vector3(1, 0, 0)));

	Vector3 tail_pos = tail_dn->GetTransform().GetPositionVector();
	tail_dn->SetTransform(Matrix4::Translation(tail_pos) * Matrix4::Rotation(sin(currenttailrot) * 45.0f, Vector3(1, 0, 0)));


}