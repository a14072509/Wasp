#include "Joint.h"

Joint::Joint()
{
	offset[0] = 0;
	offset[1] = 0;
	offset[2] = 0;

	boxMin = glm::vec3(-1);
	boxMax = glm::vec3(1);
	Cube.MakeBox(boxMin, boxMax);

	Dof *rot_x = new Dof(0, -10000, 10000);
	Dof *rot_y = new Dof(0, -10000, 10000);
	Dof *rot_z = new Dof(0, -10000, 10000);
	DOFS.push_back(rot_x);
	DOFS.push_back(rot_y);
	DOFS.push_back(rot_z);

	toWorld = glm::mat4(1);
	local = glm::mat4(1);
	invBind = glm::mat4(1);
	parent = NULL;
}

Joint::~Joint()
{
	delete parent;

	for (auto i : childrens)
	{
		delete i;
	}

	for (auto i : DOFS)
	{
		delete i;
	}
}

bool Joint::Load(Tokenizer &t, std::vector<Joint *> &l)
{
	t.FindToken("{");
	while (1)
	{
		char temp[256];
		t.GetToken(temp);

		if (strcmp(temp, "offset") == 0)
		{
			offset[0] = t.GetFloat();
			offset[1] = t.GetFloat();
			offset[2] = t.GetFloat();
		}
		else if (strcmp(temp, "boxmin") == 0)
		{
			boxMin[0] = t.GetFloat();
			boxMin[1] = t.GetFloat();
			boxMin[2] = t.GetFloat();

			Cube.MakeBox(boxMin, boxMax);
		}
		else if (strcmp(temp, "boxmax") == 0)
		{
			boxMax[0] = t.GetFloat();
			boxMax[1] = t.GetFloat();
			boxMax[2] = t.GetFloat();

			Cube.MakeBox(boxMin, boxMax);
		}
		else if (strcmp(temp, "rotxlimit") == 0)
		{
			DOFS[0]->setMin(t.GetFloat());
			DOFS[0]->setMax(t.GetFloat());
		}
		else if (strcmp(temp, "rotylimit") == 0)
		{
			DOFS[1]->setMin(t.GetFloat());
			DOFS[1]->setMax(t.GetFloat());
		}
		else if (strcmp(temp, "rotzlimit") == 0)
		{
			DOFS[2]->setMin(t.GetFloat());
			DOFS[2]->setMax(t.GetFloat());
		}
		else if (strcmp(temp, "pose") == 0)
		{
			DOFS[0]->setValue(t.GetFloat());
			DOFS[1]->setValue(t.GetFloat());
			DOFS[2]->setValue(t.GetFloat());
		}
		else if (strcmp(temp, "balljoint") == 0)
		{
			Joint *jnt = new Joint;
			t.GetToken(jnt->name);
			l.push_back(jnt);
			jnt->Load(t, l);
			Joint::AddChild(jnt);
		}
		else if (strcmp(temp, "}") == 0)
		{
			return true;
		}
		else
		{
			t.SkipLine(); // Unrecognized token
		}
	}
}

void Joint::AddChild(Joint *jnt)
{
	jnt->parent = this;
	childrens.push_back(jnt);
}

void Joint::Draw(const glm::mat4 &viewProjMtx, uint shader)
{
	Cube.Draw(toWorld, viewProjMtx, shader);

	if (!childrens.empty())
	{
		for (auto childJoint : childrens)
		{
			childJoint->Draw(viewProjMtx, shader);
		}

	}
}

void Joint::Update()
{
	local = glm::rotate(DOFS[2]->getValue(), glm::vec3(0, 0, 1)) *
			glm::rotate(DOFS[1]->getValue(), glm::vec3(0, 1, 0)) *
			glm::rotate(DOFS[0]->getValue(), glm::vec3(1, 0, 0));

	local[3] = glm::vec4(offset, 1);

	if (parent == NULL)
	{
		toWorld = local;
	}
	else
	{
		toWorld = parent->toWorld * local;
	}

	if (!childrens.empty())
	{
		for (auto childJoint : childrens)
		{
			childJoint->Update();
		}
	}
}