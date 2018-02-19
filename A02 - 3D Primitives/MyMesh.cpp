#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// My Code
	// Bottom Center
	vector3 pointBottom(0, -a_fHeight / 2, 0);

	// Top Center
	vector3 pointTop(0, a_fHeight / 2, 0); 

	// Generate vertices of the base
	std::vector<vector3> baseVerts = std::vector<vector3>();
	for (int i = 0; i < a_nSubdivisions; i++) {
		float angle = (PI * 2) * ((float)i / (float)a_nSubdivisions);
		float sine = sin(angle);
		float cosine = cos(angle);
		baseVerts.push_back(vector3(cosine * a_fRadius, -a_fHeight / 2, sine * a_fRadius));
	}

	// Generate triangle
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		// Base Triangle
		AddTri(baseVerts[i], baseVerts[i + 1], pointBottom);

		// Top Triangle
		AddTri(baseVerts[i + 1], baseVerts[i], pointTop);
	}

	// Add final triangles connecting the beggining and ending points
	// Base Triangle
	AddTri(baseVerts[a_nSubdivisions - 1], baseVerts[0], pointBottom);

	// Top Triangle
	AddTri(baseVerts[0], baseVerts[a_nSubdivisions - 1], pointTop);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// My Code
	// Bottom Center
	vector3 pointBottom(0, -a_fHeight / 2, 0);

	// Top Center
	vector3 pointTop(0, a_fHeight / 2, 0);

	// Generate vertices of the base
	std::vector<vector3> baseVerts = std::vector<vector3>();
	std::vector<vector3> topVerts = std::vector<vector3>();
	for (int i = 0; i < a_nSubdivisions; i++) {
		float angle = (PI * 2) * ((float)i / (float)a_nSubdivisions);
		float sine = sin(angle);
		float cosine = cos(angle);
		baseVerts.push_back(vector3(cosine * a_fRadius, -a_fHeight / 2, sine * a_fRadius));
		topVerts.push_back(vector3(cosine * a_fRadius, a_fHeight / 2, sine * a_fRadius));
	}

	// Generate the shape
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		// Base Triangle
		AddTri(baseVerts[i], baseVerts[i + 1], pointBottom);

		// Top Triangle
		AddTri(topVerts[i + 1], topVerts[i], pointTop);

		// Side Quad
		AddQuad(baseVerts[i + 1], baseVerts[i], topVerts[i + 1], topVerts[i]);
	}

	// Add final triangles connecting the beggining and ending points
	// Base Triangle
	AddTri(baseVerts[a_nSubdivisions - 1], baseVerts[0], pointBottom);

	// Top Triangle
	AddTri(topVerts[0], topVerts[a_nSubdivisions - 1], pointTop);

	// Add final side quad
	AddQuad(baseVerts[0], baseVerts[a_nSubdivisions - 1], topVerts[0], topVerts[a_nSubdivisions - 1]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// My Code
	// Generate vertices of the outside and inside bases
	std::vector<vector3> baseOutsideVerts = std::vector<vector3>();
	std::vector<vector3> topOutsideVerts = std::vector<vector3>();
	std::vector<vector3> baseInsideVerts = std::vector<vector3>();
	std::vector<vector3> topInsideVerts = std::vector<vector3>(); 
	for (int i = 0; i < a_nSubdivisions; i++) {
		float angle = (PI * 2) * ((float)i / (float)a_nSubdivisions);
		float sine = sin(angle);
		float cosine = cos(angle);

		// Add Outside vertices
		baseOutsideVerts.push_back(vector3(cosine * a_fOuterRadius, -a_fHeight / 2, sine * a_fOuterRadius));
		topOutsideVerts.push_back(vector3(cosine * a_fOuterRadius, a_fHeight / 2, sine * a_fOuterRadius));

		// Add Inside vertices
		baseInsideVerts.push_back(vector3(cosine * a_fInnerRadius, -a_fHeight / 2, sine * a_fInnerRadius));
		topInsideVerts.push_back(vector3(cosine * a_fInnerRadius, a_fHeight / 2, sine * a_fInnerRadius));
	}

	// Generate the shape
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		// Base Quad
		AddQuad(baseInsideVerts[i + 1], baseInsideVerts[i], baseOutsideVerts[i + 1], baseOutsideVerts[i]);

		// Top Quad
		AddQuad(topOutsideVerts[i + 1], topOutsideVerts[i], topInsideVerts[i + 1], topInsideVerts[i]);

		// Side Quad Outside 
		AddQuad(baseOutsideVerts[i + 1], baseOutsideVerts[i], topOutsideVerts[i + 1], topOutsideVerts[i]);

		// Side Quad Inside
		AddQuad(topInsideVerts[i + 1], topInsideVerts[i], baseInsideVerts[i + 1], baseInsideVerts[i]);
	}

	// Add final quads connecting the beggining and ending points
	// Base Quad
	AddQuad(baseInsideVerts[0], baseInsideVerts[a_nSubdivisions - 1], baseOutsideVerts[0], baseOutsideVerts[a_nSubdivisions - 1]);

	// Top Quad
	AddQuad(topOutsideVerts[0], topOutsideVerts[a_nSubdivisions - 1], topInsideVerts[0], topInsideVerts[a_nSubdivisions - 1]);

	// Side Quad Outside 
	AddQuad(baseOutsideVerts[0], baseOutsideVerts[a_nSubdivisions - 1], topOutsideVerts[0], topOutsideVerts[a_nSubdivisions - 1]);

	// Side Quad Inside
	AddQuad(topInsideVerts[0], topInsideVerts[a_nSubdivisions - 1], baseInsideVerts[0], baseInsideVerts[a_nSubdivisions - 1]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// My Code
	//// Testing Taurus V1 - Does not make use of the passed in number of horizontal subdivisions (a_nSubdivisionsA)
	//// Radius is equal to half the distance between the inner and outer edges
	//float radius = (a_fOuterRadius - a_fInnerRadius) / 2;

	//std::vector<vector3> bottomVerts = std::vector<vector3>();
	//std::vector<vector3> topVerts = std::vector<vector3>();
	//std::vector<vector3> insideVerts = std::vector<vector3>();
	//std::vector<vector3> outsideVerts = std::vector<vector3>();
	//for (int i = 0; i < a_nSubdivisionsB; i++) {
	//	float angle = (PI * 2) * ((float)i / (float)a_nSubdivisionsB);
	//	float sine = sin(angle);
	//	float cosine = cos(angle);

	//	// Add Outside vertices
	//	bottomVerts.push_back(vector3(cosine * (a_fInnerRadius + radius), -radius, sine * (a_fInnerRadius + radius)));
	//	topVerts.push_back(vector3(cosine * (a_fInnerRadius + radius), radius, sine * (a_fInnerRadius + radius)));

	//	// Add Inside vertices
	//	insideVerts.push_back(vector3(cosine * a_fInnerRadius, 0, sine * a_fInnerRadius));
	//	outsideVerts.push_back(vector3(cosine * a_fOuterRadius, 0, sine * a_fOuterRadius));
	//}

	//// Generate Geometry
	//for (int i = 0; i < a_nSubdivisionsB - 1; i++) {
	//	// Bottom - Inside
	//	AddQuad(insideVerts[i + 1], insideVerts[i], bottomVerts[i + 1], bottomVerts[i]);
	//	// Bottom - Outside
	//	AddQuad(bottomVerts[i + 1], bottomVerts[i], outsideVerts[i + 1], outsideVerts[i]);
	//	// Top - Inside
	//	AddQuad(topVerts[i + 1], topVerts[i], insideVerts[i + 1], insideVerts[i]);
	//	// Top - Outside
	//	AddQuad(outsideVerts[i + 1], outsideVerts[i], topVerts[i + 1], topVerts[i]);
	//}

	//// Add final quads connecting the beggining and ending points
	//// Bottom - Inside
	//AddQuad(insideVerts[0], insideVerts[a_nSubdivisionsB - 1], bottomVerts[0], bottomVerts[a_nSubdivisionsB - 1]);
	//// Bottom - Outside
	//AddQuad(bottomVerts[0], bottomVerts[a_nSubdivisionsB - 1], outsideVerts[0], outsideVerts[a_nSubdivisionsB - 1]);
	//// Top - Inside
	//AddQuad(topVerts[0], topVerts[a_nSubdivisionsB - 1], insideVerts[0], insideVerts[a_nSubdivisionsB - 1]);
	//// Top - Outside
	//AddQuad(outsideVerts[0], outsideVerts[a_nSubdivisionsB - 1], topVerts[0], topVerts[a_nSubdivisionsB - 1]);


	// Testing Taurus V1 - Makes use of the passed in number of horizontal subdivisions (a_nSubdivisionsA)
	for (int x = 0; x < a_nSubdivisionsA; x++)
	{
		float phi1 = PI * ((float)(x) / a_nSubdivisionsA);
		float phi2 = PI * ((float)(x + 1) / a_nSubdivisionsA);

		for (int y = 0; y < a_nSubdivisionsB; y++)
		{
			float theta1 = (PI * 2) * ((float)(y) / a_nSubdivisionsB);
			float theta2 = (PI * 2) * ((float)(y + 1) / a_nSubdivisionsB);

			//theta2  theta1
			//	 |      |
			//   2------1 -- phi1
			//   |\ _   |
			//   |    \ |
			//   3------4 -- phi2

			vector3 vertex1 = vector3(sin(phi1) * cos(theta1), sin(phi1) * sin(theta1), cos(phi1)); //vertex on a sphere of radius a_fRadius at spherical coords theta1, phi1
			vector3 vertex2 = vector3(sin(phi2) * cos(theta1), sin(phi2) * sin(theta1), cos(phi2)); //vertex on a sphere of radius a_fRadius at spherical coords theta1, phi2
			vector3 vertex3 = vector3(sin(phi2) * cos(theta2), sin(phi2) * sin(theta2), cos(phi2)); //vertex on a sphere of radius a_fRadius at spherical coords theta2, phi2
			vector3 vertex4 = vector3(sin(phi1) * cos(theta2), sin(phi1) * sin(theta2), cos(phi1)); //vertex on a sphere of radius a_fRadius at spherical coords theta2, phi1

			// Middle of the sphere
			AddQuad(vertex3, vertex4, vertex2, vertex1);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// My Code
	//a_nSubdivisions = 10;  // USED FOR TESTING - COMMENT ME OUT BEFORE SUBMITTING
	int cuts = a_nSubdivisions; // The number of horizontal cuts
	int slices = a_nSubdivisions; // The number of vertical cuts

	for (int x = 0; x < cuts; x++)
	{
		float phi1 = PI * ((float)(x) / cuts);
		float phi2 = PI * ((float)(x + 1) / cuts);

		for (int y = 0; y < slices; y++)
		{
			float theta1 = (PI * 2) * ((float)(y) / slices);
			float theta2 = (PI * 2) * ((float)(y + 1) / slices);

			//theta2  theta1
			//	 |      |
			//   2------1 -- phi1
			//   |\ _   |
			//   |    \ |
			//   3------4 -- phi2

			vector3 vertex1 = vector3(sin(phi1) * cos(theta1), sin(phi1) * sin(theta1), cos(phi1)) * a_fRadius; //vertex on a sphere of radius a_fRadius at spherical coords theta1, phi1
			vector3 vertex2 = vector3(sin(phi2) * cos(theta1), sin(phi2) * sin(theta1), cos(phi2)) * a_fRadius; //vertex on a sphere of radius a_fRadius at spherical coords theta1, phi2
			vector3 vertex3 = vector3(sin(phi2) * cos(theta2), sin(phi2) * sin(theta2), cos(phi2)) * a_fRadius; //vertex on a sphere of radius a_fRadius at spherical coords theta2, phi2
			vector3 vertex4 = vector3(sin(phi1) * cos(theta2), sin(phi1) * sin(theta2), cos(phi1)) * a_fRadius; //vertex on a sphere of radius a_fRadius at spherical coords theta2, phi1

			// Generate the Sphere
			if (x == 0)
			{
				// Top of the sphere
				AddTri(vertex1, vertex2, vertex3); 
			}
			else if (x + 1 == cuts)
			{
				// Bottom of the sphere
				AddTri(vertex1, vertex2, vertex4); 
			}
			else
			{
				// Middle of the sphere
				AddQuad(vertex3, vertex4, vertex2, vertex1);
			}
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}