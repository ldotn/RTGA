#pragma once
#include "Helpers.h"
#include <systemc.h>
#include "SharedTypes.h"

/*
   Register Types
		4 sets of this registers, one per pixel of the quad
			- float          r[kRegistersPerThread]
			- CodeAddress   IP
			- float         TexSampleResult[3]
			- bool          CmpResult
			- TraceCommand  CurrentTraceCommand
				* float RayOrigin[3]
				* float RayDirection[3]
				* float MinT
				* float MaxT
			- TraceResult    HitData : Read only
				* float ClosestHitT
				* float BarycentricHit[3]
				* Triangle Hit
					* float Vertex[3][3]
					* float Normal[3][3]
					* float UV[3][2]
					* float UserDataIndex : Allows to have per triangle data (usually a material). Stored on the cbuffer
		One per EU (on the program logic it's one per Quad)
			- float CurrentPixelIdx : It's only 2 bits but stored as a float so it can be operated upon 
			- float QuadXY[2]  : Read only
			- float OpConstants[kOpConstantsPerEU] : Taking a page from ARM here. Instead of storing operation constants on the instruction,
													store them on a small buffer filled by the compiler and index that
 
	Instructions
		- Arithmetic (only floats)
			* Two options
				- r[A], r[B] => r[Out]
				- r[A], OpConstants[ConstantIndex] => r[Out]
			* Ops
				- add : Out = A + B
				- sub : Out = A - B
				- mul : Out = A * B
				- div : Out = A / B
				- sqrt : Out = sqrt(A)
				- floor : Out = floor(A)
				- round : Out = round(A)
				- min : Out = min(A, B)
				- max : Out = max(A, B)
				- neg : Out = -A
				- abs : Out = abs(A)
		- Memory
			* copy : r[Out] = r[InA]
			* select : r[Out] = CmpResult ? r[InA] : r[InB]
			* set : r[Out] = OpConstants[Constant]
		- Constants
			* cb_load : r[Out] = ConstantBuffer[r[Index]];
		- Flow control
			* cmp_lt : CmpResult = (A < B)
			* cmp_le : CmpResult = (A <= B)
			* cmp_gt : CmpResult = (A > B)
			* cmp_ge : CmpResult = (A >= B)
			* cmp_eq : CmpResult = (A == B)
			* cmp_ne : CmpResult = (A != B)
			* jpm : if(CmpResult) IP = CodeAddress. ALWAYS flushes the pipeline, there's no prediction or prefetch
		- Special
			* finish : Indicates that the current pixel is finished.
				Moves to the next pixel on the quad. If the quad its finished, notifies the pixel buffer
			* request_sample r0, r1 : starts an async memory read 
				* r0 and r1 must be positive integers less than 2^24
					* Integers on that range are stored accurately and can be easily converted to ints
					*   I_24 = (mantissa >> (23 - exp - 127) | (1 << (exp - 127))
				* r0 is the texture start address, r1 the texture offset
				* Min texture size is 16 bytes (2x2 pixels), max texture size is 64 MB and sample (pixel) size is 4 bytes
				* When finished, memory_read_data = MemoryLoad32(4 * r0 + r1) // Memory is indexed every 4 bytes
			* wait_sample FORMAT: waits until the sample result is on memory_read_data and converts it
				* FORMAT can be r8g8b8a8_unorm, r8g8b8_unorm, r16g16_unorm, or f32
				* TexSampleResult = Convert(FORMAT, memory_read_data) 
				I could move to the next pixel at this point, but keeping it simple for now because that requires more complex logic when you arrive to another memory request
			* trace : Requests a trace op. 
				Pauses execution and moves to the next pixel on the quad. If it's the last pixel of the quad it just waits.
			* nop : No-Op, does nothing. Should only be used at the start so the first instruction can be fetched
*/

constexpr int kRegistersPerThread = 16;
constexpr int kOpConstantsPerEU = 8;
constexpr int kRegisterBankSize = kRegistersPerThread 
								  + 3  // Tex sample results
								  + 8  // Trace command
								  + 25 // Trace results
								  + 1  // Current pixel idx
								  + 2; // QuadXY (shared on the EU)
constexpr int kRegisterIndexBits = max_to_bits(kRegisterBankSize - 1);
constexpr int kOpConstantIndexBits = max_to_bits(kOpConstantsPerEU - 1);

struct TraceCommand
{
	float RayOrigin[3];
	float RayDirection[3];
	float MinT;
	float MaxT;
};

struct Triangle
{
	float Vertex[3][3];
	float Normal[3][3];
	float UV[3][2];
	float UserDataIndex; // Allows to have per triangle data(usually a material). Stored on the cbuffer
};

struct TraceResult
{
	float ClosestHitT;
	float BarycentricHit[3];
	Triangle HitTriangle;
};

struct RegisterFile
{
	RegisterFile()
	{
		memset(&Bank.Physical, 0, sizeof(float) * kRegisterBankSize);
		
		IP = 0;
		CmpResult = 0;
	}

	union
	{
		struct
		{
			float r[kRegistersPerThread];
			float TexSampleResult[3];
			TraceCommand  CurrentTraceCommand;
			TraceResult   HitData;
			float QuadXY[2];
		} Logical;

		float Physical[kRegisterBankSize]; // I'm duplicating the QuadXY registers this way, for now let's keep it simple
	} Bank;
	
	CodeAddress   IP;
	bool          CmpResult;
};

inline void sc_trace(sc_trace_file* file, const TraceCommand& cmd, const std::string& str)
{
	sc_trace(file, cmd.RayOrigin[0], str + "_ray_orig_x");
	sc_trace(file, cmd.RayOrigin[1], str + "_ray_orig_y");
	sc_trace(file, cmd.RayOrigin[2], str + "_ray_orig_z");
	sc_trace(file, cmd.RayDirection[0], str + "_ray_dir_x");
	sc_trace(file, cmd.RayDirection[1], str + "_ray_dir_y");
	sc_trace(file, cmd.RayDirection[2], str + "_ray_dir_z");
	sc_trace(file, cmd.MinT, str + "_min_t");
	sc_trace(file, cmd.MaxT, str + "_max_t");
}

inline void sc_trace(sc_trace_file* file, const Triangle& trig, const std::string& str)
{
	sc_trace(file, trig.Vertex[0][0], str + "_v0_x");
	sc_trace(file, trig.Vertex[0][1], str + "_v0_y");
	sc_trace(file, trig.Vertex[0][2], str + "_v0_z");
	sc_trace(file, trig.Vertex[1][0], str + "_v1_x");
	sc_trace(file, trig.Vertex[1][1], str + "_v1_y");
	sc_trace(file, trig.Vertex[1][2], str + "_v1_z");
	sc_trace(file, trig.Vertex[2][0], str + "_v2_x");
	sc_trace(file, trig.Vertex[2][1], str + "_v2_y");
	sc_trace(file, trig.Vertex[2][2], str + "_v2_z");

	sc_trace(file, trig.Normal[0][0], str + "_n0_x");
	sc_trace(file, trig.Normal[0][1], str + "_n0_y");
	sc_trace(file, trig.Normal[0][2], str + "_n0_z");
	sc_trace(file, trig.Normal[1][0], str + "_n1_x");
	sc_trace(file, trig.Normal[1][1], str + "_n1_y");
	sc_trace(file, trig.Normal[1][2], str + "_n1_z");
	sc_trace(file, trig.Normal[2][0], str + "_n2_x");
	sc_trace(file, trig.Normal[2][1], str + "_n2_y");
	sc_trace(file, trig.Normal[2][2], str + "_n2_z");

	sc_trace(file, trig.UV[0][0], str + "_uv0_x");
	sc_trace(file, trig.UV[0][1], str + "_uv0_y");
	sc_trace(file, trig.UV[1][0], str + "_uv1_x");
	sc_trace(file, trig.UV[1][1], str + "_uv1_y");
	sc_trace(file, trig.UV[2][0], str + "_uv2_x");
	sc_trace(file, trig.UV[2][1], str + "_uv2_y");

	sc_trace(file, trig.UserDataIndex, str + "_cb_index");
}

inline void sc_trace(sc_trace_file* file, const TraceResult& result, const std::string& str)
{
	sc_trace(file, result.ClosestHitT, str + "_closest_t");
	sc_trace(file, result.BarycentricHit[0], str + "_barycentric_u");
	sc_trace(file, result.BarycentricHit[1], str + "_barycentric_v");
	sc_trace(file, result.BarycentricHit[2], str + "_barycentric_w");
	sc_trace(file, result.HitTriangle, str + "_trig");
}

inline void sc_trace(sc_trace_file* file, const RegisterFile& regs, const std::string& str)
{
	int counter = 0;
	for(float r : regs.Bank.Logical.r)
		sc_trace(file, r, str + "_r" + std::to_string(counter++));
	sc_trace(file, regs.IP, str + "_ip");
	sc_trace(file, regs.Bank.Logical.TexSampleResult[0], str + "_tex_r");
	sc_trace(file, regs.Bank.Logical.TexSampleResult[1], str + "_tex_g");
	sc_trace(file, regs.Bank.Logical.TexSampleResult[2], str + "_tex_b");
	sc_trace(file, regs.CmpResult, str + "_cmp");
	sc_trace(file, regs.Bank.Logical.CurrentTraceCommand, str + "_trace_cmd");
	sc_trace(file, regs.Bank.Logical.HitData, str + "_trace_result");
}

struct InstructionOpcode
{
	enum
	{
		add,
		sub,
		mul,
		div,
		sqrt,
		floor,
		round,

		min,
		max,
		neg,
		abs,

		cmp_lt,
		cmp_le,
		cmp_gt,
		cmp_ge,
		cmp_eq,
		cmp_ne,

		copy,
		select,
		set,

		request_sample,
		wait_sample,
		cb_load,
		jmp,
		finish,
		trace,

		nop,

		Count
	};

	static constexpr int kBits = max_to_bits(Count - 1);
};

struct Instruction
{
	union 
	{
		/*add,
		sub,
		mul,
		div,
		sqrt,
		floor,
		round,
		min,
		max,
		neg,
		abs,*/
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
			uint32_t use_constant : 1; // Always 0
			uint32_t r_out : kRegisterIndexBits;
			uint32_t r_a : kRegisterIndexBits;
			uint32_t r_b : kRegisterIndexBits; // Ignored on unitary ops
		} Arithmetic;
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
			uint32_t use_constant : 1;  // Always 1
			uint32_t r_out : kRegisterIndexBits;
			uint32_t r_a : kRegisterIndexBits;
			uint32_t constant_idx : kOpConstantIndexBits;
		} ArithmeticConst;

		// copy, select, set
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
			uint32_t r_out : kRegisterIndexBits;
			uint32_t r_a : kRegisterIndexBits;
			uint32_t r_b : kRegisterIndexBits; // Only used by select
			uint32_t constant_idx : kOpConstantIndexBits; // Only used by set
		} CopySelectSet;

		// cb_load
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
			uint32_t r_out : kRegisterIndexBits;
			uint32_t r_index : kRegisterIndexBits;
		} ConstantBuffer;

		/*cmp_lt,
			cmp_le,
			cmp_gt,
			cmp_ge,
			cmp_eq,
			cmp_ne,*/
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
			uint32_t use_constant : 1; // Always 0
			uint32_t r_a : kRegisterIndexBits;
			uint32_t r_b : kRegisterIndexBits;
		} Compare;
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
			uint32_t use_constant : 1; // Always 1
			uint32_t r_a : kRegisterIndexBits;
			uint32_t constant_idx : kOpConstantIndexBits;
		} CompareConst;

		// jmp
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
			uint32_t address : kCodeAddressBits;

			static constexpr int kBits = InstructionOpcode::kBits + kCodeAddressBits;
		} Jump;

		// finish, trace
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
		} Signal;


		// request_sample
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
			uint32_t r_base : kRegisterIndexBits;
			uint32_t r_offset : kRegisterIndexBits;
		} Sample;

		// wait_sample
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
			uint32_t format : TextureFormat::kBits;
		} WaitSample;

		// Undecoded instruction, just a way to get the op
		struct
		{
			uint32_t op : InstructionOpcode::kBits;
			uint32_t data : (32 - InstructionOpcode::kBits);
		} Raw;
	};

	bool operator==(const Instruction& rhs) const
	{
		return (Raw.op == rhs.Raw.op) && (Raw.data == rhs.Raw.data);
	}
};

inline void sc_trace(sc_trace_file* file, const Instruction& inst, const std::string& str)
{
	sc_trace(file, inst.Raw.op, str + "_op");
	sc_trace(file, inst.Raw.data, str + "_data");
}

inline ostream& operator<<(ostream& os, Instruction const& inst)
{
	os << inst.Raw.op << inst.Raw.data;
	return os;
}

// Make sure our instructions fit on 32 bits
static_assert(sizeof(Instruction) == 4);