#include "../asmjit/asmjit.h"

using namespace asmjit;
using namespace asmjit::x86;
int main(int argc, char* argv[]) {
	JitRuntime runtime;
	X86Compiler c(&runtime);

	// Function returning 'int' accepting pointer and two indexes.
	c.addFunc(kFuncConvHost, FuncBuilder3<int, const int*, intptr_t, intptr_t>());

	X86GpVar p(c, kVarTypeIntPtr, "p");
	X86GpVar aIndex(c, kVarTypeIntPtr, "aIndex");
	X86GpVar bIndex(c, kVarTypeIntPtr, "bIndex");

	c.setArg(0, p);
	c.setArg(1, aIndex);
	c.setArg(2, bIndex);

	X86GpVar a(c, kVarTypeInt32, "a");
	X86GpVar b(c, kVarTypeInt32, "b");

	// Read 'a' by using a memory operand having base register, index register
	// and scale. Translates to 'mov a, dword ptr [p + aIndex << 2]'.
	
	c.mov(a, ptr(p, aIndex, 2));

	// Read 'b' by using a memory operand having base register only. Variables
	// 'p' and 'bIndex' are both modified.

	// Shift bIndex by 2 (exactly the same as multiplying by 4).
	// And add scaled 'bIndex' to 'p' resulting in 'p = p + bIndex * 4'.
	c.shl(bIndex, 2);
	c.add(p, bIndex);

	// Read 'b'.
	c.mov(b, ptr(p));

	// a = a + b;
	c.add(a, b);

	c.ret(a);
	c.endFunc();

	// The prototype of the generated function changed also here.
	typedef int(*FuncType)(const int*, intptr_t, intptr_t);
	FuncType func = asmjit_cast<FuncType>(c.make());

	// Array passed to 'func'
	const int array[] = { 1, 2, 3, 5, 8, 13 };

	int x = func(array, 1, 2);
	int y = func(array, 3, 5);

	printf("x=%d\n", x); // Outputs "x=5".
	printf("y=%d\n", y); // Outputs "y=18".

	runtime.release((void*)func);
	return 0;
}