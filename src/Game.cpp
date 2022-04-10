#include "Game.hpp"
//#define ENABLE_TICKS

Game::Game(): mLogger("Game")
{
	mLogger.setLevel(logging::Level::Debug);
	mLogger.setOption(logging::Options::ENABLE_COLOR, true);
	setFrameRate(60);
}

Game::~Game()
{
}

void Game::setFrameRate(int fps)
{
	mMaxLoopTimeForFPS = 1000 / fps;
}

void Game::gameSettingsToData(const GameSettings &settings)
{
	mData.winSizeX = settings.winSizeX;
	mData.winSizeY = settings.winSizeY;
	mData.cellSizeX = (float)settings.winSizeX / (float)settings.nCellsPerRow;
	mData.cellSizeXIt = std::round(mData.cellSizeX);
	mData.cellSizeY = (float)settings.winSizeY / (float)settings.nCellsPerCol;
	mData.cellSizeYIt = std::round(mData.cellSizeY);
	mData.nCells = settings.nCellsPerRow * settings.nCellsPerCol;
	mData.nCellsPerRow = settings.nCellsPerRow;
	mData.nCellsPerCol = settings.nCellsPerCol;
}

int Game::init(const GameSettings &settings)
{
	gameSettingsToData(settings);

	int rc;
	if ((rc=SDL_Init(SDL_INIT_VIDEO)))
	{
		mLogger.error("Failed to init SDL: ", SDL_GetError());
		return rc;
	}
	mLogger.info("Successfully initialized SDL.", "");

	mWindow = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mData.winSizeX, mData.winSizeY, SDL_WINDOW_SHOWN);
	if (!mWindow)
	{
		mLogger.error("Failed to initialize window: ", SDL_GetError());
		return 1;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
	if (!mRenderer)
	{
		mLogger.error("Failed to initialize renderer: ", SDL_GetError());
		return 2;
	}

	mLogger.debug("Game is starting");
	mCells.reserve(mData.nCellsPerRow);
	resetGame();
	pause();
	setRunning(true);
	return 0;
}

void Game::resetGame()
{
	mCells.clear();
	for (auto &cellCol: mCells)
	{
		cellCol.clear();
	}
	
	std::vector<int> cellPosX;
	cellPosX.reserve(mData.nCellsPerRow);
	std::vector<int> cellPosY;
	cellPosY.reserve(mData.nCellsPerCol);

	// pre-compute cells positions
	for (int i=0; i<mData.nCellsPerRow; ++i)
	{
		int tmp = std::round(i * mData.cellSizeX);
		cellPosX.emplace_back(tmp);	
	}
	for (int i=0; i<mData.nCellsPerCol; ++i)
	{
		int tmp = std::round(i * mData.cellSizeY);
		cellPosY.emplace_back(tmp);	
	}
	for (int i=0; i<mData.nCellsPerRow; ++i)
	{
		std::vector<Cell> cellsCol;
		cellsCol.reserve(mData.nCellsPerCol);
		for (int j=0; j<mData.nCellsPerCol; ++j)
		{
			const auto &c=cellsCol.emplace_back(cellPosX[i], cellPosY[j]);
		}
		mCells.emplace_back(std::move(cellsCol));
	}
}

void Game::end()
{
	if (mWindow)
		SDL_DestroyWindow(mWindow);
	if (mRenderer)
		SDL_DestroyRenderer(mRenderer);
	SDL_Quit();
}

void Game::tick()
{
	handleEvents();
	if (!isPaused())
	{
		for (int x=0; x<mData.nCellsPerRow; ++x)
		{
			for (int y=0; y<mData.nCellsPerCol; ++y)
			{
				auto &cell = mCells[x][y];
				int aliveNeighbors = 0;
				for (int i=x-1; i<x+2; ++i)
				{
					if (i >= 0 && i < mData.nCellsPerRow)
					{
						for (int j=y-1; j<y+2; ++j)
						{
							if (j >= 0 && j < mData.nCellsPerCol && (j!=y || i!=x))
							{
								if (mCells[i][j].isAlive())
								{
									++aliveNeighbors;
								}
							}
						}
					}
				}

				if (cell.isAlive() && (aliveNeighbors < 2 || aliveNeighbors > 3))
				{
					cell.kill();
				}
				else if (cell.isDead() && aliveNeighbors == 3)
				{
					cell.spawn();
				}
			}
		}
	}
	for (auto &cellsCol: mCells)
	{
		for (auto &cell: cellsCol)
			cell.update();
	}
}

void Game::handleEvents()
{
	for (const auto &event: mEvents)
	{
		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				int mX, mY;
				SDL_GetMouseState(&mX, &mY);
				mLogger.debug("Mouse: ", mX, " ", mY);
				for (auto &cellsCol: mCells)
				{
					bool _break{false};
					for (auto &cell: cellsCol){
						if (cell.isClicked(mX, mY, mData.cellSizeXIt, mData.cellSizeYIt))
						{
							cell.toggle();
							mLogger.debug("Cell clicked: ", cell.getX(), " ", cell.getY());
							_break = true;
							break;
						}
					}
					if (_break)
						break;
				}
				break;
			}
			break;
		}
	}
	mEvents.clear();
}

void Game::handleSysEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			scheduleQuit();
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
				scheduleQuit();
				break;
			case SDLK_SPACE:
				togglePause();
				mLogger.debug("Paused: ", isPaused());
				break;
			case SDLK_r:
				resetGame();
				break;
			default:
				mEvents.emplace_back(std::move(event));
				break;
			}
			break;
		default:
			mEvents.emplace_back(std::move(event));
			break;
		}
	}
}

void Game::render()
{
	if (SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255))
	{
		mLogger.error("Failed to set renderer draw color: ", SDL_GetError());
	}
	if (SDL_RenderClear(mRenderer))
	{
		mLogger.error("Failed to clear renderer: ", SDL_GetError());
	}
	for (auto &cellsCol: mCells)
	{
		for (auto &cell: cellsCol)
		{
			cell.render(mRenderer, mData.cellSizeXIt, mData.cellSizeYIt);
		}
	}
	SDL_RenderPresent(mRenderer);
}

void Game::loop()
{
	const Uint64 begin = SDL_GetTicks64();
	handleSysEvents();

#ifdef ENABLE_TICKS
	bool inTick=false;
	if (!(mFrames%4))
	{
		inTick =true;
		tick();
	}
#else
	tick();
#endif

	render();
	++mFrames;
	mDeltaTime = SDL_GetTicks64() - begin;
	if (mMaxLoopTimeForFPS > mDeltaTime)
		SDL_Delay(mMaxLoopTimeForFPS - mDeltaTime);
	else

#ifdef ENABLE_TICKS
		mLogger.warn("Max loop time exceeded: frame: ", mFrames, ", delta: ", mDeltaTime, ", in tick: ", inTick);
#else
		mLogger.warn("Max loop time exceeded: frame: ", mFrames, ", delta: ", mDeltaTime);
#endif

}

void Game::scheduleQuit()
{
	mLogger.debug("Scheduled Quit.");
	setRunning(false);
};
