#pragma once
#include <vector>
#include "Keyframe.h"
#include "Tokenizer.h"

class Channel
{
public:
    std::vector<Keyframe *> keyframes;

    int extrapolateIn;
    int extrapolateOut;
    
    float evaluate(float time);
    bool load(Tokenizer &t);
    void computeTangent();
    void computeTangentHelper(char rule, int inOrOut, int frameIdx);
    void computeCubic();
    float computeExtrapolationIn(float time);
    float computeExtrapolationOut(float time);

    int getSpan(float time);
    //mode 0 in, mode 1 out
    float computeLinear(int i, int mode);
    float computeSmooth(int i, int mode);

    glm::vec4 col1 = glm::vec4(2, -3, 0, 1);
    glm::vec4 col2 = glm::vec4(-2, 3, 0, 0);
    glm::vec4 col3 = glm::vec4(1, -2, 1 , 0);
    glm::vec4 col4 = glm::vec4(1, -1, 0 , 0);
    glm::mat4 invHermite = glm::mat4(col1, col2, col3, col4);
};