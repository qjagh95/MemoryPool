#pragma once

#define MAX_COUNT 4096

template<typename T>
class MemoryManager
{
private:
	struct MemoryInfo
	{
		T* pMemoryBuffer;		    //처음 정해진 뭉탱이 버퍼
		size_t nCurPointerIndex;    //할당된 객체 갯수(인덱스)
		std::string strClassName;   //클래스 이름
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

	//클래스 타입을 넣어주면 해당 클래스 타입의 메모리 덩어리를 지운다.
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

	//얻어오기
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
			//반납된 포인터가 하나라도 있다면 그 주소를 던져주고 리스트에서 빼준다
			if (getInfo->DeletePointerList.size() != 0)
			{
				auto BeginIter = getInfo->DeletePointerList.begin();

				returnBuffer = (*BeginIter);
				//객체 초기화 해준다.
				memset(returnBuffer, 0, sizeof(T));

				getInfo->DeletePointerList.erase(BeginIter);
			}
			else
			{
				//최대사이즈 초과
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

	//반납
	void DeleteMemory(T** DeletePointer)
	{
		std::string Name = typeid(T).name();
		InsertDeletePointer(Name, *DeletePointer);
		*DeletePointer = nullptr;
	}

	//반납
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