#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include <string>

// has it's own mem banks that it can swap out at any time
// and shit so it's its own class.

class Cartridge
{
public:
	void LoadROM(const std::string& filePath);

	uint8_t GetByte(uint16_t addr);
	void WriteByte(uint16_t addr, uint8_t data);

private:
	// has two ROM banks, bank 00, and then bank 01-NN, so 01 can be interchangeable
	// that's a later issue :P
	std::vector<uint8_t> rom;
};