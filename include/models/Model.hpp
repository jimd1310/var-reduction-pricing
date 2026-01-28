#pragma once

class Model 
{
public: 
    virtual ~Model() = default; 
    virtual double simulate(double T, double Z) const = 0; 
};