package SharedDefinitions

typedef enum bit[4:0]
{  
    OP_FirstFPU,
	OP_FirstMultiCycleFPU = OP_FirstFPU,

	OP_add = OP_FirstMultiCycleFPU,
	OP_sub,
	OP_mul,
	OP_div,
	OP_sqrt,
	OP_floor,
	OP_round,

	OP_LastMultiCycleFPU = OP_round,

	OP_FirstCombFPU,

	OP_min = OP_FirstCombFPU,
	OP_max,
	OP_neg,
	OP_abs,

	OP_FirstCmp,
	OP_cmp_lt = OP_FirstCmp,
	OP_cmp_le,
	OP_cmp_gt,
	OP_cmp_ge,
	OP_cmp_eq,
	OP_cmp_ne,
	OP_LastCmp = OP_cmp_ne,

	OP_LastCombFPU = OP_cmp_ne,
	OP_LastFPU = OP_LastCombFPU,

	OP_FirstAsync,

	OP_wait_sample = OP_FirstAsync,
	OP_trace,
		
	OP_LastAsync = OP_trace,

	// Other ops
	OP_copy,
	OP_select,
	OP_set,

	OP_request_sample,
		
	OP_cb_load,
	OP_cjmp,
	OP_finish,

	OP_nop,

	OP_Count
} InstructionOpcode;

typedef enum bit [1:0] 
{
    TXFMT_R8G8B8A8_UNORM,
    TXFMT_R8G8B8_UNORM,
    TXFMT_R16G16_UNORM,
    TXFMT_F32
} TextureFormat;

parameter CODE_ADDRESS_BITS = 8
typedef bit [CODE_ADDRESS_BITS-1:0] CodeAddress;

// 1GB addressed at 32 bit directions
parameter MEMORY_ADDRESS_BITS = 28
typedef bit [MEMORY_ADDRESS_BITS-1:0] MemoryAddress;

parameter CBUFFER_INDEX_BITS =  8 
typedef bit [CBUFFER_INDEX_BITS-1:0] CBufferIndex; // 255 entries 32 bits each (1KB)

// 32 bit instructions
typedef struct packed 
{
    InstructionOpcode Opcode;
    bit [26:0] Data;
} Instruction;

// 2048x2048, the max I can get from HDMI is 1920x1080 anyway
typedef struct packed {
    bit [10:0] x;
    bit [10:0] y;
} ScreenCoord;

typedef struct packed {
    bit [7:0] r;
    bit [7:0] g;
    bit [7:0] b;
} Pixel;

endpackage