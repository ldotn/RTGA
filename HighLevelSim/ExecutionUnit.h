#pragma once
#include <systemc.h> 
#include "SharedTypes.h"
#include "NiosFPU.h"
#include "ISA.h"

struct ExecutionUnit : sc_module
{
	sc_in_clk clk;

	sc_in<Instruction> next_instruction[4];
	sc_out<CodeAddress> next_code_address[4];

	// This can be a trace or a memory request
	sc_in<bool> async_request_finished[4];
	
	// There might not be space on the queues
	// TODO : See if it makes sense to provide a unified API for async requests
	sc_in<bool> is_trace_queue_full;
	sc_in<bool> is_memory_queue_full;

	sc_out<MemoryAddress> memory_request_address;
	sc_out<bool> request_memory_read;
	sc_in<uint32_t> memory_read_data;

	sc_out<CBIndex> cb_read_index;
	sc_in<float> cb_data;

	sc_out<bool> request_trace[4];
	sc_in<TraceResult> trace_results[4];

	sc_in<PixelCoord> quad_xy;
	sc_out<Pixel> out_quad[4];

	sc_in<bool> begin_signal;
	sc_out<bool> finished_signal;

	SC_CTOR(ExecutionUnit) :
		clk("clk"),
		next_instruction { sc_in<Instruction>("next_instruction0"), 
			sc_in<Instruction>("next_instruction1"),
			sc_in<Instruction>("next_instruction2"), 
			sc_in<Instruction>("next_instruction3") },
		next_code_address { sc_out<CodeAddress>("next_code_address0"),
			sc_out<CodeAddress>("next_code_address1"),
			sc_out<CodeAddress>("next_code_address2"),
			sc_out<CodeAddress>("next_code_address3") },
		memory_request_address("memory_request_address"),
		request_memory_read("request_memory_read"),
		memory_read_data("memory_read_data"),
		quad_xy("quad_xy"),
		out_quad { sc_out<Pixel>("out_quad0"),
			sc_out<Pixel>("out_quad1"),
			sc_out<Pixel>("out_quad2"),
			sc_out<Pixel>("out_quad3") },
		begin_signal("begin_signal"),
		finished_signal("finished_signal"),
		cb_read_index("cb_read_index"),
		cb_data("cb_data"),
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
		TRACE_ARR(gTraceFile, mRegisters, 0);
		TRACE_ARR(gTraceFile, mRegisters, 1);
		TRACE_ARR(gTraceFile, mRegisters, 2);
		TRACE_ARR(gTraceFile, mRegisters, 3);
		TRACE_ARR(gTraceFile, next_code_address, 0);
		TRACE_ARR(gTraceFile, next_code_address, 1);
		TRACE_ARR(gTraceFile, next_code_address, 2);
		TRACE_ARR(gTraceFile, next_code_address, 3);
		TRACE_ARR(gTraceFile, next_instruction, 0);
		TRACE_ARR(gTraceFile, next_instruction, 1);
		TRACE_ARR(gTraceFile, next_instruction, 2);
		TRACE_ARR(gTraceFile, next_instruction, 3);
		TRACE_VAR(gTraceFile, mExecutingMultiCycle);
		TRACE_VAR(gTraceFile, mStartExecutingMultiCycle);

		mFPU.BindTrace();
	}

	//  Taking a page from ARM here.Instead of storing operation constants on the instruction,
	// store them on a small buffer filled by the compiler and index that
	float mOpConstants[kOpConstantsPerEU];
private:
	NiosFPU    mFPU;
	sc_signal<float> fpu_dataa;
	sc_signal<float> fpu_datab;
	sc_signal<sc_uint<5>> fpu_n;
	sc_signal<float> fpu_result;
	sc_signal<bool> fpu_reset;
	sc_signal<bool> fpu_clk_en;
	sc_signal<bool> fpu_start;
	sc_signal<bool> fpu_done;

	sc_signal<TraceResult> trace_result_sig[4];

	bool mFinished[4];
	Instruction mCurrentInstruction[4];
	sc_uint<2> mCurrentPixelIndex;
	RegisterFile mRegisters[4]; // One file per pixel of the quad
	bool mStartExecutingMultiCycle;
	bool mExecutingMultiCycle;
};