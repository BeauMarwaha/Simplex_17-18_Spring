#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Beau Marwaha - bcm2463@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager

		//Generate the stops for this shape
		std::vector<vector3> stops;
		for (int x = 0; x < i; x++)
		{
			float angle = (PI * 2) * ((float)x / (float)i);
			float sine = sin(angle);
			float cosine = cos(angle);
			stops.push_back(vector3(cosine * fSize, sine * fSize, 0));
		}
		m_lastStops.push_back(0); //add the starting point as the initial last stop for each shape
		m_stopsList.push_back(stops); //add the generated stops to the stops list

		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);


	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	static float distanceToNextStop = 0; // Static tracker of the percentage of the way to the next stop
	static int lastFullSecond = 0; // Static tracker of the last full second of game run time

	// Calculates the current time since the last full second 
	distanceToNextStop = fmod(fTimer, 1);


	// This logic allows us to figure out when the object should start moving to the next stop by incrementing lastStop every one second
	//  as one second is how long it will take the object to traverse one stop
	if (lastFullSecond < (int)fTimer)
	{
		// Set the last full second to the current second
		lastFullSecond = (int)fTimer;

		// Reset the distance to the next stop to 0
		distanceToNextStop = 0;

		// Increment the last stop for all shapes unless this will be the final stop in which case reset last stop to 0
		for (int i = 0; i < m_uOrbits; i++)
		{
			if ((m_lastStops[i] + 1) != m_stopsList[i].size())
			{
				m_lastStops[i]++;
			}
			else
			{
				m_lastStops[i] = 0;
			}
		}
	}

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));

		vector3 v3CurrentPos = ZERO_V3;

		// LERP logic between the concurrent points in the stops list 
		//  unless this will be the final stop in which case LERP between the final and first stops
		if ((m_lastStops[i] + 1) != m_stopsList[i].size())
		{
			v3CurrentPos = glm::lerp(m_stopsList[i][m_lastStops[i]], m_stopsList[i][m_lastStops[i] + 1], distanceToNextStop);
		}
		else
		{
			v3CurrentPos = glm::lerp(m_stopsList[i][m_stopsList[i].size() - 1], m_stopsList[i][0], distanceToNextStop);
		}
		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}