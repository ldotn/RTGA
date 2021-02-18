#pragma once

constexpr int max_to_bits(int x)
{
	int bits = 0;

	while (x > 0)
	{
		x >>= 1;
		++bits;
	}

	return bits;
}

constexpr int round_to_mult(int x, int base)
{
	return x + base - (x % base);
}


// [https://forums.accellera.org/topic/6084-vcd-dump-with-hierarchy-systemc-232/]
#define TRACE_VAR(F, X)    sc_trace(F, X, std::string(this->name()) + "." #X)
#define TRACE_ARR(F, X, I) sc_trace(F, X[I], (std::string(this->name()) + "." #X "(" + std::to_string(I) + ")").c_str());
#define TRACE_SIG(F, X)    sc_trace(F, X, X.name())