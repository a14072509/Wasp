#pragma once

#include "Skeleton.h"
#include "Tokenizer.h"
#include "Vertex.h"
#include "Triangle.h"

class Skin
{
    
public:
    Skin(Skeleton *skel);
    
    Skeleton *Skel;
    std::vector<Vertex *> allVertex;
    std::vector<Vertex> bindVertex;
    std::vector<Triangle *> allTriangle;

    Tokenizer t;

    bool Load(const char *filename);
    void Draw(const glm::mat4 &viewProjMtx, uint shader);
    void Update();
    
};
