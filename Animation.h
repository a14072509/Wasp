#pragma once
#include "Channel.h"

class Animation
{
public:
    float start;
    float end;

    std::vector<Channel *> channels;

    bool Load(const char *filename);
};