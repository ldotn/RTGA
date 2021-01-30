#include "ExecutionUnit.h"

void ExecutionUnit::Step()
{
	// initial
	mIP = kHaltCodeAddress;

	// always @(posedge clk)
	while (true)
	{
		bool begin = begin_signal.read();
		
		if (mIP != kHaltCodeAddress)
		{
			++mIP;
			next_code_address.write(mIP);
		}
		else
		{
			if (begin) mIP = kFirstCodeAddress;
		}

		cout << name() << " " << sc_time_stamp() << endl;
		cout << mIP << endl;
		wait();
	}
}

void FetchUnit::Step()
{
	// initial


	// always @(posedge clk)
	while (true)
	{
		cout << name() << " " << sc_time_stamp() << endl;
		wait();
	}
}

void DecodeUnit::Step()
{
	// initial


	// always @(posedge clk)
	while (true)
	{
		cout << name() << " " << sc_time_stamp() << endl;
		wait();
	}
}

void ALU::Step()
{
	// initial


	// always @(posedge clk)
	while (true)
	{
		cout << name() << " " << sc_time_stamp() << endl;
		wait();
	}
}