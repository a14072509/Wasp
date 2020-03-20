////////////////////////////////////////
// Model.h
////////////////////////////////////////

#pragma once

#include "Core.h"

////////////////////////////////////////////////////////////////////////////////

struct ModelVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
};

////////////////////////////////////////////////////////////////////////////////

class Model {
public:
	Model();
	~Model();

	void Draw(const glm::mat4 &modelMtx,const glm::mat4 &viewProjMtx,uint shader);

	void MakeBox(const glm::vec3 &boxMin,const glm::vec3 &boxMax);
	void MakeTri(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 n1, glm::vec3 n2, glm::vec3 n3);
	
	void SetBuffers(const std::vector<ModelVertex> &vtx,const std::vector<uint> &idx);

	// Access functions

private:
	uint VertexBuffer;
	uint IndexBuffer;

	int Count;
};

////////////////////////////////////////////////////////////////////////////////
