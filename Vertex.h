#pragma once

#include "Core.h"
#include "Joint.h"

class Vertex
{

public:
    glm::vec4 position;    
    glm::vec4 normal;

    std::vector<std::pair<Joint *, float>> skinWeights;

};
