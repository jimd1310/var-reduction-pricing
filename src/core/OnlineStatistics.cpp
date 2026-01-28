#include "core/OnlineStatistics.hpp"

void OnlineStatistics::add(double x) 
{
    ++n_; 
    double delta = x - mean_; 
    mean_ += delta / n_; 
    double delta2 = x - mean_; 
    m2_ += delta * delta2; 
}

double OnlineStatistics::variance() const 
{
    return (n_ > 1) ? m2_ / (n_ - 1) : 0.0;
}

double OnlineStatistics::standard_error() const 
{
    return std::sqrt(variance() / n_); 
}