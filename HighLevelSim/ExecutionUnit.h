#pragma once
#include <systemc.h>
#include "SharedTypes.h"

struct FetchUnit : sc_module
{
	sc_in_clk clk;

	SC_CTOR(FetchUnit)
	{
		SC_CTHREAD(Step, clk.pos());
	}

	void Step();
};

struct DecodeUnit : sc_module
{
	sc_in_clk clk;

	SC_CTOR(DecodeUnit)
	{
		SC_CTHREAD(Step, clk.pos());
	}

	void Step();
};

struct ALU : sc_module
{
	sc_in_clk clk;

	SC_CTOR(ALU)
	{
		SC_CTHREAD(Step, clk.pos());
	}

	void Step();
};

struct ExecutionUnit : sc_module
{
	sc_in_clk clk;

	sc_in<Instruction> current_instruction;
	sc_out<CodeAddress> next_code_address;

	sc_out<MemoryAddress> memory_request_address;
	sc_out<bool> request_memory_read;

	sc_in<PixelCoord> next_block_xy;
	sc_out<PixelQuad> out_quad;

	sc_in<bool> begin_signal;
	sc_out<bool> finished_signal;

	SC_CTOR(ExecutionUnit) :
		clk("clk"),
		current_instruction("current_instruction"),
		next_code_address("next_code_address"),
		memory_request_address("memory_request_address"),
		request_memory_read("request_memory_read"),
		next_block_xy("next_block_xy"),
		out_quad("out_quad"),
		begin_signal("begin_signal"),
		finished_signal("finished_signal"),

		mFetch("Fetch"),
		mDecode("Decode"),
		mALU("ALU")
	{
		SC_CTHREAD(Step, clk.pos());

		mFetch.clk(clk);
		mDecode.clk(clk);
		mALU.clk(clk);
	}

	void Step();
private:
	int mIP;
	int mCurrentPixelIndex; // Inside the quad
	PixelCoord mCurrentQuadXY;

	FetchUnit  mFetch;
	DecodeUnit mDecode;
	ALU        mALU;
};