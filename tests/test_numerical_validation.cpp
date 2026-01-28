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

static auto print_row = [](const std::string& label, auto value) {
    std::cout << std::left << std::setw(25) << label << value << '\n';
};

void print_results_header(
    double analytic
);

void print_result(
    std::string_view name, 
    std::size_t n_paths, 
    double price, 
    double var, 
    double se, 
    double analytic
);

void print_variance(
    std::string_view name1, 
    std::string_view name2, 
    double var1, 
    double var2 
);

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
    std::size_t n_paths = 200'000;

// MCSampler ------------------------------------------------------------------
    MCSampler mc_sampler(model, option); 
    RandomEngine mc_rng(1310);
    MonteCarloEngine mc_engine(mc_sampler); 

    OnlineStatistics mc_results = mc_engine.run(n_paths, mc_rng); 
    double mc_price = discount(T) * mc_results.mean();
    double mc_se = discount(T) * mc_results.standard_error();
    double mc_var = mc_se * mc_se; 

// AntitheticSampler ----------------------------------------------------------
    AntitheticSampler anti_sampler(model, option); 
    RandomEngine anti_rng(1310); 
    MonteCarloEngine anti_engine(anti_sampler); 

    // use n_paths / 2 as it uses 2x payoff computation internally 
    OnlineStatistics anti_results = anti_engine.run(n_paths / 2, anti_rng); 
    double anti_price = discount(T) * anti_results.mean(); 
    double anti_se = discount(T) * anti_results.standard_error();
    double anti_var = anti_se * anti_se; 

// ControlSampler -------------------------------------------------------------
    // calibrate beta using a pilot run
    NoOption control(T);
    MCSampler target_calib(model, option); 
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
    OnlineStatistics cv_results = cv_engine.run(n_paths, cv_rng);
    double cv_price = discount(T) * cv_results.mean();
    double cv_se = discount(T) * cv_results.standard_error();
    double cv_var = cv_se * cv_se;

// Display Results ------------------------------------------------------------
    double analytic = black_scholes_price(S, K, r, v, T, OptionType::Call); 
    std::cout << std::fixed << std::setprecision(4);

    std::cout << '\n';
    print_results_header(analytic);
    print_result("MCSampler", n_paths, mc_price, mc_var, mc_se, analytic);
    print_result("AntitheticSampler", n_paths / 2, anti_price, anti_var, anti_se, analytic);
    print_variance("Antithetic", "MC", anti_var, mc_var);

    print_result("ControlSampler", n_paths, cv_price, cv_var, cv_se, analytic);
    print_variance("Control", "MC", cv_var, mc_var);
    print_variance("Control", "Antithetic", cv_var, anti_var);
    print_row("Beta:", beta);
    std::cout << '\n';

    return 0;
}

void print_results_header(
    double analytic
)
{
    std::cout << "============ Numerical Test Results ============" << '\n';
    print_row(" Analytic Price:", analytic);
    std::cout << '\n';
}

void print_result(
    std::string_view name, 
    std::size_t n_paths, 
    double price, 
    double var, 
    double se, 
    double analytic
) 
{
    std::cout << name << " Results" << '\n';
    std::cout << "------------------------------------------------" << '\n';
    print_row(" Number of Paths:", n_paths);
    print_row(" Simulated Price:", price); 
    print_row(" Variance:", var);
    print_row(" Standard Error:", se);
    print_row(" Absolute Error:", std::abs(analytic - price));

    double z = (price - analytic) / se; 
    double CI_low = price - 1.96 * se; 
    double CI_up = price + 1.96 * se; 

    print_row(" z-score vs analytic:", z); 
    std::cout << std::left << std::setw(25) << " 95% CI" << 
    "[" << CI_low << ", " << CI_up << "]" << '\n';
    std::cout << '\n';
}

void print_variance(
    std::string_view name1, 
    std::string_view name2, 
    double var1, 
    double var2 
)
{
    std::cout << "Variance of " << name1 << " vs " << name2 << ":" << '\n';
    print_row(" Variance Ratio:", var1 / var2);
    print_row(" Variance Reduction(%):", (1.0 - var1 / var2) * 100);
    std::cout << '\n';
}