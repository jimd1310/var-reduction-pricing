#include "samplers/MCSampler.hpp"
#include "samplers/AntitheticSampler.hpp"
#include "samplers/ControlSampler.hpp"
#include "models/BlackScholesModel.hpp"
#include "options/EuropeanOption.hpp"
#include "options/NoOption.hpp"
#include "market/FlatDiscount.hpp"
#include "core/MonteCarloEngine.hpp"
#include "core/RandomEngine.hpp"
#include "analytics/BlackScholesClosedForm.hpp"
#include "analytics/CalibrateControl.hpp"
#include <iostream>
#include <iomanip>
#include <string_view>
#include <memory>
#include <fstream> 

int main()
{
    double S = 100.0;
    double r = 0.05;
    double v = 0.2;
    double T = 1.0;
    double K = 100.0;

    FlatDiscount discount(r); 
    BlackScholesModel model(S, r, v); 
    EuropeanOption option(K, T, OptionType::Call); 

    // Calibrate beta once 
    NoOption control(T); 
    RandomEngine calib_rng(429);
    MCSampler target_calib(model, option); 
    MCSampler control_calib(model, control); 
    double beta = calibrate_beta(
        target_calib, 
        control_calib, 
        calib_rng, 
        10000,   // max samples
        1000,    // min samples 
        0.01     // error target 
    );

    std::vector<std::size_t> path_counts = {
        1000, 2000, 5000, 10'000, 20'000, 50'000, 100'000, 1'000'000, 10'000'000
    };

    std::ofstream csv("../data/convergence_results.csv"); 
    if (!csv.is_open()) {
    std::cerr << "Failed to open CSV file\n";
    return 1;
    }
    csv << "paths,mc_se,anti_se,cv_se\n";

    for (auto n : path_counts)
    {
    // MCSampler --------------------------------------------------------------
        MCSampler mc_sampler(model, option);
        RandomEngine mc_rng(1310); 
        MonteCarloEngine mc_engine(mc_sampler);

        OnlineStatistics mc_results = mc_engine.run(n, mc_rng);
        double mc_se = discount(T) * mc_results.standard_error(); 

    // AntitheticSampler ------------------------------------------------------
        AntitheticSampler anti_sampler(model, option);
        RandomEngine anti_rng(1310); 
        MonteCarloEngine anti_engine(anti_sampler); 

        OnlineStatistics anti_results = anti_engine.run(n / 2, anti_rng);
        double anti_se = discount(T) * anti_results.standard_error();

    // ControlSampler ---------------------------------------------------------
        auto cv_target = std::make_unique<MCSampler>(model, option);
        auto cv_control = std::make_unique<MCSampler>(model, control); 
        double control_mean = S / discount(T); 
        ControlSampler cv_sampler(
            std::move(cv_target), 
            std::move(cv_control), 
            control_mean, 
            beta
        );
        RandomEngine cv_rng(1310); 
        MonteCarloEngine cv_engine(cv_sampler);
        OnlineStatistics cv_results = cv_engine.run(n, cv_rng);
        double cv_se = discount(T) * cv_results.standard_error();

        csv << n << "," << mc_se << "," << anti_se << "," << cv_se << "\n";
    }

    return 0;
}