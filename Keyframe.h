#pragma once

class Keyframe
{
public:

    Keyframe(float time, float val) {
        Keyframe::time = time;
        Keyframe::val = val;
    }
    
    float time;
    float val;

    float tangentIn, tangentOut;

    /*
    RUlES

    flat => tangent = 0
    Linear => p1 - p0 / t1 - t0  
    Smooth => p2 - p0 / t2 - t0,cannot be used for first and last keyframe 
    fixed => used an arbitrary fixed result
    */ 

    char ruleIn, ruleOut;

    float A,B,C,D;
};
