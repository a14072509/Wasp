#pragma once

////////////////////////////////////////
// Tester.h
////////////////////////////////////////

#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Skeleton.h"
#include "Skin.h"
#include "Animation.h"
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////

// The 'Tester' is a simple top level application class. It creates and manages a
// window with the GLUT extension to OpenGL and it maintains a simple 3D scene
// including a camera and some other components.

class Main{
public:
	Main(const char* windowTitle, int argc, char** argv);
	~Main();

	void Update();
	void Reset();
	void Draw();

	void Quit();

	// Event handlers
	void Resize(int x, int y);
	void Keyboard(int key, int x, int y);
	void MouseButton(int btn, int state, int x, int y);
	void MouseMotion(int x, int y);
	void printJoints();
	void selectJoint();
	void selectDof(Joint * j);
	void dof();

private:
	// Window management
	int WindowHandle;
	int WinX, WinY;

	// Input
	bool LeftDown, MiddleDown, RightDown;
	int MouseX, MouseY;
	int btnType;
    
	int drawInd = -1;

	// Components
	ShaderProgram* Program;
	Skeleton* Skel;
	Skin * Sk;
	Camera* Cam;
	Animation* Ani;
	Player* Ply;
};

////////////////////////////////////////////////////////////////////////////////
