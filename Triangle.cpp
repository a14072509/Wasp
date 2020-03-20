#include "Triangle.h"

void Triangle::Draw(const glm::mat4 &viewProjMtx, uint shader)
{
    tri.MakeTri(vertexs[0]->position, vertexs[1]->position, vertexs[2]->position,
                vertexs[0]->normal, vertexs[1]->normal, vertexs[2]->normal);

    tri.Draw(glm::mat4(1), viewProjMtx, shader);
}