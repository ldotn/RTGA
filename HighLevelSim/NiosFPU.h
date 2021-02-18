#pragma once
#include <systemc.h> 
#include "SharedTypes.h"
#include "ISA.h"

// Emulates the NiosII Floating Point Hardware 2 module from Intel
// https://www.intel.com/content/www/us/en/programmable/documentation/cru1439932898327.html#shd1506201174145
struct NiosFPU : sc_module
{
	// Signal names are the same as on Quartus
	// The component have separated ports for single cycle and multiple cycle instructions, so I think they could be executed simultaneously
	//		For simplicity I'm only doing one op at a time, so I replicate the ports on the HDL. I may change this later on.

	sc_in<float> dataa;
	sc_in<float> datab;
	sc_in<sc_uint<5>> n; // The input is 4 bits but the last bit switches between comb and seq
	sc_out<float> result;

	sc_in_clk clk;
	sc_in<bool> clk_en; 
	sc_in<bool> reset;
	//sc_in<bool> reset_req; // Don't know what this does...

	sc_in<bool> start;
	sc_out<bool> done; // For multicycle instructions

	SC_CTOR(NiosFPU)
	{
		SC_CTHREAD(Step, clk.pos());
	}

	void Step();

	void BindTrace()
	{
		TRACE_VAR(gTraceFile, clk);
		TRACE_VAR(gTraceFile, dataa);
		TRACE_VAR(gTraceFile, datab);
		TRACE_VAR(gTraceFile, n);
		TRACE_VAR(gTraceFile, result);
		TRACE_VAR(gTraceFile, clk_en);
		TRACE_VAR(gTraceFile, reset);
		TRACE_VAR(gTraceFile, start);
		TRACE_VAR(gTraceFile, done);
	}
private:
};