#include "MyCamera.h"
using namespace Simplex;

//Accessors
void Simplex::MyCamera::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }

vector3 Simplex::MyCamera::GetPosition()
{
	return m_v3Position;
}

void Simplex::MyCamera::SetTarget(vector3 a_v3Target) { m_v3Target = a_v3Target; }

vector3 Simplex::MyCamera::GetTarget()
{
	return m_v3Target;
}

void Simplex::MyCamera::SetUp(vector3 a_v3Up) { m_v3Up = a_v3Up; }

vector3 Simplex::MyCamera::GetUp()
{
	return m_v3Up;
}

void Simplex::MyCamera::SetPerspective(bool a_bPerspective) { m_bPerspective = a_bPerspective; }

void Simplex::MyCamera::SetFOV(float a_fFOV) { m_fFOV = a_fFOV; }

void Simplex::MyCamera::SetResolution(vector2 a_v2Resolution) { m_v2Resolution = a_v2Resolution; }

void Simplex::MyCamera::SetNearFar(vector2 a_v2NearFar) { m_v2NearFar = a_v2NearFar; }

void Simplex::MyCamera::SetHorizontalPlanes(vector2 a_v2Horizontal) { m_v2Horizontal = a_v2Horizontal; }

void Simplex::MyCamera::SetVerticalPlanes(vector2 a_v2Vertical) { m_v2Vertical = a_v2Vertical; }

matrix4 Simplex::MyCamera::GetProjectionMatrix(void) { return m_m4Projection; }

matrix4 Simplex::MyCamera::GetViewMatrix(void) { CalculateViewMatrix(); return m_m4View; }

Simplex::MyCamera::MyCamera()
{
	Init(); //Init the object with default values
}

Simplex::MyCamera::MyCamera(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	Init(); //Initialize the object
	SetPositionTargetAndUp(a_v3Position, a_v3Target, a_v3Upward); //set the position, target and up
}

Simplex::MyCamera::MyCamera(MyCamera const& other)
{
	m_v3Position = other.m_v3Position;
	m_v3Target = other.m_v3Target;
	m_v3Up = other.m_v3Up;

	m_bPerspective = other.m_bPerspective;

	m_fFOV = other.m_fFOV;

	m_v2Resolution = other.m_v2Resolution;
	m_v2NearFar = other.m_v2NearFar;

	m_v2Horizontal = other.m_v2Horizontal;
	m_v2Vertical = other.m_v2Vertical;

	m_m4View = other.m_m4View;
	m_m4Projection = other.m_m4Projection;
}

MyCamera& Simplex::MyCamera::operator=(MyCamera const& other)
{
	if (this != &other)
	{
		Release();
		SetPositionTargetAndUp(other.m_v3Position, other.m_v3Target, other.m_v3Up);
		MyCamera temp(other);
		Swap(temp);
	}
	return *this;
}

void Simplex::MyCamera::Init(void)
{
	ResetCamera();
	CalculateProjectionMatrix();
	CalculateViewMatrix();
	//No pointers to initialize here
}

void Simplex::MyCamera::Release(void)
{
	//No pointers to deallocate yet
}

void Simplex::MyCamera::Swap(MyCamera & other)
{
	std::swap(m_v3Position, other.m_v3Position);
	std::swap(m_v3Target, other.m_v3Target);
	std::swap(m_v3Up, other.m_v3Up);

	std::swap(m_bPerspective, other.m_bPerspective);

	std::swap(m_fFOV, other.m_fFOV);

	std::swap(m_v2Resolution, other.m_v2Resolution);
	std::swap(m_v2NearFar, other.m_v2NearFar);

	std::swap(m_v2Horizontal, other.m_v2Horizontal);
	std::swap(m_v2Vertical, other.m_v2Vertical);

	std::swap(m_m4View, other.m_m4View);
	std::swap(m_m4Projection, other.m_m4Projection);

	std::swap(m_fSpeed, other.m_fSpeed);
	std::swap(m_qOrientation, other.m_qOrientation);
	std::swap(m_v3Forward, other.m_v3Forward);
	std::swap(m_v3Right, other.m_v3Right);
}

Simplex::MyCamera::~MyCamera(void)
{
	Release();
}

void Simplex::MyCamera::ResetCamera(void)
{
	m_v3Position = vector3(0.0f, 0.0f, 10.0f); //Where my camera is located
	m_v3Target = vector3(0.0f, 0.0f, 0.0f); //What I'm looking at
	m_v3Up = vector3(0.0f, 1.0f, 0.0f); //What is up

	m_v3Forward = glm::normalize(m_v3Target - m_v3Position); // Forward vector of the camera
	m_v3Right = glm::cross(m_v3Forward, m_v3Up); // Right vector of the camera
	m_qOrientation = quaternion(m_v3Forward); // Orientation of the camera represented as a quaternion

	m_bPerspective = true; //perspective view? False is Orthographic

	m_fFOV = 45.0f; //Field of View

	m_v2Resolution = vector2(1280.0f, 720.0f); //Resolution of the window
	m_v2NearFar = vector2(0.001f, 1000.0f); //Near and Far planes

	m_v2Horizontal = vector2(-5.0f, 5.0f); //Ortographic horizontal projection
	m_v2Vertical = vector2(-5.0f, 5.0f); //Ortographic vertical projection

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

void Simplex::MyCamera::SetPositionTargetAndUp(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	m_v3Position = a_v3Position;
	m_v3Target = a_v3Target;
	//m_v3Up = a_v3Position + a_v3Upward;
	m_v3Up = a_v3Upward;

	m_v3Forward = glm::normalize(m_v3Target - m_v3Position); // Forward vector of the camera
	m_v3Right = glm::cross(m_v3Forward, m_v3Up); // Right vector of the camera
	m_qOrientation = quaternion(m_v3Forward); // Orientation of the camera represented as a quaternion

	CalculateProjectionMatrix();
}

void Simplex::MyCamera::CalculateViewMatrix(void)
{
	//Calculate the look at
	m_m4View = glm::lookAt(m_v3Position, m_v3Target, m_v3Up);
}

void Simplex::MyCamera::CalculateProjectionMatrix(void)
{
	//perspective
	float fRatio = m_v2Resolution.x / m_v2Resolution.y;
	if (m_bPerspective)
	{
		m_m4Projection = glm::perspective(m_fFOV, fRatio, m_v2NearFar.x, m_v2NearFar.y);
	}
	else
	{
		m_m4Projection = glm::ortho(	m_v2Horizontal.x * fRatio, m_v2Horizontal.y * fRatio, //horizontal
										m_v2Vertical.x, m_v2Vertical.y, //vertical
										m_v2NearFar.x, m_v2NearFar.y); //near and far
	}
}

void Simplex::MyCamera::Rotate(float a_fYaw, float a_fPitch, float a_fRoll)
{
	// Generate quaternions for pitch/yaw/roll using angle axis
	quaternion qPitch = glm::angleAxis(a_fPitch * a_fSens, vector3(1.0f, 0.0f, 0.0f));
	quaternion qYaw = glm::angleAxis(a_fYaw * a_fSens, vector3(0.0f, 1.0f, 0.0f));
	//quaternion qRoll = glm::angleAxis(a_fRoll * a_fSens, vector3(0.0f, 0.0f, 1.0f));

	// For this camera we can ommit roll
	m_qOrientation = glm::cross(qPitch, qYaw);
	m_qOrientation = glm::normalize(m_qOrientation);

	// Set forward vector
	m_v3Forward = glm::rotate(m_qOrientation, glm::normalize(m_v3Target - m_v3Position));

	// Set Up vector
	m_v3Up = m_qOrientation * glm::vec3(0, 1, 0);

	// Set Right vector
	m_v3Right = glm::cross(m_v3Forward, m_v3Up);

	// Set Target vector
	m_v3Target = m_v3Forward + m_v3Position;
}

void Simplex::MyCamera::Move(void)
{
	// Reset Velocity
	vector3 velocity = vector3();

	// Adjust camera velocity depending on user input
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) // W key pressed
	{
		// Move the camera forwards
		velocity += m_v3Forward;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) // S key pressed
	{ 
		// Move the camera backwards
		velocity -= m_v3Forward;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) // A key pressed
	{
		// Move the camera to the left
		velocity -= m_v3Right;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) // D key pressed
	{
		// Move the camera to the right
		velocity += m_v3Right;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) // Q key pressed
	{
		// Move the camera upwards
		velocity += m_v3Up;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) // E key pressed
	{
		// Move the camera downwards
		velocity -= m_v3Up;
	}

	// Check to make sure the velocity isn't zero
	if (velocity != vector3()) 
	{
		// Normalize velocity and multiply it by camera speed
		velocity = glm::normalize(velocity) * m_fSpeed;
	}
	
	// Move the camera position and target based on velocity and time since last update
	SetPosition(GetPosition() + timer.dt * velocity);
	SetTarget(GetTarget() + timer.dt * velocity);
}
