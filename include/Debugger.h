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

	// temp for now
	static constexpr Color colours[4] = { WHITE, LIGHTGRAY, GRAY, DARKGRAY };
	static constexpr int TILESIZE = 8;
	static constexpr int TILEMAXX = 24;
	static constexpr int TILEMAXY = 16;

	Color vbuffer[TILESIZE * TILESIZE * TILEMAXX * TILEMAXY];
	Texture2D tileTexture;
	Image tileImage;

	void DrawRegisters();
	void DrawControls();
	void DrawMemory();
	void DrawHistory();
	void DrawTileViewer();

	void DrawTile(const PPU::Tile& tile, int startX, int startY);
};