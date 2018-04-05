#pragma once
#include "MyEntity.h"

namespace Simplex
{

	class MyEntityManager
	{
	public:
		static MyEntityManager* getInstance();
		void deleteInstance();

		MyEntity* GetEntity(String name);
		void AddEntity(String a_sFileName, String a_sUniqueID = "NA");
		void DeleteEntity(String name);

	private:
		MyEntityManager();
		~MyEntityManager();
		MyEntityManager& operator=(MyEntityManager const& other);

		static MyEntityManager* instance;
		std::vector<MyEntity*> m_EntityList;
	};

}