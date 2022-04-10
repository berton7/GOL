#include "Cell.hpp"
#include "Logger.hpp"

Cell::Cell(int tx, int ty): x(tx), y(ty), mState{State::Dead}, mNextState{State::Dead}
{}

Cell::Cell(Cell &&other): x(other.x), y(other.y), mState(other.mState){}

Cell::~Cell(){}

void Cell::kill()
{
    mNextState = State::Dead;
}

void Cell::spawn()
{
    mNextState = State::Alive;
}

void Cell::toggle()
{
    mNextState = mState == State::Dead ? State::Alive : State::Dead;
}

void Cell::update()
{
    mState = mNextState;
    mNextState = mState;
}

bool Cell::isClicked(int tx, int ty, int cellW, int cellH) const
{
    return tx > x && tx < x+cellW && ty > y && ty < y + cellH;
}

void Cell::render(SDL_Renderer *renderer, int cellW, int cellH)
{
    SDL_Rect r;
    r.x = x ;
    r.y = y ;
    r.w = cellW;
    r.h = cellH;
    int color = mState == State::Alive ? 127 : 255;
    SDL_SetRenderDrawColor(renderer, color, color, color, 255);
    SDL_RenderFillRect(renderer, &r);
    SDL_RenderDrawRect(renderer, &r);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &r);
}