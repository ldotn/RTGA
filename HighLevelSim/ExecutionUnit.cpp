#include "ExecutionUnit.h"

void ExecutionUnit::Step()
{
	// initial
	mCurrentPixelIndex = 0;
	mCurrentPixelIndexFloat = 0.0f;
	mRegisters[0].IP = kHaltCodeAddress;
	mRegisters[1].IP = kHaltCodeAddress;
	mRegisters[2].IP = kHaltCodeAddress;
	mRegisters[3].IP = kHaltCodeAddress;
	next_code_address.write(kFirstCodeAddress);
	fpu_clk_en = 0;
	mExecutingMultiCycle = false;
	mCurrentInstruction.Raw.op = InstructionOpcode::nop;

	// always @(posedge clk)
	while (true)
	{
		wait();

		if (mRegisters[mCurrentPixelIndex].IP != kHaltCodeAddress)
		{
			Instruction next_inst = current_instruction.read();

			// Check execution status
			if (mExecutingMultiCycle)
			{
				if (fpu_done)
				{
					fpu_start = 0;
					mRegisters[mCurrentPixelIndex].r[mCurrentInstruction.Arithmetic.r_out] = fpu_result;
					mExecutingMultiCycle = false;
				}
			}
			else if(mCurrentInstruction.Raw.op != InstructionOpcode::nop)
			{
				mRegisters[mCurrentPixelIndex].IP++;
				next_code_address.write(mRegisters[mCurrentPixelIndex].IP);
				
				// Write out the FPU result (comb ops only)
				float result = fpu_result; // Caching only because I'm taking the address later to do a cast
				switch (mCurrentInstruction.Raw.op)
				{
				case InstructionOpcode::min:
				case InstructionOpcode::max:
				case InstructionOpcode::neg:
				case InstructionOpcode::abs:
					mRegisters[mCurrentPixelIndex].r[mCurrentInstruction.Arithmetic.r_out] = fpu_result;
					break;

				case InstructionOpcode::cmp_lt:
				case InstructionOpcode::cmp_le:
				case InstructionOpcode::cmp_gt:
				case InstructionOpcode::cmp_ge:
				case InstructionOpcode::cmp_eq:
				case InstructionOpcode::cmp_ne:
					mRegisters[mCurrentPixelIndex].CmpResult = *(int*)&result;
					break;

				default:
					break;
				}

				// Decode and execute mem ops
				switch (next_inst.Raw.op)
				{
				case InstructionOpcode::add:
				case InstructionOpcode::sub:
				case InstructionOpcode::mul:
				case InstructionOpcode::div:
				{
					fpu_clk_en = 1;
					mExecutingMultiCycle = true;

					if (next_inst.Arithmetic.use_constant)
					{
						fpu_datab = mOpConstants[next_inst.ArithmeticConst.constant_idx];
					}
					else
					{
						fpu_datab = mRegisters[mCurrentPixelIndex].r[next_inst.Arithmetic.r_b];
					}

					fpu_dataa = mRegisters[mCurrentPixelIndex].r[next_inst.Arithmetic.r_a];
					fpu_start = true;
					switch (next_inst.Raw.op)
					{
					case InstructionOpcode::add:
						fpu_n = 253;
						break;
					case InstructionOpcode::sub:
						fpu_n = 254;
						break;
					case InstructionOpcode::mul:
						fpu_n = 252;
						break;
					case InstructionOpcode::div:
						fpu_n = 255;
						break;
					}
					break;
				}
				case InstructionOpcode::sqrt:
				case InstructionOpcode::floor:
				case InstructionOpcode::round:
				case InstructionOpcode::min:
				case InstructionOpcode::max:
				case InstructionOpcode::neg:
				case InstructionOpcode::abs:
				{
					fpu_clk_en = 1;
					fpu_dataa = mRegisters[mCurrentPixelIndex].r[next_inst.Arithmetic.r_a];
					fpu_start = true;
					switch (next_inst.Raw.op)
					{
					case InstructionOpcode::sqrt:
						mExecutingMultiCycle = true;
						fpu_n = 251;
						break;
					case InstructionOpcode::floor:
						mExecutingMultiCycle = true;
						fpu_n = 249;
						break;
					case InstructionOpcode::round:
						mExecutingMultiCycle = true;
						fpu_n = 248;
						break;
					case InstructionOpcode::min:
						mExecutingMultiCycle = false;
						fpu_n = 233;
						break;
					case InstructionOpcode::max:
						mExecutingMultiCycle = false;
						fpu_n = 232;
						break;
					case InstructionOpcode::neg:
						mExecutingMultiCycle = false;
						fpu_n = 225;
						break;
					case InstructionOpcode::abs:
						mExecutingMultiCycle = false;
						fpu_n = 224;
						break;
					}
					break;
				}

				case InstructionOpcode::cmp_lt:
				case InstructionOpcode::cmp_le:
				case InstructionOpcode::cmp_gt:
				case InstructionOpcode::cmp_ge:
				case InstructionOpcode::cmp_eq:
				case InstructionOpcode::cmp_ne:
				{
					fpu_clk_en = 1;
					mExecutingMultiCycle = false;

					if (next_inst.Compare.use_constant)
					{
						fpu_datab = mOpConstants[next_inst.CompareConst.constant_idx];
					}
					else
					{
						fpu_datab = mRegisters[mCurrentPixelIndex].r[next_inst.Compare.r_b];
					}

					fpu_dataa = mRegisters[mCurrentPixelIndex].r[next_inst.Compare.r_a];
					fpu_start = true;
					switch (next_inst.Raw.op)
					{
					case InstructionOpcode::cmp_lt:
						fpu_n = 231;
						break;
					case InstructionOpcode::cmp_le:
						fpu_n = 230;
						break;
					case InstructionOpcode::cmp_gt:
						fpu_n = 229;
						break;
					case InstructionOpcode::cmp_ge:
						fpu_n = 228;
						break;
					case InstructionOpcode::cmp_eq:
						fpu_n = 227;
						break;
					case InstructionOpcode::cmp_ne:
						fpu_n = 226;
						break;
					}
					break;
				}

				case InstructionOpcode::copy:
					mRegisters[mCurrentPixelIndex].r[next_inst.CopySelectSet.r_out] = mRegisters[mCurrentPixelIndex].r[next_inst.CopySelectSet.r_a];
					fpu_clk_en = 0;
					break;
				case InstructionOpcode::select:
					mRegisters[mCurrentPixelIndex].r[next_inst.CopySelectSet.r_out] = mRegisters[mCurrentPixelIndex].CmpResult ? 
																						mRegisters[mCurrentPixelIndex].r[next_inst.CopySelectSet.r_a] :
																						mRegisters[mCurrentPixelIndex].r[next_inst.CopySelectSet.r_b];
					fpu_clk_en = 0;
					break;
				case InstructionOpcode::set:
					mRegisters[mCurrentPixelIndex].r[next_inst.CopySelectSet.r_out] = mOpConstants[next_inst.CopySelectSet.constant_idx];
					fpu_clk_en = 0;
					break;

				default:
					__debugbreak();
					break;
				}
			}

			mCurrentInstruction = next_inst;
		}
		else if (begin_signal.read())
		{
			mRegisters[mCurrentPixelIndex].IP = kFirstCodeAddress;
		}
	}
}