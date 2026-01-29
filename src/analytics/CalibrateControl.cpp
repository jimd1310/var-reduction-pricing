#include "analytics/CalibrateControl.hpp"
#include "core/OnlineCovariance.hpp"
#include <cmath>

double calibrate_beta(
    PathSampler& target,
    PathSampler& control,
    RandomEngine& rng,
    std::size_t max_samples, 
    std::size_t min_samples,
    double error_target
)
{
    OnlineCovariance stats;
    double beta = 0.0; 

    for (std::size_t i = 0; i < max_samples; ++i)
    {
        double Z = rng.normal();
        double x = target.sample(Z); 
        double y = control.sample(Z); 
        stats.add(x, y);

        if (stats.count() > min_samples)
        {
            double var_y = stats.variance_y();
            if (var_y > 0.0)
            {
                beta = stats.covariance() / var_y; 

                // Standard error of beta estimate 
                double var_x = stats.variance_x(); 
                double cov = stats.covariance(); 
                double corr = cov / std::sqrt(var_x * var_y);
                double se_beta = std::sqrt(
                    (var_x / var_y) * (1 - corr * corr) / stats.count()
                );
                // Stop when relative error < error_target
                if (std::abs(se_beta / beta) < error_target)
                    break;
            }
        }
    }
    return beta;
}