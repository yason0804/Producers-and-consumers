#include <Windows.h>
#include <process.h>
#include <iostream>
#include <queue>
#include <mutex>

using namespace std;

#define random(a,b) (rand()%(b-a+1)+a)

//创建一个互斥体
mutex object;

//创建生产者和消费者线程处理函数
void ProducerFunc(void *para);
void ConsumerFunc(void *para);

queue<int> Store;
int Store_num = 1;//设置一个缓冲区

int data = 0;
int id = 1001-2;
//int array1[10] = { 1, 1, 2, 1, 3, 2, 5, 8, 5, 9 };
//int array2[10] = { 2, 1, 3, 2, 4, 7, 8, 5, 6, 9 };

HANDLE ProducerEvent = INVALID_HANDLE_VALUE;
HANDLE ConsumerEvent = INVALID_HANDLE_VALUE;

int main()
{

	ProducerEvent = CreateEvent(NULL, true, true, L"生产者");
	ConsumerEvent = CreateEvent(NULL, true, false, L"消费者");

	uintptr_t ProducerThread = _beginthread(ProducerFunc, 0, NULL);
	uintptr_t ConsumerThread = _beginthread(ConsumerFunc, 0, NULL);


	HANDLE threadhand_array[] = { (HANDLE)ProducerThread, (HANDLE)ConsumerThread };

	WaitForMultipleObjects(2, threadhand_array, true, INFINITE);

	return 0;
}

void ProducerFunc(void *para)
{
	while (true)
	{
		WaitForSingleObject(ProducerEvent, INFINITE);
		lock_guard<mutex> add_lock(object);

		if (Store.size() < Store_num)
		{
			
			id = id+2;

			Store.push(id);

			cout << "生产者写入数据：" << id << endl;
			Sleep((random(1, 10) % 10) * 100);
			

		}
		else
		{
			cout << "缓冲区 已经满了！！！" << endl;
			ResetEvent(ProducerEvent);
		}
		if (Store.size() > 0)
		{
			SetEvent(ConsumerEvent);
		}

	}

}
void ConsumerFunc(void *para)
{
	while (true)
	{
		WaitForSingleObject(ConsumerEvent, INFINITE);
		lock_guard<mutex> add_lock(object);
	
		if (Store.size() > 0)
		{
			int data = Store.front();
			Store.pop();
			cout << "                               消费者读取数据：" << data << endl;

			Sleep((random(1,10) % 10) * 1000);
		}
		else
		{
			cout << "                               缓冲区 已经空了..." << endl;
			ResetEvent(ConsumerEvent);
		}
		if (Store.size() < 3)
		{
			SetEvent(ProducerEvent);
		}
		
	}
}