#pragma once
#include <systemc.h>

using namespace sc_core;

struct ExecutionUnit : sc_module
{
	sc_in<int> codetable;
	sc_out<int> current_ip;
	sc_in<BlockIndex> current_block;

	sc_in_clk clk;

	SC_CTOR(ExecutionUnit)
	{
		SC_CTHREAD(Step, clk.pos());
	}

	void Step();
private:
	int mIP;
	int m
	FetchUnit  mFetch;
	DecodeUnit mDecode;
	ALU        mALU;
};