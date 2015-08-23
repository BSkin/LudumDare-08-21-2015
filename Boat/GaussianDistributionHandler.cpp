#include "GaussianDistributionHandler.h"

std::default_random_engine GaussianDistributionHandler::generator = std::default_random_engine();
std::normal_distribution<float> GaussianDistributionHandler::distribution = std::normal_distribution<float>(0.0f, 1.0f);