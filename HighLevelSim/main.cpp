#include "ExecutionUnit.h"

sc_trace_file* gTraceFile;

// Move this out of here
SC_MODULE(Testbench)
{
	sc_in_clk clk;
	sc_out<bool> begin_signal;

	void Step()
	{
		begin_signal.write(true);
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
	sc_signal<PixelCoord> next_block_xy;
	sc_signal<PixelQuad> out_quad;
	sc_signal<bool> begin_signal;
	sc_signal<bool> finished_signal;

	sc_clock clk("Clock", 10, SC_NS, 0.5, 0.0, SC_NS);

	ExecutionUnit eu0("EU0");

	eu0.clk(clk); 
	eu0.current_instruction(current_instruction); 
	eu0.next_code_address(next_code_address); 
	eu0.memory_request_address(memory_request_address); 
	eu0.request_memory_read(request_memory_read); 
	eu0.next_block_xy(next_block_xy); 
	eu0.out_quad(out_quad);
	eu0.begin_signal(begin_signal);
	eu0.finished_signal(finished_signal);

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

	sc_start(1, SC_MS);

	sc_close_vcd_trace_file(gTraceFile);

	return 0;
}