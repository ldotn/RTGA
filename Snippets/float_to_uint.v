module float_to_uint (
    input wire [31:0] float_in,
    output reg [23:0] uint_out
);

wire [4:0] exp;
assign exp = float_in[30:23] - 8'd127;

always @(float_in) begin
    if(float_in == 0) begin
        uint_out = 0;
    end else begin
        case (exp)
            0 : uint_out = 24'd1;
            1 : uint_out = {22'b0, 1'b1, float_in[22:22]};
            2 : uint_out = {21'b0, 1'b1, float_in[22:21]};
            3 : uint_out = {20'b0, 1'b1, float_in[22:20]};
            4 : uint_out = {19'b0, 1'b1, float_in[22:19]};
            5 : uint_out = {18'b0, 1'b1, float_in[22:18]};
            6 : uint_out = {17'b0, 1'b1, float_in[22:17]};
            7 : uint_out = {16'b0, 1'b1, float_in[22:16]};
            8 : uint_out = {15'b0, 1'b1, float_in[22:15]};
            9 : uint_out = {14'b0, 1'b1, float_in[22:14]};
            10 : uint_out = {13'b0, 1'b1, float_in[22:13]};
            11 : uint_out = {12'b0, 1'b1, float_in[22:12]};
            12 : uint_out = {11'b0, 1'b1, float_in[22:11]};
            13 : uint_out = {10'b0, 1'b1, float_in[22:10]};
            14 : uint_out = {9'b0, 1'b1, float_in[22:9]};
            15 : uint_out = {8'b0, 1'b1, float_in[22:8]};
            16 : uint_out = {7'b0, 1'b1, float_in[22:7]};
            17 : uint_out = {6'b0, 1'b1, float_in[22:6]};
            18 : uint_out = {5'b0, 1'b1, float_in[22:5]};
            19 : uint_out = {4'b0, 1'b1, float_in[22:4]};
            20 : uint_out = {3'b0, 1'b1, float_in[22:3]};
            21 : uint_out = {2'b0, 1'b1, float_in[22:2]};
            22 : uint_out = {1'b0, 1'b1, float_in[22:1]};
            23 : uint_out = {1'b1, float_in[22:0]};
            default: uint_out = 0;
        endcase
    end
end
endmodule

// Testbench
`timescale 1 ns/10 ps  // time-unit = 1 ns, precision = 10 ps

module testbench;
    reg [31:0] freg;
    wire [23:0] ireg;
    // duration for each bit = 20 * timescale = 20 * 1 ns  = 20ns
    localparam period = 20;  

    float_to_uint convertor (freg, ireg);
    
    initial // initial block executes only once
        begin
            freg = 0;
            // should be 0
            #period; // wait for period 

            freg = 32'h42280000;
            // should be 42
            #period;

            freg = 32'h41000000;
            // should be 8
            #period;

            freg = 32'h4996b438;
            // should be 1234567
            #period;
        end
endmodule
