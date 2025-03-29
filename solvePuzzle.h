#pragma once
#include <vector>
#include "colours.h"
#include "Board.h"

bool randomBool();

bool isAllowed(Board &board, Colour::Colour colour, int pos);

bool isBoardSolved(Board &board);

bool solveBoard(Board &board, int currPos=0);

Board randomSolvedBoard(int n);

bool isBoardSolvable(Board board);

Board createPuzzle(int n);
