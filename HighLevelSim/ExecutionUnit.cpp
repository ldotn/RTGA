#include "ExecutionUnit.h"

void ExecutionUnit::StepNegEdge()
{
	while (true)
	{
		wait();
		if (!mExecutingMultiCycle)
		{
			if (mStartExecutingMultiCycle && mCurrentInstruction[mCurrentPixelIndex].IsMulticycle())
			{
				fpu_start = 1;
				mExecutingMultiCycle = true;
			}
		}
		else if (fpu_done)
		{
			fpu_start = 0;
			mExecutingMultiCycle = false;
		}
	}
}

void ExecutionUnit::Step()
{
	// initial
	mCurrentPixelIndex = 0;
	mRegisters[0].IP = kHaltCodeAddress;
	mRegisters[1].IP = kHaltCodeAddress;
	mRegisters[2].IP = kHaltCodeAddress;
	mRegisters[3].IP = kHaltCodeAddress;
	next_code_address[0] = kFirstCodeAddress;
	next_code_address[1] = kFirstCodeAddress;
	next_code_address[2] = kFirstCodeAddress;
	next_code_address[3] = kFirstCodeAddress;
	mCurrentInstruction[0].Raw.op = InstructionOpcode::nop;
	mCurrentInstruction[1].Raw.op = InstructionOpcode::nop;
	mCurrentInstruction[2].Raw.op = InstructionOpcode::nop;
	mCurrentInstruction[3].Raw.op = InstructionOpcode::nop;
	fpu_clk_en = 0;
	mExecutingMultiCycle = false;
	mStartExecutingMultiCycle = false;
	mFinished[0] = true;
	mFinished[1] = true;
	mFinished[2] = true;
	mFinished[3] = true;

	// always @(posedge clk)
	while (true)
	{
		wait();

		cout << "\n Pixel :" << mCurrentPixelIndex
			<< " IP : " << mRegisters[mCurrentPixelIndex].IP
			<< " mExecutingMultiCycle : " << mExecutingMultiCycle
			<< "\n fpu_dataa : " << fpu_dataa
			<< " fpu_datab : " << fpu_datab
			<< " fpu_result : " << fpu_result
			<< " fpu_done : " << fpu_done
			<< "\n r[0] : " << mRegisters[mCurrentPixelIndex].Bank.Physical[0]
			<< " r[1] : " << mRegisters[mCurrentPixelIndex].Bank.Physical[1]
			<< " r[2] : " << mRegisters[mCurrentPixelIndex].Bank.Physical[2]
			<< " r[3] : " << mRegisters[mCurrentPixelIndex].Bank.Physical[3]
			<< endl;

		// On verilog this is just a wire
		mRegisters[0].Bank.Logical.HitData = trace_results[0];
		mRegisters[1].Bank.Logical.HitData = trace_results[1];
		mRegisters[2].Bank.Logical.HitData = trace_results[2];
		mRegisters[3].Bank.Logical.HitData = trace_results[3];

		if (begin_signal.read())
		{
			mFinished[0] = false;
			mFinished[1] = false;
			mFinished[2] = false;
			mFinished[3] = false;

			// Load this values as the program may operate over them
			mRegisters[0].Bank.Logical.QuadXY[0] = from_uint24(quad_xy.read().x);
			mRegisters[0].Bank.Logical.QuadXY[1] = from_uint24(quad_xy.read().y);
			mRegisters[1].Bank.Logical.QuadXY[0] = from_uint24(quad_xy.read().x);
			mRegisters[1].Bank.Logical.QuadXY[1] = from_uint24(quad_xy.read().y);
			mRegisters[2].Bank.Logical.QuadXY[0] = from_uint24(quad_xy.read().x);
			mRegisters[2].Bank.Logical.QuadXY[1] = from_uint24(quad_xy.read().y);
			mRegisters[3].Bank.Logical.QuadXY[0] = from_uint24(quad_xy.read().x);
			mRegisters[3].Bank.Logical.QuadXY[1] = from_uint24(quad_xy.read().y);
		}

		if (!mFinished[mCurrentPixelIndex])
		{
			bool advance_ip = true;

			// Check if we can continue execution
			if (mCurrentInstruction[mCurrentPixelIndex].IsMulticycle() && mExecutingMultiCycle)
			{
				advance_ip = false;
			}
			else if (mCurrentInstruction[mCurrentPixelIndex].IsAsync())
			{
				// If the request queue is full, then we can't advance until the request has been enqueued 
				// Technically you could switch to another pixels, but the logic gets a lot more complex as you can't touch any of the registers of the unsent request
				if ((mCurrentInstruction[mCurrentPixelIndex].Raw.op == InstructionOpcode::request_sample && is_memory_queue_full) ||
					(mCurrentInstruction[mCurrentPixelIndex].Raw.op == InstructionOpcode::trace && is_trace_queue_full))
				{
					advance_ip = false;
				}
				else
				{
					if (async_request_finished[0])
						mCurrentPixelIndex = 0;
					else if (async_request_finished[1])
						mCurrentPixelIndex = 1;
					else if (async_request_finished[2])
						mCurrentPixelIndex = 2;
					else if (async_request_finished[3])
						mCurrentPixelIndex = 3;
					else
						advance_ip = false;

					// TODO : It might make sense to pipeline this
					mRegisters[0].Bank.Logical.CurrentPixelIdx = from_uint24(mCurrentPixelIndex);
					mRegisters[1].Bank.Logical.CurrentPixelIdx = from_uint24(mCurrentPixelIndex);
					mRegisters[2].Bank.Logical.CurrentPixelIdx = from_uint24(mCurrentPixelIndex);
					mRegisters[3].Bank.Logical.CurrentPixelIdx = from_uint24(mCurrentPixelIndex);
				}
			}
			
			// Only needed when the prev op was a mem read, but just clean it always
			request_memory_read = false;

			// Advance to the next instruction if not waiting
			if (advance_ip)
			{
				// Write out the result of the prev instruction
				if (mCurrentInstruction[mCurrentPixelIndex].Raw.op == InstructionOpcode::cb_load)
				{
					mRegisters[mCurrentPixelIndex].Bank.Physical[mCurrentInstruction[mCurrentPixelIndex].ConstantBuffer.r_out] = cb_data.read();
				}
				else if (mCurrentInstruction[mCurrentPixelIndex].IsFPU())
				{
					if (mCurrentInstruction[mCurrentPixelIndex].IsCmp())
					{
						// This cast is only needed on c++, on verilog it would just be a direct assignment 
						float result = fpu_result;
						mRegisters[mCurrentPixelIndex].CmpResult = *(int*)&result;
					}
					else
					{
						mRegisters[mCurrentPixelIndex].Bank.Physical[mCurrentInstruction[mCurrentPixelIndex].Arithmetic.r_out] = fpu_result;
					}
				}
				else if (mCurrentInstruction[mCurrentPixelIndex].Raw.op == InstructionOpcode::wait_sample)
				{
					switch (mCurrentInstruction[mCurrentPixelIndex].WaitSample.format)
					{
					case TextureFormat::f32:
						mRegisters[mCurrentPixelIndex].Bank.Logical.TexSampleResult[0] = *(float*)&memory_read_data.read();
						mRegisters[mCurrentPixelIndex].Bank.Logical.TexSampleResult[1] = 0.0f;
						mRegisters[mCurrentPixelIndex].Bank.Logical.TexSampleResult[2] = 0.0f;
						break;
					default:
						// TODO : Implement
						__debugbreak();
						break;
					}
				}
				else if (mCurrentInstruction[mCurrentPixelIndex].Raw.op == InstructionOpcode::trace)
				{
					request_trace[mCurrentPixelIndex] = false;
					mRegisters[mCurrentPixelIndex].Bank.Logical.HitData = trace_results[0].read();
				}

				// Kick off processing of the next instruction
				switch (next_instruction[mCurrentPixelIndex].read().Raw.op)
				{
				case InstructionOpcode::add:
				case InstructionOpcode::sub:
				case InstructionOpcode::mul:
				case InstructionOpcode::div:
				{
					mStartExecutingMultiCycle = true;
					fpu_clk_en = 1;

					if (next_instruction[mCurrentPixelIndex].read().Arithmetic.use_constant)
					{
						fpu_datab = mOpConstants[next_instruction[mCurrentPixelIndex].read().ArithmeticConst.constant_idx];
					}
					else
					{
						fpu_datab = mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().Arithmetic.r_b];
					}

					fpu_dataa = mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().Arithmetic.r_a];
					switch (next_instruction[mCurrentPixelIndex].read().Raw.op)
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
					fpu_dataa = mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().Arithmetic.r_a];
					switch (next_instruction[mCurrentPixelIndex].read().Raw.op)
					{
					case InstructionOpcode::sqrt:
						mStartExecutingMultiCycle = true;
						fpu_n = 251;
						break;
					case InstructionOpcode::floor:
						mStartExecutingMultiCycle = true;
						fpu_n = 249;
						break;
					case InstructionOpcode::round:
						mStartExecutingMultiCycle = true;
						fpu_n = 248;
						break;
					case InstructionOpcode::min:
						mStartExecutingMultiCycle = false;
						fpu_n = 233;
						break;
					case InstructionOpcode::max:
						mStartExecutingMultiCycle = false;
						fpu_n = 232;
						break;
					case InstructionOpcode::neg:
						mStartExecutingMultiCycle = false;
						fpu_n = 225;
						break;
					case InstructionOpcode::abs:
						mStartExecutingMultiCycle = false;
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
					mStartExecutingMultiCycle = false;
					fpu_clk_en = 1;

					if (next_instruction[mCurrentPixelIndex].read().Compare.use_constant)
					{
						fpu_datab = mOpConstants[next_instruction[mCurrentPixelIndex].read().CompareConst.constant_idx];
					}
					else
					{
						fpu_datab = mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().Compare.r_b];
					}

					fpu_dataa = mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().Compare.r_a];
					switch (next_instruction[mCurrentPixelIndex].read().Raw.op)
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

				case InstructionOpcode::cjmp:
					if (!next_instruction[mCurrentPixelIndex].read().Jump.is_conditional || mRegisters[mCurrentPixelIndex].CmpResult)
						mRegisters[mCurrentPixelIndex].IP = next_instruction[mCurrentPixelIndex].read().Jump.address;
					break;

				case InstructionOpcode::copy:
					mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().CopySelectSet.r_out]
						= mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().CopySelectSet.r_a];
					fpu_clk_en = 0;
					mStartExecutingMultiCycle = false;
					break;
				case InstructionOpcode::select:
					mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().CopySelectSet.r_out] = mRegisters[mCurrentPixelIndex].CmpResult ?
						mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().CopySelectSet.r_a] :
						mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().CopySelectSet.r_b];
					fpu_clk_en = 0;
					mStartExecutingMultiCycle = false;
					break;
				case InstructionOpcode::set:
					mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().CopySelectSet.r_out]
						= mOpConstants[next_instruction[mCurrentPixelIndex].read().CopySelectSet.constant_idx];
					fpu_clk_en = 0;
					mStartExecutingMultiCycle = false;
					break;

				case InstructionOpcode::request_sample:
				{
					sc_uint<24> tex_base = as_uint24(mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().Sample.r_base]);
					sc_uint<24> tex_offset = as_uint24(mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().Sample.r_offset]);

					MemoryAddress sample_address = tex_base;
					sample_address <<= 2;
					sample_address += tex_offset;

					memory_request_address.write(sample_address);
					request_memory_read.write(true);

					break;
				}

				case InstructionOpcode::wait_sample:
					break; // There's nothing to do here

				case InstructionOpcode::trace:
					request_trace[mCurrentPixelIndex] = true;
					break;

				case InstructionOpcode::cb_load:
					cb_read_index = as_uint24(mRegisters[mCurrentPixelIndex].Bank.Physical[next_instruction[mCurrentPixelIndex].read().ConstantBuffer.r_index]);
					break;

				case InstructionOpcode::finish:
				{
					// TODO : I can probably spread this over multiple clocks, there's no need to have this on one clock
					// This assumes that the values here have been rounded down to integers
					Pixel out_pix;
					out_pix.r = as_uint24(mRegisters[mCurrentPixelIndex].Bank.Logical.PixelOutput[0]);
					out_pix.g = as_uint24(mRegisters[mCurrentPixelIndex].Bank.Logical.PixelOutput[1]);
					out_pix.b = as_uint24(mRegisters[mCurrentPixelIndex].Bank.Logical.PixelOutput[2]);
					out_quad[mCurrentPixelIndex] = out_pix;

					if (!mFinished[0])
						mCurrentPixelIndex = 0;
					else if (!mFinished[1])
						mCurrentPixelIndex = 1;
					else if (!mFinished[2])
						mCurrentPixelIndex = 2;
					else if (!mFinished[3])
						mCurrentPixelIndex = 3;
					else
					{
						// Finished with the quad, signal and exit
						finished_signal = true;

						mCurrentPixelIndex = 0;
						mRegisters[0].IP = kHaltCodeAddress;
						mRegisters[1].IP = kHaltCodeAddress;
						mRegisters[2].IP = kHaltCodeAddress;
						mRegisters[3].IP = kHaltCodeAddress;
						next_code_address[0] = kFirstCodeAddress;
						next_code_address[1] = kFirstCodeAddress;
						next_code_address[2] = kFirstCodeAddress;
						next_code_address[3] = kFirstCodeAddress;
						mCurrentInstruction[0].Raw.op = InstructionOpcode::nop;
						mCurrentInstruction[1].Raw.op = InstructionOpcode::nop;
						mCurrentInstruction[2].Raw.op = InstructionOpcode::nop;
						mCurrentInstruction[3].Raw.op = InstructionOpcode::nop;
						fpu_clk_en = 0;
						mExecutingMultiCycle = false;
						mStartExecutingMultiCycle = false;
					}
					break;
				}

				default:
					__debugbreak();
					break;
				}

				// Update current instruction
				mCurrentInstruction[mCurrentPixelIndex] = next_instruction[mCurrentPixelIndex];
				next_code_address[mCurrentPixelIndex] = mRegisters[mCurrentPixelIndex].IP;
			}
		}
	}
}