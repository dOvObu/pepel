#include "pepel_vm.h"
#include <vector>
#include <iostream>
#include <fstream>


using namespace pepel;

// any amount of loactions
uint32_t memory[65536];

enum
{
	R_REGISTER_0 = 0,
	R_REGISTER_1,
	R_REGISTER_2,
	R_REGISTER_3,
	R_REGISTER_4,
	R_REGISTER_5,
	R_REGISTER_6,
	R_REGISTER_7,
	R_REGISTER_8,
	R_REGISTER_9,
	R_PROGRAM_COUNTER,
	R_COUNT,
};
uint32_t reg[R_COUNT];

// operations for instructions
enum
{
	OP_BRANCH = 0,
	OP_ADD,
	OP_LOAD,
	OP_STORE,
	OP_JUMP_REGISTER,
	OP_AND,
	OP_LOAD_REGISTER,
	OP_STORE_REGISTER,
	OP_RTI, // ???
	OP_BITWISE_NOT,
	OP_LOAD_INDIRECT,
	OP_STORE_INDERECT,
	OP_JUMP,
	OP_RESERVED, // ???
	OP_LOAD_EFFECTIVE_ADDRESS,
	OP_EXECUTE_TRAP,
};

// condition flags
enum // sign of prew calculation
{
	FL_POSITIVE = 1 << 0,
	FL_ZERO     = 1 << 1,
	FL_NEGATIVE = 1 << 2,
};



void run(char const* path)
{
	
}

