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

private:
	MemoryBus mem;
	CPU cpu;
	PPU ppu;
	std::string bootROMpath = "ROMS/dmg_boot.bin";
	bool paused = false;
};