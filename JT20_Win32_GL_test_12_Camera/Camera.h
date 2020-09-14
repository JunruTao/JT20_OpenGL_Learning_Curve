#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>
#include "glm/glm.hpp"
#include "glm/common.hpp"
#include "glm/ext.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"



enum class CamDir
{
	LEFT,RIGHT,FORWARD,BACKWARD
};


class Camera 
{
protected:
	Camera() 
	{
		campos = { 0.0f,4.0f,10.0f };
		viewpos = campos + glm::vec3( 0.0f,0.0f,-1.0f );
		focal_length = 45.0f;
	};
	virtual ~Camera() {};
	virtual void ProcessInput(int *wndWidth, int *wndHeight, bool is_MainCam = false) = 0;
	virtual void RotateCamera(float speed) = 0;
	virtual glm::mat4 GetCameraMatrix(int *wndWidth, int *wndHeight) = 0;

	glm::vec3 campos;
	glm::vec3 viewpos;
	float focal_length;
};




class GameCamera: public Camera
{
public:
	GameCamera() {};
	~GameCamera() {};

	void Key_Input();

	void ProcessInput(int *wndWidth, int *wndHeight, bool is_MainCam = false) override;
	glm::mat4 GetCameraMatrix(int *wndWidth, int *wndHeight) override;

private:
	static float move_speed;


	void Mouse_Move(int *wndWidth, int *wndHeight);
	void MoveCamera(CamDir dir);
	void RotateCamera(float speed) override;
};

#endif

