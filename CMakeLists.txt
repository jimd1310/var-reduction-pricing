cmake_minimum_required(VERSION 3.12)
project(option_pricer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------
# Core library
# -----------------------
set(OPTION_PRICER_SOURCES
     src/core/MonteCarloEngine.cpp
     src/core/OnlineCovariance.cpp
     src/core/OnlineStatistics.cpp
     src/core/RandomEngine.cpp
     src/models/BlackScholesModel.cpp
     src/options/EuropeanOption.cpp
     src/samplers/MCSampler.cpp
     src/samplers/AntitheticSampler.cpp
     src/samplers/ControlSampler.cpp
     src/analytics/BlackScholesClosedForm.cpp
     src/analytics/CalibrateControl.cpp
     src/analytics/Greeks.cpp
)

add_library(option_pricer_lib ${OPTION_PRICER_SOURCES})
target_include_directories(option_pricer_lib PUBLIC include)

# -----------------------
# Main executable
# -----------------------
add_executable(option_pricer src/main.cpp)
target_link_libraries(option_pricer option_pricer_lib)

# -----------------------
# Tests
# -----------------------
enable_testing()

add_executable(test_numerical_validation
    tests/test_numerical_validation.cpp
)
target_link_libraries(test_numerical_validation option_pricer_lib)

add_executable(test_convergence
    tests/test_convergence.cpp
)
target_link_libraries(test_convergence option_pricer_lib)

add_test(
    NAME NumericalValidation
    COMMAND test_numerical_validation
)

add_test(
    NAME Convergence
    COMMAND test_convergence
)

# -----------------------
# Benchmarks
# -----------------------
add_executable(timing_benchmark
    benchmarks/timing_benchmark.cpp
)
target_link_libraries(timing_benchmark option_pricer_lib)