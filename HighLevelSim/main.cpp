#include "ExecutionUnit.h"

sc_trace_file* gTraceFile;

// Move this out of here
SC_MODULE(Testbench)
{
	sc_in_clk clk;
	sc_out<bool> begin_signal;

	bool started = false;

	void Step()
	{
		if (!started)
		{
			begin_signal.write(true);
			started = true;
		}
		else
		{
			begin_signal.write(false);
		}
	}

	SC_CTOR(Testbench)
	{
		SC_THREAD(Step);
		sensitive << clk.pos();
	}
};


int main(int argc, const char* argv[])
{
	// I want to log EVERYTHING
	sc_report_handler::set_actions(SC_INFO, SC_DISPLAY);
	sc_report_handler::set_actions(SC_WARNING, SC_DISPLAY);
	sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
	sc_report_handler::set_actions(SC_FATAL, SC_THROW);

	// Structural coding
	sc_signal<Instruction> current_instruction;
	sc_signal<CodeAddress> next_code_address;
	sc_signal<MemoryAddress> memory_request_address;
	sc_signal<bool> request_memory_read;
	sc_signal<PixelCoord> quad_xy;
	sc_signal<Pixel> out_quad;
	sc_signal<bool> begin_signal;
	sc_signal<bool> finished_signal;
	sc_signal<bool> async_request_finished;
	sc_signal<bool> is_trace_queue_full;
	sc_signal<bool> is_memory_queue_full;
	sc_signal<float> cb_data;
	sc_signal<TraceResult> trace_results;

	sc_clock clk("Clock", 10, SC_NS, 0.5, 0.0, SC_NS);

	ExecutionUnit eu0("EU0");
	eu0.clk(clk); 
	eu0.next_instruction[0](current_instruction);
	eu0.next_instruction[1](current_instruction);
	eu0.next_instruction[2](current_instruction);
	eu0.next_instruction[3](current_instruction);
	eu0.next_code_address[0](next_code_address); 
	eu0.next_code_address[1](next_code_address); 
	eu0.next_code_address[2](next_code_address); 
	eu0.next_code_address[3](next_code_address); 
	eu0.memory_request_address(memory_request_address); 
	eu0.request_memory_read(request_memory_read); 
	eu0.quad_xy(quad_xy);
	eu0.out_quad[0](out_quad);
	eu0.out_quad[1](out_quad);
	eu0.out_quad[2](out_quad);
	eu0.out_quad[3](out_quad);
	eu0.begin_signal(begin_signal);
	eu0.finished_signal(finished_signal);
	eu0.async_request_finished[0](async_request_finished);
	eu0.async_request_finished[1](async_request_finished);
	eu0.async_request_finished[2](async_request_finished);
	eu0.async_request_finished[3](async_request_finished);
	eu0.cb_data(cb_data);
	eu0.trace_results[0](trace_results);
	eu0.trace_results[1](trace_results);
	eu0.trace_results[2](trace_results);
	eu0.trace_results[3](trace_results);
	eu0.is_trace_queue_full(is_trace_queue_full);
	eu0.is_memory_queue_full(is_memory_queue_full);

	// Run simulation
	Testbench tst("Test");
	tst.clk(clk);
	tst.begin_signal(begin_signal);


	// TODO : MOVE THIS OUT OF HERE
	eu0.mOpConstants[0] = 1.0f;
	Instruction add1;
	add1.ArithmeticConst.constant_idx = 0;
	add1.ArithmeticConst.op = InstructionOpcode::add;
	add1.ArithmeticConst.r_a = 0;
	add1.ArithmeticConst.r_out = 0;
	add1.ArithmeticConst.use_constant = 1;
	current_instruction = add1;


	gTraceFile = sc_create_vcd_trace_file("wave");
	eu0.BindTrace();

	sc_start(10, SC_US);

	sc_close_vcd_trace_file(gTraceFile);

	return 0;
}