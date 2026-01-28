#pragma once 

class PathSampler
{
public: 
    virtual ~PathSampler() = default; 

    virtual double sample(double Z) const = 0;
};