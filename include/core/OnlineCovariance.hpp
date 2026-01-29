#pragma once 
#include <cstddef> 
#include <cmath> 

/// A class for calculating Welford online covariance.
class OnlineCovariance
{
public: 
    void add(double x, double y);

    std::size_t count() const { return n_; }
    double covariance() const;
    double variance_x() const; 
    double variance_y() const;

private: 
    std::size_t n_ = 0; 
    double mean_x_ = 0.0; 
    double mean_y_ = 0.0; 
    double var_x_ = 0.0; 
    double var_y_ = 0.0;
    double c_ = 0.0; 
};