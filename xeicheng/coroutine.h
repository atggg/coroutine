#pragma once
#define STACK_LEN 10240 //ջ�Ĵ�С x *void
#include<thread>
#include<functional>
#include<map>
#include<list>
#include<queue>
#include<mutex>

class coroutine_control;
typedef size_t coroutine_id;
using coroutine_function = std::function<void()>;
#ifdef _WIN32
#ifdef _WIN64
#define STACK_SEAT uint64_t, uint64_t, uint64_t, uint64_t,
struct context
{
	context();
	void* rax; //0
	void* rbx; //8
	void* rcx; //10
	void* rdx; //18
	void* rsi; //20
	void* rdi; //28
	void* r8; //30
	void* r9; //38
	void* r10; //40
	void* r11; //48
	void* r12; //50
	void* r13; //58
	void* r14; //60
	void* r15; //68
	void* rsp; //70
	void* rbp; //78
	void* rip; //80
};
#else
#define STACK_SEAT
struct context
{
	context();
	void* eax;//0
	void* ebx;//4
	void* ecx;//8
	void* edx;//C
	void* esi;//10
	void* edi;//14
	void* esp;//18
	void* ebp;//1C
	void* eip;//20
};
#endif // _WIN64
#endif // _WIN32
#ifdef linux
#define STACK_SEAT uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t,
struct context
{
	context();
	void* rax; //0
	void* rbx; //8
	void* rcx; //10
	void* rdx; //18
	void* rsi; //20
	void* rdi; //28
	void* r8; //30
	void* r9; //38
	void* r10; //40
	void* r11; //48
	void* r12; //50
	void* r13; //58
	void* r14; //60
	void* r15; //68
	void* rsp; //70
	void* rbp; //78
	void* rip; //80
};
#endif // linux


class coroutine
{
public:
	enum state
	{
		state_ready,
		state_end,
	};
public:
	friend class coroutine_control;
public:
	coroutine(coroutine_function f, coroutine_id id, coroutine_control* control);
	~coroutine();

private:
#ifdef _WIN32
	static void __cdecl start(coroutine* cor);
	static void __cdecl end(STACK_SEAT coroutine* cor);
#endif // _WIN32
#ifdef linux
	static void __attribute__((__cdecl__)) start(coroutine* cor);
	static void __attribute__((__cdecl__)) end(STACK_SEAT coroutine* cor);
#endif // LINUX



private:
	//����
	coroutine_function _f;
	//��Я�̵Ŀ�����
	coroutine_control* _control;
	//id
	coroutine_id _id;
	//״̬
	state _state;
	//ջ
	void** _stack;
	//ջ�Ĵ�С
	size_t _stack_len;
	//������
	context* _reg;
};

class coroutine_control
{
public:
	friend class coroutine;
public:
	coroutine_control();
	~coroutine_control();
	coroutine_id create(coroutine_function f);
	void free(coroutine_id id);
	static void yeild();
	void resume(coroutine_id id);
	void join();
private:
	//Я���б�
	std::map<coroutine_id, coroutine*> _coroutine_list;
	//Я�̶���
	std::queue<coroutine_id> _coroutine_queue;
	//��ǰ���е�Я��
	coroutine* _cur_coroutine;
	//��������������
	context _control_context;
	//�߳�id
	std::thread::id _thid;
	//������
	static std::mutex _mtx;
	//��������
	static std::map<std::thread::id, coroutine_control*> _coroutine_control_list;
};

