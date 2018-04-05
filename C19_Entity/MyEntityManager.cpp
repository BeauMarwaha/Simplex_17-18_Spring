#include "MyEntity.h"
#include "MyEntityManager.h"
using namespace Simplex;

Simplex::MyEntityManager::MyEntityManager()
{
	m_EntityList = std::vector<MyEntity*>();
}

Simplex::MyEntityManager::~MyEntityManager() {}
MyEntityManager& Simplex::MyEntityManager::operator=(MyEntityManager const& other) {}

MyEntityManager* Simplex::MyEntityManager::getInstance()
{
	if (instance != nullptr)
	{
		return instance;
	}

	return instance = new MyEntityManager();
}

void Simplex::MyEntityManager::deleteInstance()
{
	delete instance;
	//delete[] m_EntityList;
}

MyEntity* Simplex::MyEntityManager::GetEntity(String name)
{
	if (m_EntityList.size() == 0)
	{
		return nullptr;
	}

	MyEntity* item = m_EntityList[0]->m_IDMap.find(name)->second;

	for (int i = 0; i < m_EntityList.size(); i++)
	{
		if (item == m_EntityList[i])
		{
			m_EntityList.erase(m_EntityList.begin() + i);
		}
	}
}

void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID = "NA")
{
	m_EntityList.push_back(new MyEntity(a_sFileName, a_sUniqueID));
}

void Simplex::MyEntityManager::DeleteEntity(String name)
{
	if (m_EntityList.size() == 0) 
	{
		return;
	}

	MyEntity* item = m_EntityList[0]->m_IDMap.find(name)->second;

	for (int i = 0; i < m_EntityList.size(); i++)
	{
		if (item == m_EntityList[i])
		{
			m_EntityList.erase(m_EntityList.begin() + i);
		}
	}
	delete item;
}

