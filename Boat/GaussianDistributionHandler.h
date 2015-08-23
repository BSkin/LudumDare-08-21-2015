#ifndef GAUSSIAN_DISTRIBUTION_HANDLER_H
#define GAUSSIAN_DISTRIBUTION_HANDLER_H

#include <random>

class GaussianDistributionHandler
{
public:
	static void seed(unsigned long s) { generator.seed(s); }
	static float getRandomGaussianFloat() { return distribution(generator); }
private:
	static std::default_random_engine generator;
	static std::normal_distribution<float> distribution;
};

#endif