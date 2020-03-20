#pragma once

#include "Tokenizer.h"
#include "Model.h"
#include "Dof.h"

class Joint
{
public:
	Joint();
	~Joint();

	glm::vec3 offset;

	glm::mat4 toWorld;
	glm::mat4 invBind;
	glm::mat4 local;

	Model Cube;
	glm::vec3 boxMin;
	glm::vec3 boxMax;

	std::vector<Dof *> DOFS;

	Joint *parent;
	std::vector<Joint *> childrens;

	char name[256];

	bool Load(Tokenizer &t, std::vector<Joint *> &l);
	void AddChild(Joint *jnt);
	void Draw(const glm::mat4 &viewProjMtx, uint shader);
	void Update();
};
