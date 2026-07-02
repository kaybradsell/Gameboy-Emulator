#pragma once

#include <memory>
#include <array>
#include <cstdint>

// has it's own mem banks that it can swap out at any time
// and shit so it's its own class.

class Cartridge
{
public:
	Cartridge();
	uint8_t GetByte(uint16_t addr);
	void WriteByte(uint16_t addr, uint8_t data);

private:
	// has two ROM banks, bank 00, and then bank 01-NN, so 01 can be interchangeable
	// that's a later issue :P
	std::array<uint8_t, 0x4000> bank0{};
	std::array<uint8_t, 0x4000> bank1{};	// switchable later.
};