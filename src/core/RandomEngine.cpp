#include "core/RandomEngine.hpp"

RandomEngine::RandomEngine(unsigned int seed) 
: generator_(seed) {}

double RandomEngine::normal() 
{
    return normal_(generator_); 
}

void RandomEngine::seed(unsigned int seed)
{
    generator_.seed(seed);
}