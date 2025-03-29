#include "Board.h"
#include "colours.h"
#include <vector>
#include <iostream>

Board::Board(int n){
    r = n;
    boardValues = std::vector<Colour::Colour>(r*r, Colour::none);
}

int Board::rank(){
    return r;
}

Colour::Colour Board::value(int n){
    return boardValues[n];
}

void Board::setValue(int n, Colour::Colour colour){
    boardValues[n] = colour;
}

Colour::Colour Board::valueRC(int row, int column){
    return boardValues[r*(row-1)+column-1];
}

void Board::setValueRC(int row, int column, Colour::Colour colour){
    boardValues[r*(row-1)+column-1] = colour;
}


void Board::print(){
    for (int i=1;i<=r;i++){
        for (int j=1;j<=r;j++){
            int a;
            if (valueRC(i,j)==Colour::black){
                a = 1;
            } else {
                a = 2;
            }
            if (valueRC(i,j)==Colour::none) a = 0;

            std::cout << a << " ";
        }
        std::cout << "\n";
    }
}
