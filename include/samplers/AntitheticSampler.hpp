#pragma once
#include "samplers/PathSampler.hpp"
#include "models/Model.hpp"
#include "options/Option.hpp"
#include "core/RandomEngine.hpp"

class AntitheticSampler : public PathSampler
{
public: 
    AntitheticSampler(
        const Model& model, 
        const Option& option
    );

    double sample(double Z) const override; 

private: 
    const Model& model_; 
    const Option& option_;
};