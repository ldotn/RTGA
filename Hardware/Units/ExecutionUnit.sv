import SharedDefinitions::*;

module execution_unit 
//#() 
(
    input bit clk,
    input Instruction next_instruction[4],
    input CodeAddress next_code_address[4],
    input bit async_request_finished[4],
	
	// There might not be space on the queues  TODO : See if it makes sense to provide a unified API for async requests
	input bit is_trace_queue_full,
	input bit is_memory_queue_full,

	output MemoryAddress memory_request_address,
	output bit request_memory_read,
	input bit [31:0] memory_read_data,

	output CBufferIndex cb_read_index,
	input bit [31:0] cb_data,

	output bit request_trace[4],
	input TraceResult trace_results[4],

	input ScreenCoord quad_xy,
	output Pixel out_quad[4],

	input bit begin_signal,
	output bit finished_signal,

	// Initialization signals
	input bit rst,
	input float constants_in[OP_CONSTANTS_PER_EU]
);

// Op constants
float op_constants[OP_CONSTANTS_PER_EU];

// Register file


endmodule