`define ADDR_W (EXT_ADDR_W + $clog2(LINES_W / DATA_W))
`define LINES_COUNT (1 << LINE_ADDR_W)

// Fully associative write through cache
module Cache
#(
	parameter LINES_W = 128,
	parameter DATA_W = 8, // Must be less than LINES_W
	parameter LINE_ADDR_W = 8,
	parameter EXT_ADDR_W = 26 
)
(
	input wire clk,
	input wire read_rq,
	input wire write_rq,
	input wire [`ADDR_w-1:0] address,
	input wire [DATA_W-1:0] write_data,
		
	output wire ext_read_rq,
	output wire ext_write_rq,
	input wire ext_rq_finished,
	output wire [EXT_ADDR_W-1:0] ext_address,
	output wire [LINES_W-1:0] ext_write_data,
	input wire [LINES_W-1:0] ext_read_data,
	
	output wire [DATA_W-1:0] read_data,
	output wire finished
);
 
reg [DATA_W - 1:0] data_lines[`LINES_COUNT];
reg [LINE_ADDR_W-1:0] last_accessed_line;

`define STATE_IDLE 0
`define STATE_

always @(posedge clk)
begin
end

endmodule