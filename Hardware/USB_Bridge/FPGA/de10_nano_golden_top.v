// ============================================================================
// Copyright (c) 2015 by Terasic Technologies Inc.
// ============================================================================
//
// Permission:
//
//   Terasic grants permission to use and modify this code for use
//   in synthesis for all Terasic Development Boards and Altera Development 
//   Kits made by Terasic.  Other use of this code, including the selling 
//   ,duplication, or modification of any portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design reference
//   which illustrates how these types of functions can be implemented.
//   It is the user's responsibility to verify their design for
//   consistency and functionality through the use of formal
//   verification methods.  Terasic provides no warranty regarding the use 
//   or functionality of this code.
//
// ============================================================================
//           
//  Terasic Technologies Inc
//  9F., No.176, Sec.2, Gongdao 5th Rd, East Dist, Hsinchu City, 30070. Taiwan
//  
//  
//                     web: http://www.terasic.com/  
//                     email: support@terasic.com
//
// ============================================================================
//Date:  Tue Mar  3 15:11:40 2015
// ============================================================================

//`define ENABLE_HPS

module DE10_Nano_golden_top(

      ///////// ADC /////////
      output             ADC_CONVST,
      output             ADC_SCK,
      output             ADC_SDI,
      input              ADC_SDO,

      ///////// ARDUINO /////////
      inout       [15:0] ARDUINO_IO,
      inout              ARDUINO_RESET_N,

      ///////// FPGA /////////
      input              FPGA_CLK1_50,
      input              FPGA_CLK2_50,
      input              FPGA_CLK3_50,

      ///////// GPIO /////////
      inout       [35:0] GPIO_0,
      inout       [35:0] GPIO_1,

      ///////// HDMI /////////
      inout              HDMI_I2C_SCL,
      inout              HDMI_I2C_SDA,
      inout              HDMI_I2S,
      inout              HDMI_LRCLK,
      inout              HDMI_MCLK,
      inout              HDMI_SCLK,
      output             HDMI_TX_CLK,
      output      [23:0] HDMI_TX_D,
      output             HDMI_TX_DE,
      output             HDMI_TX_HS,
      input              HDMI_TX_INT,
      output             HDMI_TX_VS,

`ifdef ENABLE_HPS
      ///////// HPS /////////
      inout              HPS_CONV_USB_N,
      output      [14:0] HPS_DDR3_ADDR,
      output      [2:0]  HPS_DDR3_BA,
      output             HPS_DDR3_CAS_N,
      output             HPS_DDR3_CKE,
      output             HPS_DDR3_CK_N,
      output             HPS_DDR3_CK_P,
      output             HPS_DDR3_CS_N,
      output      [3:0]  HPS_DDR3_DM,
      inout       [31:0] HPS_DDR3_DQ,
      inout       [3:0]  HPS_DDR3_DQS_N,
      inout       [3:0]  HPS_DDR3_DQS_P,
      output             HPS_DDR3_ODT,
      output             HPS_DDR3_RAS_N,
      output             HPS_DDR3_RESET_N,
      input              HPS_DDR3_RZQ,
      output             HPS_DDR3_WE_N,
      output             HPS_ENET_GTX_CLK,
      inout              HPS_ENET_INT_N,
      output             HPS_ENET_MDC,
      inout              HPS_ENET_MDIO,
      input              HPS_ENET_RX_CLK,
      input       [3:0]  HPS_ENET_RX_DATA,
      input              HPS_ENET_RX_DV,
      output      [3:0]  HPS_ENET_TX_DATA,
      output             HPS_ENET_TX_EN,
      inout              HPS_GSENSOR_INT,
      inout              HPS_I2C0_SCLK,
      inout              HPS_I2C0_SDAT,
      inout              HPS_I2C1_SCLK,
      inout              HPS_I2C1_SDAT,
      inout              HPS_KEY,
      inout              HPS_LED,
      inout              HPS_LTC_GPIO,
      output             HPS_SD_CLK,
      inout              HPS_SD_CMD,
      inout       [3:0]  HPS_SD_DATA,
      output             HPS_SPIM_CLK,
      input              HPS_SPIM_MISO,
      output             HPS_SPIM_MOSI,
      inout              HPS_SPIM_SS,
      input              HPS_UART_RX,
      output             HPS_UART_TX,
      input              HPS_USB_CLKOUT,
      inout       [7:0]  HPS_USB_DATA,
      input              HPS_USB_DIR,
      input              HPS_USB_NXT,
      output             HPS_USB_STP,
`endif /*ENABLE_HPS*/

      ///////// KEY /////////
      input       [1:0]  KEY,

      ///////// LED /////////
      output      [7:0]  LED,

      ///////// SW /////////
      input       [3:0]  SW
);

// TODO : See what's the best frequency we get from this, using 1 MHz for now
wire bridge_clk;
usb_pll ( FPGA_CLK1_50, 0,  bridge_clk );



// Performance bench

reg [23:0] clkdivider = 0;
wire bench_clk = clkdivider[18];
wire slow_bridge_clk = clkdivider[16];

always @(posedge bridge_clk) begin
	clkdivider <= clkdivider + 1;
end


`define WAITING_HEADER 1
`define READING 2
`define WRITING 3


reg [1:0] bench_state = `READING;//`WAITING_HEADER;
reg write_sig = 0;
reg read_sig = 1;


wire ready;
reg [7:0] write_data;
wire [7:0] read_data;

always @(posedge ready) begin
	case(bench_state)
		/*`WAITING_HEADER : begin
			if(read_data == 16) begin
				bench_state <= `READING;
			end
		end*/
		
		`READING : begin
			// Switch to writing
			write_data <= read_data;
			read_sig <= 0;
			write_sig <= 1;
			bench_state <= `WRITING;
		end
		
		`WRITING : begin
			// Finished writing so read the next value
			read_sig <= 1;
			write_sig <= 0;
			bench_state <= `READING;
		end
	endcase
end


usb_bridge 
(
	slow_bridge_clk, write_sig, read_sig, write_data, read_data, ready,
	//ft_D, ft_cs, ft_a0, ft_rd, ft_wr
	GPIO_0[7:0], GPIO_0[8], GPIO_0[9], GPIO_0[10], GPIO_0[11]
);


// debug
assign LED[4:0] = (bench_state == `WRITING) ? write_data : read_data;
assign LED[7:6] = bench_state;
/*assign LED[1:0] = bench_state;
assign LED[6] = slow_bridge_clk;
assign LED[7] = ready;*/

assign GPIO_1[1:0] = bench_state;
assign GPIO_1[5] = slow_bridge_clk;
assign GPIO_1[6] = ready;
/*assign GPIO_1[3] = read_sig;
assign GPIO_1[4] = write_sig;*/

/*assign GPIO_1[5] = GPIO_0[9];
assign GPIO_1[6] = GPIO_0[10];
assign GPIO_1[7] = GPIO_0[11];*/

/*assign GPIO_1[0] = bridge_clk;
assign GPIO_1[2] = ready;
assign GPIO_1[3] = read_sig;
assign GPIO_1[12:4] = GPIO_0[7:0];*/

endmodule
