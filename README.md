# Monte Carlo Option Pricer with Variance Reduction

High-performance C++17 Monte Carlo option pricer for European options with up to 85% variance reduction. 

This project implements a production-inspired Monte Carlo option pricer for European options under Black-Scholes, featuring antithetic sampling and control variates that deliver up to 2.38x effective speedup over standard Monte Carlo. 

The architecture prioritizes deterministic reproducibility (Mersenne Twister with seed control) and modular extensibility, allowing variance reduction techniques to be composed with arbitrary model-option pairs without code modification.

## Technical report 

Full mathematical derivations, convergence analysis, and numerical validation are documented in the accompanying [report (PDF)](report/VarReductionPricing.pdf).

## Key Results Summary

Analysis performed on an at-the-money European call (5M effective paths, averaged over 10 runs):

Method | Variance Reduction | Effective Speedup
-|-|-
Standard MC | Baseline | 1.0x
Antithetic | 50.8% | 2.38x
Control | 85.5% | 1.89x

Parameters: $S_0 = K = 100$, $T = 1$, $r = 0.05$, $\sigma = 0.2$.

Effective speedup is defined as the ratio of estimator efficiency, measured as inverse variance per unit compute time relative to standard Monte Carlo. 

## Quick Start 

```bash
# build 
mkdir build 
cd build 
cmake ..
cmake --build .

# run examples and tests
./option_pricer                  # runs a demo pricing example
./test_numerical_validation      # validates numerical accuracy & var reduction
./test_convergence               # generates CSV in ../data/
./timing_benchmark               # measures compute time & efficiency
```

**Dependencies:** 
- C++17-compatible compiler 
- CMake 3.12+

## Project Architecture 

The project is designed with modularity and extensibility in mind. Core components are separated into:

- Simulation engines – handle Monte Carlo path generation and random number management.

- Option definitions – encapsulate payoffs and option-specific logic.

- Variance reduction techniques – such as antithetic sampling and control variates, implemented independently so they can be applied to any simulation or option type.

This structure allows new models, options, or variance reduction methods to be added without modifying existing code, mirroring best practices in production-grade quantitative libraries.

```
OptionPricer/include/
├── analytics/                          # Model validation & calibration
│   ├── BlackScholesClosedForm.hpp      # Closed form BS for call and put
│   ├── CalibrateControl.hpp            # Calibrate β w/ pilot simulation
│   └── Greeks.hpp                      # Option sensitivities (WIP)
│
├── core/                               # RNG, Monte Carlo engine, online stats
│   ├── RandomEngine.hpp                # Deterministic Mersenne Twister wrapper
│   ├── MonteCarloEngine.hpp            # Orchestrates sampling + aggregation
│   ├── OnlineStatistics.hpp            # Welford's online algorithm
│   └── OnlineCovariance.hpp            # Welford covariance for β calibration
│
├── market/                             # Discounting and rate assumptions
│   ├── Discount.hpp                    # Discount factor interface
│   └── FlatDiscount.hpp                # Flat continuously-compounded rate
│
├── models/                             # Stochastic process simulation
│   ├── Model.hpp                       # Abstract interface
│   └── BlackScholesModel.hpp           # Black-Scholes model with GBM
│
├── options/                            # Payoff definitions
│   ├── Option.hpp                      # Abstract payoff
│   ├── NoOption.hpp                    # For control variate baseline
│   └── EuropeanOption.hpp              # Call/put payoff
│
└── samplers/                           # Variance reduction techniques
│   ├── PathSampler.hpp                 # Abstract Interface
│   ├── MCSampler.hpp                   # Standard Monte Carlo
│   ├── AntitheticSampler.hpp           # Antithetic variate sampler
│   └── ControlSampler.hpp              # Control variate sampler 
```

## Simple Usage Example

```cpp
#include "samplers/AntitheticSampler.hpp"
#include "models/BlackScholesModel.hpp"
#include "options/EuropeanOption.hpp"
#include "market/FlatDiscount.hpp"
#include "core/MonteCarloEngine.hpp"
#include "core/RandomEngine.hpp"
#include <iostream>

int main()
{
    double S = 120.0;
    double r = 0.025;
    double v = 0.2;
    double T = 0.5;
    double K = 110.0;

    FlatDiscount discount(r); 
    BlackScholesModel model(S, r, v); 
    EuropeanOption put(K, T, OptionType::Put);
    std::size_t n_paths = 100'000;

    AntitheticSampler sampler(model, put); 
    RandomEngine rng(1310); 
    MonteCarloEngine engine(sampler); 

    OnlineStatistics results = engine.run(n_paths, rng); 
    double price = discount(T) * results.mean(); 
    double se = discount(T) * results.standard_error();

    std::cout << "Price: " << price << " ± " << se << '\n';
    return 0;
}
```

## Known Limitations 

- **Product scope:** Currently restricted to European vanilla options. Path-dependent securities (Asian, barrier) and early-exercise features (American/Bermudan) require Brownian bridge path generation and least-squares Monte Carlo (Longstaff-Schwartz) extensions, respectively.

- **Calibration stability:** Calibrated control variate coefficients ($\beta_{\text{call}} = 0.693$, $\beta_{\text{put}} = -0.315$) empirically converge to theoretical Delta. However, $\beta$  stability across volatility surfaces $\sigma$, tenors $T$, and moneyness $S/K$ requires profiling for production deployment.

- **Measurement noise:** Single-run benchmarks exhibit ±5-10% noise due to OS scheduling and cache effects. 

## Future Work

**Product Expansion**

Extend coverage to path-dependent and multi-asset derivatives, such as:

- Barrier options and Asian options
- Multi-asset baskets

**Model Extensions**

Progress beyond Black-Scholes to capture market microstructure, such as:

- Heston model: Stochastic volatility with characteristic function methods for Fourier-based pricing

**Risk Management**

- Adding sensitivities (Greeks) and stress-testing model stability via parameter perturbation.

## References 

[1] Glasserman, P 2004, *Monte Carlo Methods in Financial Engineering*, Springer, New York.

## License

MIT License - see LICENSE file for details
