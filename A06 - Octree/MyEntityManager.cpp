#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
Simplex::MyEntityManager* Simplex::MyEntityManager::m_pInstance = nullptr;
void Simplex::MyEntityManager::Init(void)
{
	m_uEntityCount = 0;
	m_mEntityArray = nullptr;
}
void Simplex::MyEntityManager::Release(void)
{
	for (uint uEntity = 0; uEntity < m_uEntityCount; ++uEntity)
	{
		MyEntity* pEntity = m_mEntityArray[uEntity];
		SafeDelete(pEntity);
	}
	m_uEntityCount = 0;
	m_mEntityArray = nullptr;
}
Simplex::MyEntityManager* Simplex::MyEntityManager::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void Simplex::MyEntityManager::ReleaseInstance()
{
	if(m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	//look one by one for the specified unique id
	for (uint uIndex = 0; uIndex < m_uEntityCount; ++uIndex)
	{
		if (a_sUniqueID == m_mEntityArray[uIndex]->GetUniqueID())
			return uIndex;
	}
	//if not found return -1
	return -1;
}
//Accessors
Simplex::uint Simplex::MyEntityManager::GetEntityCount(void) {	return m_uEntityCount; }
Simplex::uint Simplex::MyEntityManager::GetOctantCount(void) { return m_vOctants.size(); }
Simplex::Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return nullptr;

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetModel();
}
Simplex::Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetModel();
	}
	return nullptr;
}
Simplex::MyRigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return nullptr;

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetRigidBody();
}
Simplex::MyRigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetRigidBody();
	}
	return nullptr;
}
Simplex::matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return matrix4();

	// if out of bounds
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetModelMatrix();
}
Simplex::matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->GetModelMatrix();
	}
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->SetModelMatrix(a_m4ToWorld);
	}
}
void Simplex::MyEntityManager::SetAxisVisibility(bool a_bVisibility, uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->SetAxisVisible(a_bVisibility);
}
void Simplex::MyEntityManager::SetAxisVisibility(bool a_bVisibility, String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->SetAxisVisible(a_bVisibility);
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	m_mEntityArray[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
std::vector<Simplex::MyEntityManager::Octant> Simplex::MyEntityManager::GetOctants(void) { return m_vOctants; }
//The big 3
Simplex::MyEntityManager::MyEntityManager(){Init();}
Simplex::MyEntityManager::MyEntityManager(MyEntityManager const& a_pOther){ }
Simplex::MyEntityManager& Simplex::MyEntityManager::operator=(MyEntityManager const& a_pOther) { return *this; }
Simplex::MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	//Clear all collisions
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		m_mEntityArray[i]->ClearCollisionList();
	}

	//check collisions
	for (uint i = 0; i < m_uEntityCount - 1; i++)
	{
		for (uint j = i + 1; j < m_uEntityCount; j++)
		{
			m_mEntityArray[i]->IsColliding(m_mEntityArray[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	//Create a temporal entity to store the object
	MyEntity* pTemp = new MyEntity(a_sFileName, a_sUniqueID);
	//if I was able to generate it add it to the list
	if (pTemp->IsInitialized())
	{
		//create a new temp array with one extra entry
		PEntity* tempArray = new PEntity[m_uEntityCount + 1];
		//start from 0 to the current count
		uint uCount = 0;
		for (uint i = 0; i < m_uEntityCount; ++i)
		{
			tempArray[uCount] = m_mEntityArray[i];
			++uCount;
		}
		tempArray[uCount] = pTemp;
		//if there was an older array delete
		if (m_mEntityArray)
		{
			delete[] m_mEntityArray;
		}
		//make the member pointer the temp pointer
		m_mEntityArray = tempArray;
		//add one entity to the count
		++m_uEntityCount;
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return;

	// if out of bounds choose the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	// if the entity is not the very last we swap it for the last one
	if (a_uIndex != m_uEntityCount - 1)
	{
		std::swap(m_mEntityArray[a_uIndex], m_mEntityArray[m_uEntityCount - 1]);
	}
	
	//and then pop the last one
	//create a new temp array with one less entry
	PEntity* tempArray = new PEntity[m_uEntityCount - 1];
	//start from 0 to the current count
	for (uint i = 0; i < m_uEntityCount - 1; ++i)
	{
		tempArray[i] = m_mEntityArray[i];
	}
	//if there was an older array delete
	if (m_mEntityArray)
	{
		delete[] m_mEntityArray;
	}
	//make the member pointer the temp pointer
	m_mEntityArray = tempArray;
	//add one entity to the count
	--m_uEntityCount;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	int nIndex = GetEntityIndex(a_sUniqueID);
	RemoveEntity((uint)nIndex);
}
Simplex::String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return "";

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->GetUniqueID();
}
Simplex::MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return nullptr;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	//if out of bounds will do it for all
	if (a_uIndex >= m_uEntityCount)
	{
		//add for each one in the entity list
		for (a_uIndex = 0; a_uIndex < m_uEntityCount; ++a_uIndex)
		{
			m_mEntityArray[a_uIndex]->AddToRenderList(a_bRigidBody);
		}
	}
	else //do it for the specified one
	{
		m_mEntityArray[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddDimension(uint a_uIndex, uint a_uDimension)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->AddDimension(a_uDimension);
}
void Simplex::MyEntityManager::AddDimension(String a_sUniqueID, uint a_uDimension)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->AddDimension(a_uDimension);
	}
}
void Simplex::MyEntityManager::RemoveDimension(uint a_uIndex, uint a_uDimension)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->RemoveDimension(a_uDimension);
}
void Simplex::MyEntityManager::RemoveDimension(String a_sUniqueID, uint a_uDimension)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->RemoveDimension(a_uDimension);
	}
}
void Simplex::MyEntityManager::ClearDimensionSetAll(void)
{
	for (uint i = 0; i < m_uEntityCount; ++i)
	{
		ClearDimensionSet(i);
	}
}
void Simplex::MyEntityManager::ClearDimensionSet(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->ClearDimensionSet();
}
void Simplex::MyEntityManager::ClearDimensionSet(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->ClearDimensionSet();
	}
}
bool Simplex::MyEntityManager::IsInDimension(uint a_uIndex, uint a_uDimension)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return false;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->IsInDimension(a_uDimension);
}
bool Simplex::MyEntityManager::IsInDimension(String a_sUniqueID, uint a_uDimension)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->IsInDimension(a_uDimension);
	}
	return false;
}
bool Simplex::MyEntityManager::SharesDimension(uint a_uIndex, MyEntity* const a_pOther)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return false;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	return m_mEntityArray[a_uIndex]->SharesDimension(a_pOther);
}
bool Simplex::MyEntityManager::SharesDimension(String a_sUniqueID, MyEntity* const a_pOther)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		return pTemp->SharesDimension(a_pOther);
	}
	return false;
}
void Simplex::MyEntityManager::UpdateDimensionSetAll(void)
{
	for (uint i = 0; i < m_uEntityCount; ++i)
	{
		UpdateDimensionSet(i);
	}
}
void Simplex::MyEntityManager::UpdateDimensionSet(uint a_uIndex)
{
	//if the list is empty return
	if (m_uEntityCount == 0)
		return;

	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_uEntityCount)
		a_uIndex = m_uEntityCount - 1;

	// Check each end node octant to see if the object is contained within it
	for each (Octant octant in m_vOctants)
	{
		// Check if this is an end node octant
		if (octant.m_bIsEndNode)
		{
			// Check if the object is contained in the octant
			if (ContainedInOctant(octant, GetRigidBody(a_uIndex)))
			{
				// If it is, add this ocant to the dimensions thie entity is contatined in
				m_mEntityArray[a_uIndex]->AddDimension(octant.m_uOctantID);
			}
		}
	}
}
void Simplex::MyEntityManager::UpdateDimensionSet(String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		// Check each end node octant to see if the object is contained within it
		for each (Octant octant in m_vOctants)
		{
			// Check if this is an end node octant
			if (octant.m_bIsEndNode)
			{
				// Check if the object is contained in the octant
				if (ContainedInOctant(octant, pTemp->GetRigidBody()))
				{
					// If it is, add this ocant to the dimensions thie entity is contatined in
					pTemp->AddDimension(octant.m_uOctantID);
				}
			}
		}
	}
}
void Simplex::MyEntityManager::GenerateOctants(uint a_uOctantLevels)
{
	// Clear the current list of octants
	m_vOctants.clear();
	m_uOctantCount = 1;

	// If atleast one octant level is requested calculate first octant information
	if (a_uOctantLevels >= 1)
	{
		// Find the minimum and maximum points in the entity list
		vector3 max = vector3();
		vector3 min = vector3();

		// Check each entity to see if any have a max/min value greater/less-than the current max and min points 
		for (uint i = 0; i < m_uEntityCount; ++i)
		{
			MyEntity* entity = GetEntity(i);
			vector3 eMax = entity->GetRigidBody()->GetMaxGlobal();
			if (eMax.x > max.x)
				max.x = eMax.x;
		
			if (eMax.y > max.y)
				max.y = eMax.y;

			if (eMax.z > max.z)
				max.z = eMax.z;

			vector3 eMin = entity->GetRigidBody()->GetMinGlobal();
			if (eMin.x < min.x)
				min.x = eMin.x;

			if (eMin.y < min.y)
				min.y = eMin.y;

			if (eMin.z < min.z)
				min.z = eMin.z;
		}

		// Set up the first octant
		Octant firstOct = Octant();
		firstOct.m_v3Max = max;
		firstOct.m_v3Min = min;
		firstOct.m_v3Center = max - ((max - min) / 2.0f);
		firstOct.m_uOctantLevel = 1;
		firstOct.m_uOctantID = m_uOctantCount;
		m_uOctantCount++;
		if (a_uOctantLevels == 1)
			firstOct.m_bIsEndNode = true;
		else
			firstOct.m_bIsEndNode = false;

		// Add the first octant to the octants list
		m_vOctants.push_back(firstOct);

		// Subdivide down from the first octant for each additional octant level requested
		for (int i = 1; i <= a_uOctantLevels; ++i)
		{
			// Iterate through all current octants
			uint octantCount = m_vOctants.size();
			for (uint j = 0; j < octantCount; ++j)
			{
				// If the octant is of the current level we are checking
				if (m_vOctants[j].m_uOctantLevel == i)
				{
					// Check to see if the octant contains any objects
					bool containsObject = false;
					for (uint k = 0; k < m_uEntityCount; ++k)
					{
						if (ContainedInOctant(m_vOctants[j], GetRigidBody(k)))
						{
							containsObject = true;
							break;
						}
					}

					// If the octant contains no objects it is an end node and should be noted as such and no child octants should be generated
					if (!containsObject)
					{
						m_vOctants[j].m_bIsEndNode = true;
					}
					else
					{
						// Check if this is the last depth level for the octants
						if (m_vOctants[j].m_uOctantLevel == a_uOctantLevels)
						{
							// If it is set the octant as an end node
							m_vOctants[j].m_bIsEndNode = true;
						}
						else
						{
							// If it isn't Generate child octants
							GenerateChildOctant(m_vOctants[j]);
						}
					}
				}
			}
		}
	}
}
void Simplex::MyEntityManager::GenerateChildOctant(Octant a_oParent)
{
	vector3 max = a_oParent.m_v3Max;
	vector3 min = a_oParent.m_v3Min;
	vector3 center = a_oParent.m_v3Center;

	std::vector<Octant> childOctants = std::vector<Octant>();

	// Top Left Front Child Octant
	childOctants.push_back(Octant());
	childOctants[0].m_v3Max = vector3(center.x, center.y, max.z);
	childOctants[0].m_v3Min = vector3(min.x, min.y, center.z);

	// Top Right Front Child Octant
	childOctants.push_back(Octant());
	childOctants[1].m_v3Max = vector3(max.x, center.y, max.z);
	childOctants[1].m_v3Min = vector3(center.x, min.y, center.z);

	// Top Left Back Child Octant
	childOctants.push_back(Octant());
	childOctants[2].m_v3Max = vector3(center.x, max.y, max.z);
	childOctants[2].m_v3Min = vector3(min.x, center.y, center.z);

	// Top Right Back Child Octant
	childOctants.push_back(Octant());
	childOctants[3].m_v3Max = max;
	childOctants[3].m_v3Min = center;

	// Bottom Left Front Child Octant
	childOctants.push_back(Octant());
	childOctants[4].m_v3Max = center;
	childOctants[4].m_v3Min = min;

	// Bottom Right Front Child Octant
	childOctants.push_back(Octant());
	childOctants[5].m_v3Max = vector3(max.x, center.y, center.z);
	childOctants[5].m_v3Min = vector3(center.x, min.y, min.z);

	// Bottom Left Back Child Octant
	childOctants.push_back(Octant());
	childOctants[6].m_v3Max = vector3(center.x, max.y, center.z);
	childOctants[6].m_v3Min = vector3(min.x, center.y, min.z);

	// Bottom Right Back Child Octant
	childOctants.push_back(Octant());
	childOctants[7].m_v3Max = vector3(max.x, max.y, center.z);
	childOctants[7].m_v3Min = vector3(center.x, center.y, min.z);

	// Fill out additional octant member variables and add them to the main octants list
	for each(Octant octant in childOctants)
	{
		octant.m_v3Center = octant.m_v3Max - ((octant.m_v3Max - octant.m_v3Min) / 2.0f);
		octant.m_uOctantLevel = a_oParent.m_uOctantLevel + 1;
		octant.m_uOctantID = m_uOctantCount;
		m_uOctantCount++;
		octant.m_bIsEndNode = false;

		// Add the child octants to the main octants list
		m_vOctants.push_back(octant);
	}
}
bool Simplex::MyEntityManager::ContainedInOctant(Octant a_octant, MyRigidBody* a_rigidBody)
{
	bool bColliding = true;

	// Check for sphere collision
	bColliding = (glm::distance(a_octant.m_v3Center, a_rigidBody->GetCenterGlobal()) < glm::distance(a_octant.m_v3Max, a_octant.m_v3Center) + a_rigidBody->GetRadius());

	// If they are sphere colliding check the Axis Aligned Bounding Box
	if (bColliding) //they are colliding with bounding sphere
	{
		if (a_octant.m_v3Max.x < a_rigidBody->GetMinGlobal().x) //this to the right of other
			bColliding = false;
		if (a_octant.m_v3Min.x > a_rigidBody->GetMaxGlobal().x) //this to the left of other
			bColliding = false;

		if (a_octant.m_v3Max.y < a_rigidBody->GetMinGlobal().y) //this below of other
			bColliding = false;
		if (a_octant.m_v3Min.y > a_rigidBody->GetMaxGlobal().y) //this above of other
			bColliding = false;

		if (a_octant.m_v3Max.z < a_rigidBody->GetMinGlobal().z) //this behind of other
			bColliding = false;
		if (a_octant.m_v3Min.z > a_rigidBody->GetMaxGlobal().z) //this in front of other
			bColliding = false;

		if (bColliding) //they are colliding with bounding box also
		{
			return true;
		}
	}

	return false;
}
void Simplex::MyEntityManager::DisplayOctree(MeshManager* a_pMeshManager, uint a_uOctantToDisplay)
{
	// Set the octant wireframe display color
	vector3 color = vector3(0, 200, 0); // Green

	// If the octant to display is greater than the number of octants display all octants
	if (a_uOctantToDisplay >= m_vOctants.size())
	{
		for each (MyEntityManager::Octant octant in m_vOctants)
		{
			matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(octant.m_v3Max.x - octant.m_v3Min.x, octant.m_v3Max.y - octant.m_v3Min.y, octant.m_v3Max.z - octant.m_v3Min.z));
			matrix4 m4Translate = glm::translate(IDENTITY_M4, octant.m_v3Center);
			matrix4 m4Model = m4Translate * m4Scale;
			a_pMeshManager->AddWireCubeToRenderList(m4Model, color);
		}

	}
	else
	{
		// Otherwise just display the requested octant
		matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(m_vOctants[a_uOctantToDisplay].m_v3Max.x - m_vOctants[a_uOctantToDisplay].m_v3Min.x, m_vOctants[a_uOctantToDisplay].m_v3Max.y - m_vOctants[a_uOctantToDisplay].m_v3Min.y, m_vOctants[a_uOctantToDisplay].m_v3Max.z - m_vOctants[a_uOctantToDisplay].m_v3Min.z));
		matrix4 m4Translate = glm::translate(IDENTITY_M4, m_vOctants[a_uOctantToDisplay].m_v3Center);
		matrix4 m4Model = m4Translate * m4Scale;
		a_pMeshManager->AddWireCubeToRenderList(m4Model, color);
	}
}
void Simplex::MyEntityManager::UpdateOctantsAndDimensions(uint a_uOctantLevels)
{
	// Clear current octant associations
	ClearDimensionSetAll();

	// Regenerate Octants
	GenerateOctants(a_uOctantLevels);

	// Regenerate octant associations
	UpdateDimensionSetAll();
}