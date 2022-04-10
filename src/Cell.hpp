#include <SDL2/SDL.h>

class Cell
{
public:
    enum class State
    {
        Dead, Alive
    };

private:
    int x, y;
    State mState;
    State mNextState;

public:
    Cell(int x, int y);
    Cell(Cell &&);
    ~Cell();

    void kill();
    void spawn();
    void toggle();
    void update();
    bool isClicked(int x, int y, int cellW, int cellH) const;
    void render(SDL_Renderer *renderer, int cellW, int CellH);
    int getX() const { return x; }
    int getY() const { return y; }
    bool isAlive() const { return getState() == State::Alive; }
    bool isDead() const { return getState() == State::Dead; }
    State getState() const { return mState; }
    //State getNextState() const { return mNextState; }
};