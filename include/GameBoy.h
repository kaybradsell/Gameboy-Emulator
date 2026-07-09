#pragma once

#include <CPU.h>
#include <MemoryBus.h>
#include <string>

class GameBoy
{
public:
	GameBoy();
	void Cycle();

	void Pause(bool pause);
	void Step();

	const CPU& GetCPU() const;
	const PPU& GetPPU() const;
	const MemoryBus& GetMemoryBus() const;

	void SkipLoop();

	int speed = 1;

private:
	MemoryBus mem;
	CPU cpu;
	PPU ppu;
	Cartridge cart;
	std::string bootROMpath = "ROMS/dmg_boot.bin";
	bool paused = false;
};