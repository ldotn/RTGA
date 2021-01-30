#pragma once
constexpr int max2bits(int x)
{
	int bits = 0;

	while (x > 0)
	{
		x >>= 1;
		++bits;
	}

	return bits;
}

constexpr int kMaxCodeAddress = 255;
constexpr int kMaxMemoryAddress = 1024 * 1024 * 1024 - 1; // 1GB
constexpr int kMaxPixelChannelValue = 255;
constexpr int kMaxPixelCoord = 2047;

constexpr int kCodeAddressBits = max2bits(kMaxCodeAddress);
constexpr int kMemoryAddressBits = max2bits(kMaxMemoryAddress);
constexpr int kPixelChannelBits = max2bits(kMaxPixelChannelValue);
constexpr int kPixelCoordBits = max2bits(kMaxPixelCoord);

constexpr int kHaltCodeAddress  = 0; // Moving the IP to this causes execution to halt
constexpr int kFirstCodeAddress = 1; // First valid code address