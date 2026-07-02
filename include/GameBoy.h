#pragma once

#include <CPU.h>
#include <MemoryBus.h>
#include <string>

class GameBoy
{
public:
	GameBoy();
	void Cycle();

	const CPU& GetCPU() const;
	const MemoryBus& GetMemoryBus() const;

	void SkipLoop();

private:
	MemoryBus mem;
	CPU cpu;
	std::string bootROMpath = "ROMS/dmg_boot.bin";
};