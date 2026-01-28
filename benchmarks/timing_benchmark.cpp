#include "samplers/MCSampler.hpp"
#include "samplers/AntitheticSampler.hpp"
#include "samplers/ControlSampler.hpp"
#include "models/BlackScholesModel.hpp"
#include "options/EuropeanOption.hpp"
#include "options/NoOption.hpp"
#include "market/FlatDiscount.hpp"
#include "core/OnlineStatistics.hpp"
#include "core/MonteCarloEngine.hpp"
#include "core/RandomEngine.hpp"
#include "analytics/BlackScholesClosedForm.hpp"
#include "analytics/CalibrateControl.hpp"
#include <iostream>
#include <iomanip>
#include <string_view>
#include <chrono>
#include <memory>

using clock_type = std::chrono::high_resolution_clock; 

struct TimedResult
{
    OnlineStatistics time_result;
    OnlineStatistics mc_result; 
    double avg_time;
};

static auto print_row = [](const std::string& label, auto value) {
    std::cout << std::left << std::setw(25) << label << value << '\n';
};

void print_time_header(std::size_t N);

void print_time(
    std::string_view name,
    TimedResult result1,    // result corresponding to the name
    TimedResult result2     // a reference result
);

TimedResult time_engine(
    MonteCarloEngine& engine,
    RandomEngine& rng,
    std::size_t n_paths,
    std::size_t n_iter = 10
);

int main()
{
    constexpr std::size_t N = 5'000'000; 

    double S = 100.0;
    double r = 0.05;
    double v = 0.2;
    double T = 1.0;
    double K = 100.0;

    FlatDiscount discount(r); 
    BlackScholesModel model(S, r, v); 
    EuropeanOption option(K, T, OptionType::Call); 

// MCSampler ------------------------------------------------------------------
    MCSampler mc_sampler(model, option); 
    RandomEngine mc_rng(1310); 
    MonteCarloEngine mc_engine(mc_sampler); 

    TimedResult mc_time = time_engine(mc_engine, mc_rng, N); 

// AntitheticSampler ----------------------------------------------------------
    AntitheticSampler anti_sampler(model, option); 
    RandomEngine anti_rng(1310); 
    MonteCarloEngine anti_engine(anti_sampler); 

    // uses N / 2 paths 
    TimedResult anti_time = time_engine(anti_engine, anti_rng, N / 2); 

// ControlSampler -------------------------------------------------------------
    // Calibrate beta 
    NoOption control(T); 
    RandomEngine calib_rng(429); 
    MCSampler target_calib(model, option); 
    MCSampler control_calib(model, control); 

    double beta = calibrate_beta(
        target_calib, 
        control_calib, 
        calib_rng, 
        10'000,      // max samples 
        1000,        // min samples
        0.01         // error target
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

    MonteCarloEngine cv_engine(cv_sampler); 
    RandomEngine cv_rng(1310); 

    TimedResult cv_time = time_engine(cv_engine, cv_rng, N); 

// Results --------------------------------------------------------------------
    std::cout << std::fixed << std::setprecision(4);
    std::cout << '\n';
    print_time_header(N);
    print_time("MC", mc_time, mc_time);
    print_time("Antithetic", anti_time, mc_time);
    print_time("Control", cv_time, mc_time);
    std::cout << '\n';

    return 0;
}

void print_time_header(std::size_t N)
{
    std::cout << "========= Timing Results =========\n";
    std::cout << "Paths (effective): " << N << "\n\n";
    std::cout << std::setw(20) << "Method"
              << std::setw(20) << "Time (s)"
              << std::setw(20) << "Efficiency"
              << std::setw(20) << "- Time(%)"
              << std::setw(20) << "+ Efficiency(%)"
              << '\n';
}

void print_time(
    std::string_view name,
    TimedResult result1,    // result corresponding to the name
    TimedResult result2     // a reference result
)
{
    double eff1 = 1.0 / (result1.mc_result.standard_error() * result1.avg_time);
    double eff2 = 1.0 / (result2.mc_result.standard_error() * result2.avg_time); 
    double time_pct_decrease = 100 * (
        (result2.avg_time - result1.avg_time) / result2.avg_time
    );
    double eff_pct_increase = 100 * ((eff1 - eff2) / eff2);

        std::cout << std::setw(20) << name               // name 
                  << std::setw(20) << result1.avg_time   // time
                  << std::setw(20) << eff1               // efficiency
                  << std::setw(20) << time_pct_decrease  // relative improvement
                  << std::setw(20) << eff_pct_increase   // relative improvement        
                  << '\n';
}

TimedResult time_engine(
    MonteCarloEngine& engine,
    RandomEngine& rng,
    std::size_t n_paths,
    std::size_t n_iter
)
{
    // Warm up
    engine.run(10'000, rng);
    // Reset rng state 
    rng.seed(1310);

    OnlineStatistics time_result;
    OnlineStatistics mc_result;

    for (std::size_t i = 0; i < n_iter; ++i)
    {
        auto start = clock_type::now();
        mc_result = engine.run(n_paths, rng);  // overwrite each time
        auto end = clock_type::now();

        std::chrono::duration<double> elapsed = end - start;
        time_result.add(elapsed.count());
    }

    return {
        time_result,
        mc_result,            // last-run MC statistics
        time_result.mean(),  // average timing
    };
}