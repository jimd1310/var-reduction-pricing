# pragma once 
#include <random> 

class RandomEngine 
{
public: 
    explicit RandomEngine(unsigned int seed = 1310); 
    
    double normal(); 
    void seed(unsigned int seed);

private: 
    std::mt19937_64 generator_;
    std::normal_distribution<double> normal_{0.0, 1.0}; 
};