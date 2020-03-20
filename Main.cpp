////////////////////////////////////////
// Tester.cpp
////////////////////////////////////////

#include "Main.h"
#include "Tokenizer.h"
#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////////////////////

static Main *TESTER = 0;

using namespace std;

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	TESTER = new Main("Skeleton", argc, argv);
	glutMainLoop();

	delete TESTER;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

// These are really HACKS to make glut call member functions instead of static functions
static void idle() { TESTER->Update(); }
static void display() { TESTER->Draw(); }
static void resize(int x, int y) { TESTER->Resize(x, y); }
static void mousebutton(int btn, int state, int x, int y) { TESTER->MouseButton(btn, state, x, y); }
static void keyboard(unsigned char key, int x, int y) { TESTER->Keyboard(key, x, y); }
static void mousemotion(int x, int y) { TESTER->MouseMotion(x, y); }

////////////////////////////////////////////////////////////////////////////////

Main::Main(const char *windowTitle, int argc, char **argv)
{
	WinX = 1200;
	WinY = 800;
	LeftDown = MiddleDown = RightDown = false;
	MouseX = MouseY = 0;

	// Create the window
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WinX, WinY);
	glutInitWindowPosition(100, 100);
	WindowHandle = glutCreateWindow(windowTitle);
	glutSetWindowTitle(windowTitle);
	glutSetWindow(WindowHandle);

	// Background color
	glClearColor(0., 0., 0., 1.);

	// Callbacks
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemotion);
	glutPassiveMotionFunc(mousemotion);
	glutReshapeFunc(resize);

	// Initialize GLEW
	glewInit();
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// Initialize components
	Program = new ShaderProgram("Model.glsl", ShaderProgram::eRender);
	Skel = new Skeleton;
	Sk = new Skin(Skel);
	Ani = new Animation;

	if (argc == 1)
	{
		Skel->Load("wasp.skel");
		Sk->Load("wasp.skin");
		Ani->Load("wasp_walk.anim");
	}
	else if (argc == 4)
	{
		Skel->Load(argv[1]);
		Sk->Load(argv[2]);
		Ani->Load(argv[3]);
	}

	Ply = new Player(Ani, Skel);
	Cam = new Camera;
	Cam->SetAspect(float(WinX) / float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

Main::~Main()
{
	delete Program;
	delete Skel;
	delete Sk;
	delete Cam;

	glFinish();
	glutDestroyWindow(WindowHandle);
}

////////////////////////////////////////////////////////////////////////////////

void Main::Update()
{
	// Update the components in the world
		Ply->update();
		Skel->Update();
		Sk->Update();
		Cam->Update();

		// Tell glut to re-display the scene
		glutSetWindow(WindowHandle);
		glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////

void Main::Reset()
{
	Cam->Reset();
	Cam->SetAspect(float(WinX) / float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

void Main::Draw()
{
	// Begin drawing scene
	glViewport(0, 0, WinX, WinY);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw components
	//Skel->Draw(Cam->GetViewProjectMtx(), Program->GetProgramID());
	Sk->Draw(Cam->GetViewProjectMtx(), Program->GetProgramID());

	if (drawInd != -1) {
		Ply->drawchannel(drawInd);
	}

	// Finish drawing scene
	glFinish();
	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////

void Main::Quit()
{
	glFinish();
	glutDestroyWindow(WindowHandle);
	exit(0);
}

////////////////////////////////////////////////////////////////////////////////

void Main::Resize(int x, int y)
{
	WinX = x;
	WinY = y;
	Cam->SetAspect(float(WinX) / float(WinY));
}
////////////////////////////////////////////////////////////////////////////////

void Main::MouseButton(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON)
	{
		LeftDown = (state == GLUT_DOWN);
	}
	else if (btn == GLUT_MIDDLE_BUTTON)
	{
		MiddleDown = (state == GLUT_DOWN);
	}
	else if (btn == GLUT_RIGHT_BUTTON)
	{
		RightDown = (state == GLUT_DOWN);
	}
}

////////////////////////////////////////////////////////////////////////////////

void Main::MouseMotion(int nx, int ny)
{
	int maxDelta = 100;
	int dx = glm::clamp(nx - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-(ny - MouseY), -maxDelta, maxDelta);

	MouseX = nx;
	MouseY = ny;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if (LeftDown)
	{
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown)
	{
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////

void Main::Keyboard(int key, int x, int y)
{
	switch (key)
	{
	case 0x1b: // Escape
		Quit();
		break;
	case 'r':
		Reset();
		break;
	case 'j':
		selectJoint();
		break;
	case 'd':
		dof();
		break;
	}
}

void Main::dof() 
{
	cout << "Select Dof 0-71" << endl;
	string selectedDof;
	getline(cin, selectedDof);
	try
	{
		int jointIdx = stoi(selectedDof);

		if (jointIdx > 71 || jointIdx < 0)
		{
			cout << "Please Enter A Number in Range" << endl;
			cout << endl;
		}
		else
		{
			drawInd = jointIdx;
		}
	}
	catch (const std::exception &)
	{
		cout << "Please Enter A valid Number" << endl;
		cout << endl;
	}

}

void Main::selectJoint()
{
	while (true)
	{
		printJoints();

		cout << endl;
		cout << "<Joint Number> or q to quit" << endl;

		string selectedJoint;
		getline(cin, selectedJoint);

		if (selectedJoint == "q")
		{
			break;
		}

		try
		{
			int jointIdx = stoi(selectedJoint);

			if (jointIdx > Skel->allJoints.size() || jointIdx < 0)
			{
				cout << "Please Enter A Number in Range" << endl;
				cout << endl;
			}
			else
			{
				Joint *jnt = Skel->allJoints[jointIdx];
				selectDof(jnt);
			}
		}
		catch (const std::exception &)
		{
			cout << "Please Enter A Number" << endl;
			cout << endl;
		}
	}
}

void Main::selectDof(Joint *j)
{
	while (true)
	{
		cout << endl;

		for (int i = 0; i < j->DOFS.size(); i++)
		{
			cout << i;
			cout << " ";

			cout << "MIN: ";
			cout << j->DOFS[i]->getMin();
			cout << "   ";

			cout << "VALUE: ";
			cout << j->DOFS[i]->getValue();
			cout << "   ";

			cout << "MAX: ";
			cout << j->DOFS[i]->getMax();
			cout << "   ";

			cout << endl;
		}

		cout << endl;
		cout << "<dof number, value> pair for the joint --";
		cout << j->name;
		cout << "-- or q when done" << endl;

		string input;
		getline(cin, input);

		if (input == "q")
		{
			break;
		}

		try
		{
			string::size_type sz;

			int dofNum = stoi(input, &sz);
			double enteredVal = stod(input.substr(static_cast<int>(sz)), &sz);

			if (dofNum > j->DOFS.size() || dofNum < 0)
			{
				cout << "The dof number must be in range" << endl;
				cout << endl;
			}
			else
			{
				j->DOFS[dofNum]->setValue(enteredVal);
				j->Update();
				j->Draw(Cam->GetViewProjectMtx(), Program->GetProgramID());
				Sk->Update();
				Sk->Draw(Cam->GetViewProjectMtx(), Program->GetProgramID());
				Main::Draw();
			}
		}
		catch (const std::exception &e)
		{
			cout << "Please enter a pair of number" << endl;
			cout << endl;
		}
	}
}

void Main::printJoints()
{
	for (int i = 0; i < Skel->allJoints.size(); i++)
	{
		cout << i;
		cout << " ";
		cout << (Skel->allJoints)[i]->name << endl;
	}
}