#include "Timer.h"

Timer::Timer()
{
	// Initialize variables
	prev = cur = clock();
	t = dt = fps = 0;
}

Timer::~Timer()
{
}

void Timer::Update()
{
	// Update time variables
	prev = cur;
	cur = clock();
	dt = (cur - prev) / (float)CLOCKS_PER_SEC;
	t += dt;
	fps = 1 / dt;
}