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
#include <memory>

int main()
{
    // Parameters
    double S = 100.0;
    double r = 0.05;
    double v = 0.2;
    double T = 1.0;
    double K = 100.0;

    FlatDiscount discount(r); 
    BlackScholesModel model(S, r, v); 
    EuropeanOption put(K, T, OptionType::Put);
    std::size_t n_paths = 100'000;

    // Analytic price 
    double analytic = black_scholes_price(S, K, r, v, T, OptionType::Put);

// Standard MC ----------------------------------------------------------------
    MCSampler mc_sampler(model, put); 
    RandomEngine mc_rng(1310);
    MonteCarloEngine mc_engine(mc_sampler); 

    OnlineStatistics mc_results = mc_engine.run(n_paths, mc_rng); 
    double mc_price = discount(T) * mc_results.mean();
    double mc_se = discount(T) * mc_results.standard_error();

// Antithetic Variates --------------------------------------------------------
    AntitheticSampler anti_sampler(model, put); 
    RandomEngine anti_rng(1310); 
    MonteCarloEngine anti_engine(anti_sampler); 

    // use n_paths / 2 as it computes 2x payoffs internally
    OnlineStatistics anti_results = anti_engine.run(n_paths / 2, anti_rng); 
    double anti_price = discount(T) * anti_results.mean(); 
    double anti_se = discount(T) * anti_results.standard_error();

// Control Variates -----------------------------------------------------------
    // calibrate beta using a pilot run
    NoOption control(T);
    MCSampler target_calib(model, put); 
    MCSampler control_calib(model, control);
    RandomEngine calib_rng(429); 
    double beta = calibrate_beta(
        target_calib, 
        control_calib, 
        calib_rng, 
        10'000, // max samples 
        1000,   // min samples
        0.01    // error target
    );
    
    // set up the control variate sampler
    auto cv_target = std::make_unique<MCSampler>(model, put);
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
    OnlineStatistics cv_results = cv_engine.run(n_paths, cv_rng);
    double cv_price = discount(T) * cv_results.mean();
    double cv_se = discount(T) * cv_results.standard_error();
    
// Display Results ------------------------------------------------------------

    std::cout << std::fixed << std::setprecision(4);
    std::cout << '\n';
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║   Monte Carlo European Put (S=K=100, T=1, r=5%, σ=20%)    ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";

    std::cout << "Analytical: " << analytic << '\n';
    std::cout << "Paths: " << n_paths << "\n\n";

    std::cout << "┌──────────────┬──────────┬──────────┬──────────┐\n";
    std::cout << "│ Method       │ Price    │ Std Err  │ Error    │\n";
    std::cout << "├──────────────┼──────────┼──────────┼──────────┤\n";
    std::cout << "│ Standard MC  │ " << std::setw(8) << mc_price << " │ " 
              << std::setw(8) << mc_se << " │ " << std::setw(8) 
              << std::abs(mc_price - analytic) << " │\n";
    std::cout << "│ Antithetic   │ " << std::setw(8) << anti_price << " │ " 
              << std::setw(8) << anti_se << " │ " << std::setw(8) 
              << std::abs(anti_price - analytic) << " │\n";
    std::cout << "│ Control CV   │ " << std::setw(8) << cv_price << " │ " 
              << std::setw(8) << cv_se << " │ " << std::setw(8) 
              << std::abs(cv_price - analytic) << " │\n";
    std::cout << "└──────────────┴──────────┴──────────┴──────────┘\n\n";

    std::cout << "Beta (Control): " << beta << "\n\n";

    return 0;
}