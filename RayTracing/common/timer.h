// \brief
//		Time Query On Windows Platform.
//

#pragma once

#include <stdint.h> // uint32_t

double appInitTiming();
double appSeconds();
double appMicroSeconds();
int64_t appCycles();

class FPerformanceCounter
{
public:
	FPerformanceCounter() : _timestamp(0.0)
	{}

	inline void StartPerf()
	{
		_timestamp = appMicroSeconds();
	}

	// return micro-seconds elapsed
	inline double EndPerf()
	{
		double _endstamp = appMicroSeconds();
		return _endstamp - _timestamp;
	}

private:
	double  _timestamp;
};
