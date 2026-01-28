#pragma once 
#include <cstddef> 
#include <cmath> 

class OnlineStatistics
{
public: 
    void add(double x); 

    std::size_t count() const { return n_; } 
    double mean() const { return mean_; }
    double variance() const; 
    double standard_error() const; 

private: 
    std::size_t n_ = 0; 
    double mean_ = 0.0; 
    double m2_ = 0.0; 
};