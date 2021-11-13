import SharedDefinitions::*;

module memory_controller
#(
    parameter QUEUE_LENGTH = 8
)
(
    input bit clk,
    
    input bit request_read,
    input bit request_write,
    input MemoryAddress request_address, 
    input bit [31:0] write_data,
    output bit [31:0] read_data,

    // Only reads are queued, writes are locking
    output bit read_queue_full,

    // Avalon ports
    input  bit         ddr3_avl_ready,             
    output bit [25:0]  ddr3_avl_addr,               
    input  bit         ddr3_avl_rdata_valid,     
    input  bit [127:0] ddr3_avl_rdata,             
    output bit [127:0] ddr3_avl_wdata,                
    output bit         ddr3_avl_read_req,            
    output bit         ddr3_avl_write_req            
);

// The avalon bus is 128 bit wide
assign ddr3_avl_addr = request_address[27:2];

// TODO : Implement enqueuing 

always_ff @( clk ) begin : FSM
    
end

endmodule