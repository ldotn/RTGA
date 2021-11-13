module usb_bridge
(
	input wire clk,
	
	input wire write_sig,
	input wire read_sig,
	input wire [7:0] write_data,
	output reg [7:0] read_data,
	output reg finished,
	
	inout wire [7:0] ft_D,
	// Note that this signals are inverted, low is enabled!
	output reg ft_cs,
	output reg ft_a0,
	output reg ft_rd,
	output reg ft_wr
);



`define IDLE_STATE 0
`define SETUP_STATE 1
`define READ_STATE 2
`define WRITE_STATE 3
`define RESTORE_STATE 4

reg [2:0] state;
reg [7:0] write_buffer;

assign ft_D = ~ft_rd ? 8'bz : write_buffer;

initial begin
	ft_a0 <= 1'b 1;
	ft_cs <= 1'b 1;
	ft_wr <= 1'b 1;
	ft_rd <= 1'b 1;
	finished <= 1'b 0;
	state <= `IDLE_STATE;

	write_buffer <= 8'b 0;
	read_data <= 8'b 0;
end

always @(posedge clk) begin
	case (state)
		`RESTORE_STATE : begin
			ft_a0 <= 1'b 1;
			ft_cs <= 1'b 1;
			finished <= 1'b 0;
			state <= `IDLE_STATE;
		end
		
		`IDLE_STATE : begin
			if (read_sig || write_sig) begin
				ft_a0 <= 1'b 0;
				ft_cs <= 1'b 0;
				state <= `SETUP_STATE;
			end
		end
		
		`SETUP_STATE : begin
			if (read_sig) begin
				ft_rd <= 1'b 0;
				state <= `READ_STATE;
			end else if(write_sig) begin
				write_buffer <= write_data;
				ft_wr <= 1'b 0;
				state <= `WRITE_STATE;
			end
		end
		
		`WRITE_STATE : begin
			ft_wr <= 1'b 1;
			finished <= 1'b 1;
			state <= `RESTORE_STATE;
		end
		
		`READ_STATE : begin
			read_data <= ft_D;
			ft_rd <= 1'b 1;
			finished <= 1'b 1;
			state <= `RESTORE_STATE;
		end
	endcase
end

endmodule