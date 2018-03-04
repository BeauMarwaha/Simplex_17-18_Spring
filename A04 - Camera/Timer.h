/*----------------------------------------------
Programmer: Beau Marwaha (bcm2463@g.rit.edu)
Date: 03/2018
----------------------------------------------*/
#pragma once
#include <ctime>

class Timer
{
private:
	// Time variables
	clock_t cur, prev;

public:
	// Time in seconds since starting / constructor / declaration
	float t = 0;

	// Time in seconds between 2 most recent updates
	float dt = 0;

	// Measures 1/dt, approx immediate # of updates per second
	float fps = 0;

	// Default constructor
	Timer();

	// Deconstructor
	~Timer();

	// Update override
	void Update();
};

