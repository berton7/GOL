#pragma once
#include <SDL2/SDL.h>
#include "Logger.hpp"
#include "Cell.hpp"
#include <memory>
#include <vector>
#include <bitset>

typedef struct
{
	int winSizeX;
	int winSizeY;
	int nCellsPerRow;
	int nCellsPerCol;
} GameSettings;

typedef struct
{
	// actual cell size
	float cellSizeX;
	float cellSizeY;
	// cell size rounded to closest int
	int cellSizeXIt;
	int cellSizeYIt;
	// window size
	int winSizeX;
	int winSizeY;
	// number of cells
	int nCells;
	// number of cells per row/column
	int nCellsPerRow;
	// number of cells per row/column
	int nCellsPerCol;
} GameData;

class Game
{
public:
	enum State
	{
		running = 0,
		paused
	};

private:
	SDL_Window *mWindow{nullptr};
	SDL_Renderer *mRenderer{nullptr};
	logging::Logger mLogger;
	Uint64 mDeltaTime{0};
	Uint64 mMaxLoopTimeForFPS{0};
	int mFrames{0};
	std::vector<SDL_Event> mEvents;

	std::bitset<2> mGameState;
	GameData mData;

	std::vector<std::vector<Cell>> mCells;
	int aliveCells {0}, deadCells {0};
	Cell::State toBeRendered;

	void gameSettingsToData(const GameSettings &settings);
	void handleSysEvents();

public:
	Game();
	~Game();

	void setFrameRate(int fps);

	int init(const GameSettings &settings);
	void resetGame();
	void handleEvents();
	void tick();
	void render();
	void loop();
	void end();

	void setRunning(bool isRunning) { mGameState[State::running] = isRunning; }
	void pause() { mGameState.set(State::paused); }
	void resume() { mGameState.reset(State::paused); }
	void togglePause() { mGameState.flip(State::paused); }
	bool isRunning() const { return mGameState[State::running]; }
	bool isPaused() const { return mGameState[State::paused]; }
	void scheduleQuit();
};
