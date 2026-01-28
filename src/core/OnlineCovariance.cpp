#include "core/OnlineCovariance.hpp"

void OnlineCovariance::add(double x, double y)
{
    ++n_; 
    double delta_x = x - mean_x_; 
    mean_x_ += delta_x / n_; 
    double delta_y = y - mean_y_; 
    mean_y_ += delta_y / n_; 
    c_ += delta_x * (y - mean_y_); 
    var_x_ += delta_x * (x - mean_x_);
    var_y_ += delta_y * (y - mean_y_); 
}

double OnlineCovariance::covariance() const 
{
    return n_ > 1 ? c_ / (n_ - 1) : 0.0; 
} 

double OnlineCovariance::variance_x() const 
{
    return n_ > 1 ? var_x_ / (n_ - 1) : 0.0; 
}

double OnlineCovariance::variance_y() const 
{ 
    return n_ > 1 ? var_y_ / (n_ - 1) : 0.0;
}
