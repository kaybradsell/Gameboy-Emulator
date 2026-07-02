#include <GameBoy.h>
#include <iostream>

GameBoy::GameBoy() : mem(), cpu(mem)
{
	mem.LoadBootROM(bootROMpath);
}

void GameBoy::Cycle()
{
	// idk if i should do the proper timings here but
	// for now who cares?
	cpu.Step();
}

const CPU& GameBoy::GetCPU() const
{
	return cpu;
}

const MemoryBus& GameBoy::GetMemoryBus() const
{
	return mem;
}

void GameBoy::SkipLoop()
{
	cpu.SkipLoop();
}