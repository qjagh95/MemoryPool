#include "stdafx.h"
#include "MemoryManager.h"
#include <crtdbg.h>

class Test 
{
public:
	Test() {}
	~Test() {}

	int a = 1;
	int b = 1;
};

int main()
{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);

	Test* newTest1[MAX_COUNT] = {};

	for (int i = 0; i < MAX_COUNT; i++)
	{
		newTest1[i] = MemoryManager<Test>::Get()->GetMemory();
		newTest1[i]->a = i;
		newTest1[i]->b = i;
	}

	for (int i = 0; i < MAX_COUNT; i++)
		cout << newTest1[i]->a <<" "<< newTest1[i]->b <<" "<< endl;

	MemoryManager<Test>::Get()->DeleteMemory(newTest1[30]);
	MemoryManager<Test>::Get()->DeleteMemory(newTest1[29]);
	newTest1[29] = MemoryManager<Test>::Get()->GetMemory();
	newTest1[30] = MemoryManager<Test>::Get()->GetMemory();

	//이 함수는 템플릿 타입의 형식의 내부에서 생성된 메모리를 전부 지운다
	//MemoryManager<Test>::Get()->ReleaseClassMemoryAndEraseMap();

	//아무 클래스타입 넣어주면 모든 타입의 내부 메모리 삭제(싱글톤 객체 제거)
	MemoryManager<Test>::Delete();
}

