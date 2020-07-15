// Basic types & functions
//
//

#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>


// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// constants
const double kInfinity = std::numeric_limits<double>::infinity();
const double kPi = 3.1415926535897932385;
const double kHalfPi = kPi * 0.5;
const double kTwoPi = kPi * 2.0;
const double kPiOver180 = kPi / 180.0;
const double KINDA_SMALL_NUMBER = 0.0001;

// utility functions
inline double degrees_2_radians(double degrees)
{
	return degrees * kPiOver180;
}

inline double clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

inline double random_double() {
	return rand() * (1.0 / (RAND_MAX + 1.0));
}

inline double random_double(double min, double max)
{
	// return a random real in [min, max).
	return min + (max - min) * random_double();
}

inline int random_int(int min, int max)
{
	// return a random integer in [min, max]
	return static_cast<int>(random_double((double)min, (double)(max + 1)));
}

template<typename T>
T slerp(const T& a, const T& b, double t)
{
	return (a + t * (b - a));
}

inline double fract(double x)
{
	return x - floor(x);
}

