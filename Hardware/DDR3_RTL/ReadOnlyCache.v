`define ADDR_W (EXT_ADDR_W + $clog2(LINES_W / DATA_W))
`define LINES_COUNT (1 << LINE_ADDR_W)

// Fully associative read only cache
// It interfaces with an external memory through a bus of the same size as the cache lines
module Read_Only_Cache
#(
	parameter LINES_W = 128,
	parameter DATA_W = 8, // Must be less than LINES_W
	parameter LINE_ADDR_W = 8,
	parameter EXT_ADDR_W = 26 
)
(
	input wire clk,
	input wire read_rq,
	input wire [`ADDR_w-1:0] address,
		
	output wire ext_read_rq,
	input wire ext_rq_finished,
	output wire [EXT_ADDR_W-1:0] ext_address,
	input wire [LINES_W-1:0] ext_data,
	
	output wire [DATA_W-1:0] read_data,
	output wire finished,
	output wire busy
);
 
reg [DATA_W - 1:0] data_lines[`LINES_COUNT];
reg [LINE_ADDR_W-1:0] last_accessed_line;

`define STATE_IDLE 0
`define STATE_WAITING_EXT 1

reg [1:0] state = `STATE_IDLE;

assign busy = (state == `STATE_WAITING_EXT);
assign line_address = address & ((1 << LINE_ADDR_W) - 1);
assign ext_address = address >> $clog2(LINES_W / DATA_W);

always @(posedge clk) begin
	case(state)
		`STATE_IDLE: begin
		end
		
		`STATE_WAITING_EXT: begin
			if(ext_rq_finished) begin
				state = `STATE_IDLE;
				
			end
		end
	endcase
end

endmodule