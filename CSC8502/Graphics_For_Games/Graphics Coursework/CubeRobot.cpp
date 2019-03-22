#include "CubeRobot.h"

Mesh* CubeRobot::cube = NULL;

CubeRobot::CubeRobot(void)
{
	SceneNode*body = new SceneNode(cube, Vector4(1, 0, 0, 1)); // RED
	body->SetModelScale(Vector3(10, 15, 5));
	body->SetTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	AddChild(body);

	SceneNode * head = new SceneNode(cube, Vector4(0, 1, 0, 1)); //Green
	head->SetModelScale(Vector3(5, 5, 5));
	head->SetTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	body->AddChild(head);

	leftArm = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	leftArm->SetModelScale(Vector3(3, -18, 3));
	leftArm->SetTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	body->AddChild(leftArm);

	rightArm = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	rightArm->SetModelScale(Vector3(3, 18, 3));
	rightArm->SetTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	body->AddChild(rightArm);

	leftLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	leftLeg->SetModelScale(Vector3(3, -17.5, 3));
	leftLeg->SetTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	body->AddChild(leftLeg);

	rightLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); //Blue
	rightLeg->SetModelScale(Vector3(3, -17.5, 3));
	rightLeg->SetTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	body->AddChild(rightLeg);

	body->SetBoundingRadius(40.0f);
	head->SetBoundingRadius(20.0f);

	leftArm->SetBoundingRadius(50.0f);
	rightArm->SetBoundingRadius(50.0f);

	leftLeg->SetBoundingRadius(50.0f);
	rightLeg->SetBoundingRadius(50.0f);
}


void CubeRobot::Update(float msec) {
	

	leftArm->SetTransform(leftArm->GetTransform() * Matrix4::Rotation(msec / 10.0f, Vector3(1, 0, 0)));
	rightArm->SetTransform(rightArm->GetTransform() * Matrix4::Rotation((msec / 10.0f) , Vector3(1, 0, 0)));

	currentleftLegrot += (msec/100.0 );
	currentrightLegrot -= (msec/100.0 );

	Vector3 legpos = leftLeg->GetTransform().GetPositionVector();
	leftLeg->SetTransform(Matrix4::Translation(legpos) * Matrix4::Rotation(sin(currentleftLegrot) * 45.0f, Vector3(1, 0, 0)));

	Vector3 Rlegpos = rightLeg->GetTransform().GetPositionVector();
	rightLeg->SetTransform(Matrix4::Translation(Rlegpos) * Matrix4::Rotation(sin(currentrightLegrot) * 45.0f, Vector3(1, 0, 0)));
	
	SceneNode::Update(msec);
}
