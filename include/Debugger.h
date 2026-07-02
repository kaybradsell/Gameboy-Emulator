#pragma once

#include <GameBoy.h>
#include <raylib.h>
#include <rlImGui.h>
#include <imGui.h>

class Debugger
{
public:
	Debugger(GameBoy* gb);

	bool Running();

	void Display();

private:
	GameBoy* gb = nullptr;
	static const int HISTORY_SIZE = 100;
	std::array<std::string, HISTORY_SIZE> history;
	int historyIndex = 0;

	void DrawRegisters();
	void DrawControls();
	void DrawBootROM();
	void DrawHistory();
};