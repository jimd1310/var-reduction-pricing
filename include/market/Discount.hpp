#pragma once

class Discount
{
public: 
    virtual ~Discount()= default; 
    virtual double operator()(double T) const = 0;
};