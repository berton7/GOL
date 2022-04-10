#include "Game.hpp"

int main(int argc, char *argv[])
{
	logging::Logger logger{"Main"};
	GameSettings gs{900, 600, 60, 40};
	if (argc > 1)
	{
		switch(argc)
		{
		case 3:
			gs.winSizeX = atoi(argv[1]);
			gs.winSizeY = atoi(argv[1]);
			gs.nCellsPerRow = atoi(argv[2]);
			gs.nCellsPerCol = atoi(argv[2]);
			break;
		case 5:
			gs.winSizeX = atoi(argv[1]);
			gs.winSizeY = atoi(argv[2]);
			gs.nCellsPerRow = atoi(argv[3]);
			gs.nCellsPerCol = atoi(argv[4]);
			break;
		default:
			logger.error("Wrong number of arguments.");
			return EXIT_FAILURE;
		}
	}
	if (gs.winSizeX < 100 || gs.winSizeY < 100 ||
		gs.nCellsPerRow < 5 || gs.nCellsPerCol < 5)
	{
		logger.error("Invalid arguments.");
		return EXIT_FAILURE;
	}
	Game game;
	game.init(gs);
	game.setFrameRate(30);
	while (game.isRunning())
	{
		game.loop();
	}
	return 0;
}