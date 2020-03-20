#pragma once

#include "Vertex.h"
#include "Model.h"

class Triangle
{

public:
    std::vector<Vertex *> vertexs;
    Model tri;

    void Draw(const glm::mat4 &viewProjMtx, uint shader);
};