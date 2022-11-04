#include "coroutine.h"
#ifdef _WIN32
#ifdef _WIN64
context::context()
{
	rax = 0;
	rbx = 0;
	rcx = 0;
	rdx = 0;
	rsi = 0;
	rdi = 0;
	r8  = 0;
	r9  = 0;
	r10 = 0;
	r11 = 0;
	r12 = 0;
	r13 = 0;
	r14 = 0;
	r15 = 0;
	rsp = 0;
	rbp = 0;
	rip = 0;
}
//s 保存当前现场  t 跳转到的现场
extern "C" void __cdecl jmps(context * s, context * t);
#else
context::context()
{
	eax = 0;
	ebx = 0;
	ecx = 0;
	edx = 0;
	esi = 0;
	edi = 0;
	esp = 0;
	ebp = 0;
	eip = 0;
}

//s 保存当前现场  t 跳转到的现场
void __declspec(naked) jmps(context* s, context* t)
{
	__asm
	{
		//保存现场 也就是保存上下文
		push ebp;
		mov ebp, esp;
		push eax;
		mov eax, dword ptr ds : [ebp + 0x8] ;
		mov dword ptr ds : [eax + 0x4] , ebx;
		mov dword ptr ds : [eax + 0x8] , ecx;
		mov dword ptr ds : [eax + 0xC] , edx;
		mov dword ptr ds : [eax + 0x10] , esi;
		mov dword ptr ds : [eax + 0x14] , edi;
		mov ecx, esp;
		add ecx, 0xC;
		mov dword ptr ds : [eax + 0x18] , ecx;
		mov ecx, eax;
		pop eax;
		mov dword ptr ds : [ecx] , eax;
		pop ebp;
		mov dword ptr ds : [ecx + 0x1C] , ebp;
		mov eax, dword ptr ds : [esp] ;
		mov dword ptr ds : [ecx + 0x20] , eax;


		//加载现场 使用ret改变eip
		push ebp;
		mov ebp, esp;
		mov eax, dword ptr ds : [ebp + 0xC] ;
		mov ebx, dword ptr ds : [eax + 0x4] ;
		mov ecx, dword ptr ds : [eax + 0x8] ;
		mov edx, dword ptr ds : [eax + 0xC] ;
		mov esi, dword ptr ds : [eax + 0x10] ;
		mov edi, dword ptr ds : [eax + 0x14] ;
		mov esp, dword ptr ds : [eax + 0x18] ;
		mov ebp, dword ptr ds : [eax + 0x1C] ;
		push dword ptr ds : [eax + 0x20] ;
		mov eax, dword ptr ds : [eax] ;
		ret;
	}
}
#endif // WIN64
#endif // _WIN32

#ifdef linux
//日了狗了 我的Linux没有ucontext相关函数 只能用汇编了
context::context()
{
	rax = 0;
	rbx = 0;
	rcx = 0;
	rdx = 0;
	rsi = 0;
	rdi = 0;
	r8 = 0;
	r9 = 0;
	r10 = 0;
	r11 = 0;
	r12 = 0;
	r13 = 0;
	r14 = 0;
	r15 = 0;
	rsp = 0;
	rbp = 0;
	rip = 0;
}
void __attribute__((__naked__)) jmps(context* s, context* t)
{
	asm("\
		movq %rax,(%rdi)\n\
		movq %rbx, +0x8(%rdi)\n\
		movq %rcx, +0x10(%rdi)\n\
		movq %rdx, +0x18(%rdi)\n\
		movq %rsi, +0x20(%rdi)\n\
		movq %rdi, +0x28(%rdi)\n\
		movq %r8, +0x30(%rdi)\n\
		movq %r9, +0x38(%rdi)\n\
		movq %r10, +0x40(%rdi)\n\
		movq %r11, +0x48(%rdi)\n\
		movq %r12, +0x50(%rdi)\n\
		movq %r13, +0x58(%rdi)\n\
		movq %r14, +0x60(%rdi)\n\
		movq %r15, +0x68(%rdi)\n\
		addq $0x8, %rsp\n\
		movq %rsp, +0x70(%rdi)\n\
		subq $0x8, %rsp\n\
		movq (%rsp), %rax\n\
		movq %rbp, +0x78(%rdi)\n\
		movq %rax, +0x80(%rdi)\n\
		nop\n\
		nop\n\
		movq (%rsi), %rax\n\
		movq +0x8(%rsi), %rbx\n\
		movq +0x10(%rsi), %rcx\n\
		movq +0x18(%rsi), %rdx\n\
		movq + 0x28(%rsi), %rdi\n\
		movq + 0x30(%rsi), %r8\n\
		movq + 0x38(%rsi), %r9\n\
		movq + 0x40(%rsi), %r10\n\
		movq + 0x48(%rsi), %r11\n\
		movq + 0x50(%rsi), %r12\n\
		movq + 0x58(%rsi), %r13\n\
		movq + 0x60(%rsi), %r14\n\
		movq + 0x68(%rsi), %r15\n\
		movq + 0x70(%rsi), %rsp\n\
		movq + 0x78(%rsi), %rbp\n\
		pushq $0\n\
		pushq %rax\n\
		movq + 0x80(%rsi), %rax\n\
		movq % rax, +0x8(%rsp)\n\
		movq + 0x20(%rsi), %rsi\n\
		popq % rax\n\
		ret\n\
	");
}
#endif // linux


std::mutex coroutine_control::_mtx;
std::map<std::thread::id, coroutine_control*> coroutine_control::_coroutine_control_list;

coroutine::coroutine(coroutine_function f, coroutine_id id, coroutine_control* control)
{
	_f = f;
	_stack_len = STACK_LEN*sizeof(void*);
	_stack = new void*[STACK_LEN];
	_reg = new context;
	_control = control;
	_id = id;
	_state = state::state_ready;
#ifdef _WIN32
#ifdef _WIN64
	_reg->rip = coroutine::start;
	_reg->rsp = &_stack[STACK_LEN - 20];
	_reg->rcx = this;
	_stack[STACK_LEN - 20] = coroutine::end;
	_stack[STACK_LEN - 20 + 5] = this;
	_stack[STACK_LEN - 20 + 6] = this;
#else
	_reg->eip = coroutine::start;
	_reg->esp = &_stack[STACK_LEN - 20];
	_stack[STACK_LEN - 20] = coroutine::end;
	_stack[STACK_LEN - 20 + 1] = this;
	_stack[STACK_LEN - 20 + 2] = this;
#endif // _WIN64
#endif // _WIN32
#ifdef linux
	_reg->rip = (void*)coroutine::start;
	_reg->rsp = &_stack[STACK_LEN - 20];
	_reg->rbp = (void*)id;
	_reg->rdi = this;
	_stack[STACK_LEN - 20] = (void*)coroutine::end;
	_stack[STACK_LEN - 20 + 2] = this;
	_stack[STACK_LEN - 20 + 3] = this;
#endif // linux

}

void coroutine::start(coroutine* cor)
{
	cor->_f();
}

void coroutine::end(STACK_SEAT coroutine* cor)
{
	cor->_state = state_end;
	jmps(cor->_control->_cur_coroutine->_reg, &cor->_control->_control_context);
}

coroutine::~coroutine()
{
	if (_stack != nullptr)
	{
		delete[] _stack;
		_stack = nullptr;
	}
	if (_reg != nullptr)
	{
		delete _reg;
		_reg = nullptr;
	}
}

coroutine_control::coroutine_control()
{
	_cur_coroutine = nullptr;
	_thid = std::this_thread::get_id();
	_mtx.lock();
	_coroutine_control_list.insert(std::make_pair(std::this_thread::get_id(), this));
	_mtx.unlock();
}

coroutine_control::~coroutine_control()
{
	for (auto it = _coroutine_list.begin(); it != _coroutine_list.end();)
	{
		coroutine* c = it->second;
		_coroutine_list.erase(it++);
		delete c;
	}
	_mtx.lock();
	_coroutine_control_list.erase(_coroutine_control_list.find(std::this_thread::get_id()));
	_mtx.unlock();
}

coroutine_id coroutine_control::create(coroutine_function f)
{
	coroutine_id id = _coroutine_list.size();
	coroutine* ct = new coroutine(f, id, this);
	_coroutine_list.insert(std::make_pair(id, ct));
	
	return id;
}

void coroutine_control::free(coroutine_id id)
{
	auto it = _coroutine_list.find(id);
	if (it != _coroutine_list.end())
	{
		//好奇葩 要先删除才能释放
		coroutine* c = it->second;
		_coroutine_list.erase(it);
		delete c;
	}
}

void coroutine_control::yeild()
{
	context* control_context = nullptr;
	context* cur_coroutine_context = nullptr;
	size_t begin_ptr = 0;
	size_t end_ptr = 0;
	//获取当前esp的地址  取当前栈中的变量地址就好了 只是判断esp所在的范围
	size_t xsp_ptr = (size_t) & begin_ptr;
	_mtx.lock();
	//到控制器map里面找出当前线程的那个控制器
	auto it = _coroutine_control_list.find(std::this_thread::get_id());
	if (it == _coroutine_control_list.end()) 
	{
		_mtx.unlock();
		return;
	}
	if (it->second->_cur_coroutine == nullptr)
	{
		_mtx.unlock();
		return;
	}
	//取出当前运行携程的栈范围
	begin_ptr = (size_t)it->second->_cur_coroutine->_stack;
	end_ptr = (size_t)it->second->_cur_coroutine->_stack + it->second->_cur_coroutine->_stack_len;
	//比较栈范围
	if (xsp_ptr > begin_ptr && xsp_ptr < end_ptr)
	{
		control_context = &it->second->_control_context;
		cur_coroutine_context = it->second->_cur_coroutine->_reg;
	}
	_mtx.unlock();
	if (control_context != nullptr && cur_coroutine_context != nullptr)
	{
		it->second->_coroutine_queue.push(it->second->_cur_coroutine->_id);
		jmps(cur_coroutine_context, control_context);
	}
}

void coroutine_control::resume(coroutine_id id)
{
	auto it = _coroutine_list.find(id);
	if (it == _coroutine_list.end())
	{
		return;
	}
	else
	{
		_cur_coroutine = it->second;
		if (_cur_coroutine->_state == coroutine::state_ready)
		{
			jmps(&_control_context, _cur_coroutine->_reg);
		}
		if (_cur_coroutine->_state == coroutine::state_end)
		{
			free(id);
			_cur_coroutine = nullptr;
		}
	}
}

void coroutine_control::join()
{
	for (auto it = _coroutine_list.begin(); it != _coroutine_list.end(); it++)
	{
		_coroutine_queue.push(it->first);
	}
	while (_coroutine_queue.size())
	{
		coroutine_id id = _coroutine_queue.front();
		_coroutine_queue.pop();
		resume(id);
	}
}

