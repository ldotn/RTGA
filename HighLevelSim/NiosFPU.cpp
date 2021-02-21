#include "NiosFPU.h"
#include <cmath>

void NiosFPU::Step()
{
	// initial
	done.write(0);
	auto resetable_wait = [&](int Clocks)
	{
		for (int i = 0; i < Clocks; ++i)
		{
			if (reset.read())
				return false;
			wait();
		}

		return true;
	};

	// always @(posedge clk)
	while (true)
	{
		wait();
		done.write(0);

		if (clk_en.read())
		{
			switch (n.read())
			{
			case 255 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();

					if (!resetable_wait(15))
						break;
					assert(a == dataa.read()); // you must hold the values
					assert(b == datab.read()); // you must hold the values

					result.write(a / b);
					done.write(true);
				}
				break;
			case 254 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();

					if (!resetable_wait(4))
						break;
					assert(a == dataa.read()); // you must hold the values
					assert(b == datab.read()); // you must hold the values

					result.write(a - b);
					done.write(true);
				}
				break;
			case 253 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();

					if (!resetable_wait(4))
						break;
					assert(a == dataa.read()); // you must hold the values
					assert(b == datab.read()); // you must hold the values

					result.write(a + b);
					done.write(true);
				}
				break;
			case 252 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();

					if (!resetable_wait(3))
						break;
					assert(a == dataa.read()); // you must hold the values
					assert(b == datab.read()); // you must hold the values

					result.write(a * b);
					done.write(true);
				}
				break;
			case 251 & 31:
				if (start.read())
				{
					float a = dataa.read();

					if (!resetable_wait(7))
						break;
					assert(a == dataa.read()); // you must hold the values

					result.write(sqrtf(a));
					done.write(true);
				}
				break;
			case 249 & 31:
				if (start.read())
				{
					float a = dataa.read();

					if (!resetable_wait(1))
						break;
					assert(a == dataa.read()); // you must hold the values

					result.write(floorf(a));
					done.write(true);
				}
				break;
			case 248 & 31:
				if (start.read())
				{
					float a = dataa.read();

					if (!resetable_wait(1))
						break;
					assert(a == dataa.read()); // you must hold the values

					result.write(roundf(a));
					done.write(true);
				}
				break;
			case 233 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();
					result.write((a < b) ? a : b);
				}
				break;
			case 232 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();
					result.write((a < b) ? b : a);
				}
				break;
			case 231 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();
					int out = (a < b) ? 1 : 0;
					result.write(*((float*)&out));
				}
				break;
			case 230 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();
					int out = (a <= b) ? 1 : 0;
					result.write(*((float*)&out));
				}
				break;
			case 229 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();
					int out = (a > b) ? 1 : 0;
					result.write(*((float*)&out));
				}
				break;
			case 228 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();
					int out = (a >= b) ? 1 : 0;
					result.write(*((float*)&out));
				}
				break;
			case 227 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();
					int out = (a == b) ? 1 : 0;
					result.write(*((float*)&out));
				}
				break;
			case 226 & 31:
				if (start.read())
				{
					float a = dataa.read();
					float b = datab.read();
					int out = (a != b) ? 1 : 0;
					result.write(*((float*)&out));
				}
				break;
			case 225 & 31:
				if (start.read())
				{
					float a = dataa.read();
					result.write(-a);
				}
				break;
			case 224 & 31:
				if (start.read())
				{
					float a = dataa.read();
					result.write(fabsf(a));
				}
				break;
			default:
				__debugbreak();
				break;
			}

		}
	}
}