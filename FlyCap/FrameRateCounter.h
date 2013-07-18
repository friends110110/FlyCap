#pragma once
#include <deque>
class FrameRateCounter
{
public:
	FrameRateCounter( unsigned long queueLength = 120 );
	~FrameRateCounter(void);
	double GetFrameRate();
	void SetFrameRate( double frameRate );
	void NewFrame();
protected:
	double m_frameRate;
	std::deque<double>  m_frameTime;
	__int64  m_frequency;
	__int64  m_lastTime;
	__int64  m_prevTime;
};

