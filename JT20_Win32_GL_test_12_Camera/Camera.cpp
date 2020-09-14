#include "Camera.h"

float GameCamera::move_speed = 0.2f;

void GameCamera::Key_Input() 
{
	if ((GetKeyState(VK_UP) & 0x80) || (GetKeyState('W') & 0x80))
	{
		MoveCamera(CamDir::FORWARD);
	}

	if ((GetKeyState(VK_DOWN) & 0x80) || (GetKeyState('S') & 0x80))
	{
		MoveCamera(CamDir::BACKWARD);
	}

	if ((GetKeyState(VK_LEFT) & 0x80) || (GetKeyState('A') & 0x80))
	{
		MoveCamera(CamDir::LEFT);
	}

	if ((GetKeyState(VK_RIGHT) & 0x80) || (GetKeyState('D') & 0x80))
	{
		MoveCamera(CamDir::RIGHT);
	}
}


void GameCamera::MoveCamera(CamDir dir) 
{
	glm::vec3 vVec = viewpos - campos;
	glm::vec3 vOrth = glm::cross(vVec, glm::vec3(0, 1.0f, 0));
	switch (dir)
	{
	case CamDir::LEFT:
		campos.x -= vOrth.x * move_speed;
		campos.z -= vOrth.z * move_speed;
		viewpos.x -= vOrth.x * move_speed;
		viewpos.z -= vOrth.z * move_speed;
		break;
	case CamDir::RIGHT:
		campos.x += vOrth.x * move_speed;
		campos.z += vOrth.z * move_speed;
		viewpos.x += vOrth.x * move_speed;
		viewpos.z += vOrth.z * move_speed;
		break;
	case CamDir::FORWARD:
		campos.x += vVec.x * move_speed;
		campos.z += vVec.z * move_speed;
		viewpos.x += vVec.x * move_speed;
		viewpos.z += vVec.z * move_speed;
		break;
	case CamDir::BACKWARD:
		campos.x -= vVec.x * move_speed;
		campos.z -= vVec.z * move_speed;
		viewpos.x -= vVec.x * move_speed;
		viewpos.z -= vVec.z * move_speed;
		break;
	default:
		break;
	}
}

void GameCamera::Mouse_Move(int *wndWidth, int *wndHeight)
{
		POINT mousePos;
		int mid_x = *wndWidth >> 1;
		int mid_y = *wndHeight >> 1;
		float angle_y = 0.0f;
		float angle_z = 0.0f;

		GetCursorPos(&mousePos);	// Get the 2D mouse cursor (x,y) position					

		if ((mousePos.x == mid_x) && (mousePos.y == mid_y)) return;

		SetCursorPos(mid_x, mid_y);	// Set the mouse cursor in the center of the window						

		// Get the direction from the mouse cursor, set a resonable maneuvering speed
		angle_y = (float)((mid_x - mousePos.x)) / 1000;
		angle_z = (float)((mid_y - mousePos.y)) / 1000;

		// The higher the value is the faster the camera looks around.
		viewpos.y += angle_z * 2;

		static short rotation_limit = 8;
		// limit the rotation around the x-axis
		if ((viewpos.y - campos.y) > rotation_limit)  viewpos.y = campos.y + rotation_limit;
		if ((viewpos.y - campos.y) < -rotation_limit)  viewpos.y = campos.y - rotation_limit;
		RotateCamera(-angle_y); // Rotate
	
}

void GameCamera::RotateCamera(float speed) 
{
	glm::vec3 vVector = viewpos - campos;	// Get the view vector
	viewpos.z = (float)(campos.z + sin(speed) * vVector.x + cos(speed) * vVector.z);
	viewpos.x = (float)(campos.x + cos(speed) * vVector.x - sin(speed) * vVector.z);
}


void GameCamera::ProcessInput(int *wndWidth, int *wndHeight, bool is_MainCam)
{
	if (is_MainCam)
	{
		this->Key_Input();
		this->Mouse_Move(wndWidth,wndHeight);
	}
}



glm::mat4 GameCamera::GetCameraMatrix(int *wndWidth, int *wndHeight)
{
	auto view = glm::lookAt(campos, viewpos, glm::vec3(0.0, 1.0, 0.0));
	auto pers = glm::perspective(glm::radians(focal_length), (float)(*wndWidth) / (float) (*wndHeight), 0.1f /*NearClip*/, 100.0f/*FarClip*/);
	return pers * view;
}