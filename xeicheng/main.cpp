#include"coroutine.h"
#include<iostream>

void fun1()
{
	std::cout << "fun1-1" << std::endl;
	coroutine_control::yeild();
	std::cout << "fun1-2" << std::endl;
	coroutine_control::yeild();
	std::cout << "fun1-3" << std::endl;
}

void fun2()
{
	std::cout << "fun2-1" << std::endl;
	coroutine_control::yeild();
	std::cout << "fun2-2" << std::endl;
	coroutine_control::yeild();
	std::cout << "fun2-3" << std::endl;
}

void fun3()
{
	std::cout << "fun3-1" << std::endl;
	coroutine_control::yeild();
	std::cout << "fun3-2" << std::endl;
	coroutine_control::yeild();
	std::cout << "fun3-3" << std::endl;
}

int main()
{
	coroutine_control control;
	fun3();
	coroutine_id t1 = control.create(fun1);
	coroutine_id t2 = control.create(fun2);
	coroutine_id t3 = control.create(fun1);
	control.join();
	//std::cout << "111" << std::endl;
	//control.resume(t1);
	//std::cout << "222" << std::endl;
	//control.resume(t1);
	//std::cout << "333" << std::endl;
	//control.resume(t1);
	//coroutine_id t2 = control.create(fun2);
	//std::cout << "444" << std::endl;
	//control.resume(t2);
	//std::cout << "555" << std::endl;
	//control.resume(t2);
	//std::cout << "666" << std::endl;
	//control.resume(t2);

	return 0;
}