#include "Channel.h"
#include <Math.h>
#include <stdio.h>
#include <string.h>

bool Channel::load(Tokenizer &t)
{
    t.FindToken("extrapolate");
    char ex1[256];
    t.GetToken(ex1);

    char ex2[256];
    t.GetToken(ex2);

    if (strcmp(ex1, "constant") == 0)
    {
        extrapolateIn = 0;
    }
    else if (strcmp(ex1, "linear") == 0)
    {
        extrapolateIn = 1;
    }
    else if (strcmp(ex1, "cycle") == 0)
    {
        extrapolateIn = 2;
    }
    else if (strcmp(ex1, "cycle_offset") == 0)
    {
        extrapolateIn = 3;
    }
    else
    {
        extrapolateIn = 4;
    }

    if (strcmp(ex2, "constant") == 0)
    {
        extrapolateOut = 0;
    }
    else if (strcmp(ex2, "linear") == 0)
    {
        extrapolateOut = 1;
    }
    else if (strcmp(ex2, "cycle") == 0)
    {
        extrapolateOut = 2;
    }
    else if (strcmp(ex2, "cycle_offset") == 0)
    {
        extrapolateOut = 3;
    }
    else
    {
        extrapolateOut = 4;
    }

    t.FindToken("keys");
    int numKeys = t.GetInt();
    t.SkipLine();

    for (int i = 0; i < numKeys; i++)
    {
        float time = t.GetFloat();
        float value = t.GetFloat();

        Keyframe *kf = new Keyframe(time, value);

        char temp[256];
        t.GetToken(temp);

        char temp2[256];
        t.GetToken(temp2);

        if (strcmp(temp, "flat") == 0)
        {
            kf->ruleIn = 'f';
        }
        else if (strcmp(temp, "linear") == 0)
        {
            kf->ruleIn = 'l';
        }
        else if (strcmp(temp, "smooth") == 0)
        {
            kf->ruleIn = 's';
        }
        else
        {
            kf->ruleIn = 'c';
            kf->tangentIn = std::atof(temp);
        }

        if (strcmp(temp2, "flat") == 0)
        {
            kf->ruleOut = 'f';
        }
        else if (strcmp(temp2, "linear") == 0)
        {
            kf->ruleOut = 'l';
        }
        else if (strcmp(temp2, "smooth") == 0)
        {
            kf->ruleOut = 's';
        }
        else
        {
            kf->ruleOut = 'c';
            kf->tangentOut = std::atof(temp2);
        }

        keyframes.push_back(kf);
    }
    computeTangent();
    return true;
}

void Channel::computeTangent()
{
    if (keyframes.size() == 1)
    {
        computeTangentHelper('f', 0, 0);
        computeTangentHelper('f', 1, 0);
        return;
    }

    for (int i = 0; i < keyframes.size() - 1; i++)
    {
        char r1 = keyframes[i]->ruleIn;
        char r2 = keyframes[i]->ruleOut;

        computeTangentHelper(r2, 1, i);
        computeTangentHelper(r1, 0, i);
    }
    computeTangentHelper(keyframes[keyframes.size() - 1]->ruleIn, 0, keyframes.size() - 1);
    computeTangentHelper(keyframes[keyframes.size() - 1]->ruleOut, 1, keyframes.size() - 1);

    computeCubic();
}

void Channel::computeTangentHelper(char rule, int inOrOut, int frameIdx)
{
    float *tangent;
    if (inOrOut == 0)
    {
        tangent = &keyframes[frameIdx]->tangentIn;
    }
    else
    {
        tangent = &keyframes[frameIdx]->tangentOut;
    }

    switch (rule)
    {
    case 'f':
        *tangent = 0;
        break;
    case 'l':
        *tangent = computeLinear(frameIdx, inOrOut);
        break;
    case 's':
        *tangent = computeSmooth(frameIdx, inOrOut);
        break;
    default:
        break;
    }
}

void Channel::computeCubic()
{
    for (int i = 0; i < keyframes.size() - 1; i++)
    {
        float t1 = keyframes[i + 1]->time;
        float t0 = keyframes[i]->time;

        glm::vec4 ABCD = invHermite * glm::vec4(keyframes[i]->val,
                                                keyframes[i + 1]->val,
                                                (t1 - t0) * keyframes[i]->tangentOut,
                                                (t1 - t0) * keyframes[i + 1]->tangentIn);

        keyframes[i]->A = ABCD[0];
        keyframes[i]->B = ABCD[1];
        keyframes[i]->C = ABCD[2];
        keyframes[i]->D = ABCD[3];
    }
}

float Channel::computeLinear(int i, int mode)
{
    if (i == 0 && mode == 0)
    {
        return keyframes[i]->tangentOut;
    }
    else if (i == keyframes.size() - 1 && mode == 1)
    {
        return keyframes[i]->tangentIn;
    }

    float p1;
    float p0;
    float t1;
    float t0;

    if (mode == 0)
    {
        p1 = keyframes[i]->val;
        p0 = keyframes[i - 1]->val;
        t1 = keyframes[i]->time;
        t0 = keyframes[i - 1]->time;
    }
    else
    {
        p1 = keyframes[i + 1]->val;
        p0 = keyframes[i]->val;
        t1 = keyframes[i + 1]->time;
        t0 = keyframes[i]->time;
    }

    return (p1 - p0) / (t1 - t0);
}

float Channel::computeSmooth(int i, int mode)
{
    if (i == 0 || i == keyframes.size() - 1)
    {
        return computeLinear(i, mode);
    }

    float p2;
    float p0;
    float t2;
    float t0;

    p2 = keyframes[i + 1]->val;
    p0 = keyframes[i - 1]->val;
    t2 = keyframes[i + 1]->time;
    t0 = keyframes[i - 1]->time;

    return (p2 - p0) / (t2 - t0);
}

float Channel::evaluate(float time)
{
    if (keyframes.size() == 1)
    {
        return keyframes[0]->val;
    }

    int t0Idx = getSpan(time);

    if (t0Idx == -1)
    {
        return computeExtrapolationIn(time);
    }
    else if (t0Idx == keyframes.size())
    {
        return computeExtrapolationOut(time);
    }

    int t1Idx = t0Idx + 1;

    float t0 = keyframes[t0Idx]->time;
    float t1 = keyframes[t1Idx]->time;

    float u = (time - t0) / (t1 - t0);

    float a = keyframes[t0Idx]->A;
    float b = keyframes[t0Idx]->B;
    float c = keyframes[t0Idx]->C;
    float d = keyframes[t0Idx]->D;

    return d + u * (c + u * (b + u * a));
}

float Channel::computeExtrapolationIn(float time)
{
    if (extrapolateIn == 0)
    {
        return keyframes[0]->val;
    }
    else if (extrapolateIn == 1)
    {
        float b = keyframes[0]->val - keyframes[0]->time * keyframes[0]->tangentOut;
        return time * keyframes[0]->tangentOut + b;
    }
    else if (extrapolateIn == 2)
    {
        float channelLen = keyframes[keyframes.size() - 1]->time - keyframes[0]->time;
        float r = fmod(abs(time - keyframes[0]->time), channelLen);
        float timeInOffset = keyframes[keyframes.size() - 1]->time - r;
        return evaluate(timeInOffset);
    }
    else if (extrapolateIn == 3)
    {
        float channelLen = keyframes[keyframes.size() - 1]->time - keyframes[0]->time;
        int d = abs(time - keyframes[0]->time) / channelLen;
        float r = fmod(abs(time - keyframes[0]->time), channelLen);
        float valOffset = keyframes[keyframes.size() - 1]->val - keyframes[0]->val;
        float timeInOffset = keyframes[keyframes.size() - 1]->time - r;
        return evaluate(timeInOffset) - valOffset * d;
    }
    else
    {
        float channelLen = keyframes[keyframes.size() - 1]->time - keyframes[0]->time;
        int d = abs(time - keyframes[0]->time) / channelLen;

        float r = fmod(abs(time - keyframes[0]->time), channelLen);
        float timeInOffset = keyframes[keyframes.size() - 1]->time - r;

        if (d % 2 == 2)
        {
            evaluate(timeInOffset);
        }
        else
        {
            evaluate(-timeInOffset);
        }
    }
}

float Channel::computeExtrapolationOut(float time)
{
    if (extrapolateOut == 0)
    {
        return keyframes[keyframes.size() - 1]->val;
    }
    else if (extrapolateOut == 1)
    {
        float b = keyframes[keyframes.size() - 1]->val -
                  keyframes[keyframes.size() - 1]->time * keyframes[keyframes.size() - 1]->tangentIn;

        return time * keyframes[keyframes.size() - 1]->tangentIn + b;
    }
    else if (extrapolateOut == 2)
    {
        float channelLen = keyframes[keyframes.size() - 1]->time - keyframes[0]->time;
        float r = fmod(abs(time - keyframes[0]->time), channelLen);
        float timeInOffset = keyframes[0]->time + r;
        return evaluate(timeInOffset);
    }
    else if (extrapolateOut == 3)
    {
        float channelLen = keyframes[keyframes.size() - 1]->time - keyframes[0]->time;
        int d = abs(time - keyframes[0]->time) / channelLen;
        float r = fmod(abs(time - keyframes[0]->time), channelLen);
        float valOffset = keyframes[keyframes.size() - 1]->val - keyframes[0]->val;
        float timeInOffset = keyframes[0]->time + r;
        return evaluate(timeInOffset) + valOffset * d;
    }
    else
    {
        float channelLen = keyframes[keyframes.size() - 1]->time - keyframes[0]->time;
        int d = abs(time - keyframes[0]->time) / channelLen;

        float r = fmod(abs(time - keyframes[0]->time), channelLen);
        float timeInOffset = keyframes[keyframes.size() - 1]->time - r;

        if (d % 2 == 2)
        {
            evaluate(timeInOffset);
        }
        else
        {
            evaluate(-timeInOffset);
        }
    }
}

int Channel::getSpan(float time)
{
    if (time < keyframes[0]->time)
    {
        return -1;
    }
    else if (time > keyframes[keyframes.size() - 1]->time)
    {
        return keyframes.size();
    }

    for (int index = 1; index < keyframes.size() - 1; index++)
    {
        if (keyframes[index]->time > time)
        {
            return index - 1;
        }
    }

    return keyframes.size() - 2;

    /*
    //base case
    if (time <= keyframes[start]->time)
    {
        return start - 1;
    }
    else if (time > keyframes[end]->time)
    {
        return end + 1;
    }

    //binary search
    if (time <= keyframes[(end + start) / 2]->time)
    {
        return Channel::getSpan(time, start, ((end + start) / 2));
    }
    else
    {
        return Channel::getSpan(time, ((end + start) / 2) + 1, end);
    }
    */
}