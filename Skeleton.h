#pragma once

#include "Joint.h"

class Skeleton {

public:

	Joint* Root;

	std::vector<Joint *> allJoints;

	bool Load(const char * filename);
	void Draw(const glm::mat4& viewProjMtx, uint shader);
	void Update();

};
