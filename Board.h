#pragma once
#include <vector>
#include "colours.h"
class Board{
    int r{};
    std::vector<Colour::Colour> boardValues{};

    public:
    Board(int r);
    int rank();
    Colour::Colour value(int n);
    void setValue(int n, Colour::Colour Colour);
    Colour::Colour valueRC(int row, int column);
    void setValueRC(int row, int column, Colour::Colour Colour);
    void print();
};
