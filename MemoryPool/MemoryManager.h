#pragma once

#define MAX_COUNT 4096

template<typename T>
class MemoryManager
{
private:
	struct MemoryInfo
	{
		T* pMemoryBuffer;		    //ó�� ������ ������ ����
		size_t nCurPointerIndex;    //�Ҵ�� ��ü ����(�ε���)
		std::string strClassName;   //Ŭ���� �̸�
		std::list<T*> DeletePointerList;

		MemoryInfo()
		{
			pMemoryBuffer = nullptr;
			nCurPointerIndex = 0;
		}
		void Delete()
		{
			if (pMemoryBuffer != nullptr)
				delete[] pMemoryBuffer;
		}
	};

	MemoryInfo* FindMemoryInfo(const std::string& ClassName)
	{
		auto FindIter = m_MemoryMap.find(ClassName);

		if (FindIter == m_MemoryMap.end())
			return nullptr;

		return &FindIter->second;
	}

public:
	static MemoryManager* Get()
	{
		if (m_Instance == nullptr)
			m_Instance = new MemoryManager();

		return m_Instance;
	}
	static void Delete()
	{
		if (m_Instance != nullptr)
			delete m_Instance;
	}

	//Ŭ���� Ÿ���� �־��ָ� �ش� Ŭ���� Ÿ���� �޸� ����� �����.
	void ReleaseClassMemoryAndEraseMap()
	{
		std::string Name = typeid(T).name();
		MemoryInfo* getInfo = FindMemoryInfo(Name);

		if (getInfo != nullptr)
		{
			if (getInfo->pMemoryBuffer != nullptr)
				delete[] getInfo->pMemoryBuffer;

			m_MemoryMap.erase(Name);
		}
	}

	//������
	T* GetMemory()
	{
		std::string Name = typeid(T).name();
		MemoryInfo* getInfo = FindMemoryInfo(Name);

		T* returnBuffer = nullptr;

		if (getInfo == nullptr)
		{
			std::string Name = typeid(T).name();
			MemoryInfo newInfo = {};
			newInfo.strClassName = Name;

			newInfo.pMemoryBuffer = new T[MAX_COUNT];
			returnBuffer = newInfo.pMemoryBuffer;
			returnBuffer = &returnBuffer[newInfo.nCurPointerIndex];

			m_MemoryMap.insert(make_pair(Name, newInfo));
		}
		else
		{
			//�ݳ��� �����Ͱ� �ϳ��� �ִٸ� �� �ּҸ� �����ְ� ����Ʈ���� ���ش�
			if (getInfo->DeletePointerList.size() != 0)
			{
				auto BeginIter = getInfo->DeletePointerList.begin();

				returnBuffer = (*BeginIter);
				//��ü �ʱ�ȭ ���ش�.
				memset(returnBuffer, 0, sizeof(T));

				getInfo->DeletePointerList.erase(BeginIter);
			}
			else
			{
				//�ִ������ �ʰ�
				if (getInfo->nCurPointerIndex > MAX_COUNT)
					assert(false);

				else
				{
					returnBuffer = getInfo->pMemoryBuffer;
					returnBuffer = &returnBuffer[getInfo->nCurPointerIndex];
					getInfo->nCurPointerIndex += 1;
				}
			}
		}
		return returnBuffer;
	}

	//�ݳ�
	void DeleteMemory(T** DeletePointer)
	{
		std::string Name = typeid(T).name();
		InsertDeletePointer(Name, *DeletePointer);
		*DeletePointer = nullptr;
	}

	//�ݳ�
	T* DeleteMemory(T* DeletePointer)
	{
		std::string Name = typeid(T).name();
		InsertDeletePointer(Name, DeletePointer);
		return nullptr;
	}

private:
	void InsertDeletePointer(const std::string& ClassName, T* Pointer)
	{
		MemoryInfo* getInfo = FindMemoryInfo(ClassName);

		if (getInfo != nullptr)
			getInfo->DeletePointerList.push_front(Pointer);
	}

private:
	std::unordered_map<std::string, MemoryInfo> m_MemoryMap;

private:
	static MemoryManager* m_Instance;

	MemoryManager() {}
	~MemoryManager()
	{
		auto StartIter = m_MemoryMap.begin();
		auto EndIter = m_MemoryMap.end();

		for (;StartIter != EndIter; StartIter++)
		{
			if(StartIter->second.pMemoryBuffer != nullptr)
				StartIter->second.Delete();
		}
	}
};

template <typename T> 
MemoryManager<T>* MemoryManager<T>::m_Instance = nullptr;