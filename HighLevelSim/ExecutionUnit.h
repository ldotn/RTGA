#pragma once
#include <systemc.h> 
#include "SharedTypes.h"
#include "NiosFPU.h"
#include "ISA.h"

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

		mFPU("FPU")
	{
		SC_CTHREAD(Step, clk.pos());
		SC_CTHREAD(StepNegEdge, clk.neg());

		mFPU.clk(clk);
		mFPU.dataa(fpu_dataa);
		mFPU.datab(fpu_datab);
		mFPU.n(fpu_n);
		mFPU.result(fpu_result);
		mFPU.reset(fpu_reset);
		mFPU.clk_en(fpu_clk_en);
		mFPU.start(fpu_start);
		mFPU.done(fpu_done);
	}

	void Step();
	void StepNegEdge();

	void BindTrace()
	{
		TRACE_VAR(gTraceFile, clk);
		TRACE_VAR(gTraceFile, mCurrentPixelIndex);
		TRACE_VAR(gTraceFile, mRegisters[0]);
		TRACE_VAR(gTraceFile, mRegisters[1]);
		TRACE_VAR(gTraceFile, mRegisters[2]);
		TRACE_VAR(gTraceFile, mRegisters[3]);
		TRACE_VAR(gTraceFile, next_code_address);
		TRACE_VAR(gTraceFile, mExecutingMultiCycle);
		TRACE_VAR(gTraceFile, mStartExecutingMultiCycle);

		mFPU.BindTrace();
	}

	//  Taking a page from ARM here.Instead of storing operation constants on the instruction,
	// store them on a small buffer filled by the compiler and index that
	float mOpConstants[kOpConstantsPerEU];
private:
	bool mStartExecutingMultiCycle;
	bool mExecutingMultiCycle;

	NiosFPU    mFPU;
	sc_signal<float> fpu_dataa;
	sc_signal<float> fpu_datab;
	sc_signal<sc_uint<5>> fpu_n;
	sc_signal<float> fpu_result;
	sc_signal<bool> fpu_reset;
	sc_signal<bool> fpu_clk_en;
	sc_signal<bool> fpu_start;
	sc_signal<bool> fpu_done;

	Instruction mCurrentInstruction;
	int mCurrentPixelIndex;
	float mCurrentPixelIndexFloat; //It's only 2 bits but stored as a float so it can be operated upon 
	float mCurrentQuadXY[2];
	RegisterFile mRegisters[4]; // One file per pixel of the quad
};