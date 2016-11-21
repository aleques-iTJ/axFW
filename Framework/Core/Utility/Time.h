#pragma once
#include "Types.h"


namespace Time
{
	namespace internal
	{
		extern uint64 frequency;
		extern uint64 current;
		extern uint64 previous;
	}


	// Ticks and updates the timer
	void			Update			 ();

	// Differences, converted	
	inline float64	GetDeltaAsMs	 () { return ((internal::current - internal::previous) * 1000.0) / internal::frequency; }
	inline float64	GetDeltaAsSeconds() { return GetDeltaAsMs() / 1000.0; }
	inline float64	GetDeltaAsHz	 () { return 1000.0 / GetDeltaAsMs(); }
}


// A bit more stateful
/*class Timer
{
	static const size_t AVERAGE_OVER = 8; // How many updates to average over

public:
	
//==============================================================================================
// 
//==============================================================================================

	void			Create			(); // Init the high resolution timer	
	void			Update			(); // Tick the timer - refreshes the data, gets the delta time, and grows the accumulator


//==============================================================================================
// Getters to return the time in various formats, including averaged to smooth variation
//==============================================================================================

	// hurf
	inline float64	GetHz			() { return 1000.0 / ms[index];	}
	inline float64	GetMs			() { return ms[index];			}
	inline float64	GetSeconds		() { return ms[index] / 1000.0;	}
	inline float64	GetHzAvg		() { return 1000.0 / _avg();	}
	inline float64	GetMsAvg		() { return _avg();				}
	inline float64	GetSecondsAvg	() { return _avg() / 1000.0;	}

	// Helper to accumulate frame time
	inline float64	GetAccumulator	() { return accumulator;		}
	inline void		ResetAccumuator	() { accumulator = 0.0;			}

	// Timings
	inline uint64	GetLastTime		() { return last;				}


//==============================================================================================
// Internal helpers
//==============================================================================================

private:
	f64 _avg()
	{
		f64 avgAccum = 0.0f;
		foreach(i, AVERAGE_OVER)
		{
			avgAccum += ms[i];
		}

		avgAccum /= AVERAGE_OVER;

		return avgAccum;
	}

protected:
	size_t _step(size_t& value)
	{
		return ++value & (AVERAGE_OVER - 1);
	}


protected:
	uint64	last				= 0;
	uint64	current				= 0;	
	float64	accumulator			= 0.0;
	float64	ms[AVERAGE_OVER];

	size_t index				= 0;
};*/