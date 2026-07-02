#include <MemoryBus.h>
#include <iostream>
#include <fstream>

MemoryBus::MemoryBus()
{
	std::cout << "[Notice]: Memory Bus started init...\n";

	cartridge = Cartridge();

	std::cout << "[Notice]: Memory Bus initialised.\n";
}

void MemoryBus::LoadBootROM(const std::string& path)
{
	std::cout << "[Notice]: Reading in Boot ROM from " << path << "...\n";

	std::ifstream file(path, std::ios::binary);

	if (!file)
		throw std::runtime_error("[ERROR]: Cannot open boot ROM " + path);

	file.read(reinterpret_cast<char*>(bootROM.data()), bootROM.size());

	if (file.gcount() != bootROM.size())
		throw std::runtime_error("[ERROR]: Boot ROM did not match expected size.");

	std::cout << "[Notice]: Successfully read in Boot ROM.\n";
}

uint8_t MemoryBus::GetByte(uint16_t addr)
{
	if (addr < 0x00FF) // fix this fucking shit later
		return bootROM[addr];
	else if (addr < 0x8000)
		return cartridge.GetByte(addr);
	else if (addr >= 0xFF80 && addr < 0xFFFE)
		return HRAM[addr - 0xFF80];
}

void MemoryBus::WriteByte(uint16_t addr, uint8_t data)
{
	//std::cout << "Totally wrote " << std::hex << std::uppercase << (int)data << " into addr " << (int)addr << "\n";
	if (addr >= 0xFF80 && addr < 0xFFFE)
		HRAM[addr - 0xFF80] = data;
}

const std::array<uint8_t, 256>& MemoryBus::GetBootROM() const
{
	return bootROM;
}