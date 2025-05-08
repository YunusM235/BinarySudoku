#include "solvePuzzle.h"
#include "colours.h"
#include <random>
#include "Board.h"
#include <iostream>
#include <algorithm>
#include <iostream>

bool randomBool(){
    static std::mt19937 mt{ std::random_device{}() };
    static std::uniform_int_distribution gen{0,1};
    return gen(mt);
}

//to do: change copied code
// decides if placing a certain Colour on a given square violates the rules
bool isAllowed(Board& board, Colour::Colour colour, int pos){
    int row = pos/board.rank()+1;
    int column = pos%board.rank()+1;

    // checks if there would be three consecutive squares with the same Colour
    if (row!=1 && row!=board.rank()){
        if (board.valueRC(row-1, column) == colour && board.valueRC(row+1, column) == colour) return false;
    }
    if (column!=1 && column!=board.rank()){
        if (board.valueRC(row, column-1) == colour && board.valueRC(row, column+1) == colour) return false;
    }
    if (row > 2 && board.valueRC(row-2, column) == colour && board.valueRC(row-1, column) == colour) return false;
    if (row< board.rank()-1 && board.valueRC(row+2, column) == colour && board.valueRC(row+1, column) == colour) return false;
    if (column > 2 && board.valueRC(row, column-2) == colour && board.valueRC(row, column-1) == colour) return false;
    if (column < board.rank()-1 && board.valueRC(row, column+2) == colour && board.valueRC(row, column+1) == colour) return false;

    //checks if more than half of the row/column would have the same colour
    int count{};
    for (int i=1;i<=board.rank();i++){
        if (i==row) count++;
        else if (board.valueRC(i, column) == colour) count++;
    }
    if (count > board.rank()/2) return false;
    count = 0;
    for (int i=1;i<=board.rank();i++){
        if (i==column) count++;
        else if (board.valueRC(row, i) == colour) count++;
    }
    if (count > board.rank()/2) return false;

    //checks if each row/column is different
    bool columnFull = true;
    bool rowFull = true;
    for (int i=1;i<=board.rank();i++){
        if (i==row) continue;
        if (board.valueRC(i,column)==Colour::none){ 
            columnFull = false;
            break;
        }
    }
    for (int i=1;i<=board.rank();i++){
        if (i==column) continue;
        if (board.valueRC(row,i)==Colour::none){ 
            rowFull = false;
            break;
        }
    }
    bool isEqual;
    if (columnFull){
        for (int i=1;i<=board.rank();i++){
            if (i==column) continue;
            isEqual = true;
            for (int j=1;j<=board.rank();j++){
                if (j==row){
                    if ( board.valueRC(j, i)!=colour){
                        isEqual = false;
                        break;
                    }
                } else if (board.valueRC(j, i)!=board.valueRC(j,column)) {
                    isEqual = false;
                    break;
                }
            }
            if (isEqual) return false;
        }
    }
    if (rowFull){
        for (int i=1;i<=board.rank();i++){
            if (i==row) continue;
            isEqual = true;
            for (int j=1;j<=board.rank();j++){
                if (j==column){
                    if (board.valueRC(i, j)!=colour) {
                        isEqual = false;
                        break;
                    }
                } else if (board.valueRC(i, j)!=board.valueRC(row,j)) {
                    isEqual = false;
                    break;
                }
            }
            if (isEqual) return false;
        }
    }
    return true;
}

bool isBoardSolved(Board &board){
    for (int i=0;i<board.rank()*board.rank();i++){
        if (board.value(i)==Colour::none || !(isAllowed(board, board.value(i), i))) return false;
    }
    return true;
}

bool solveBoard(Board& board, int currPos){

    // if board is filled
    if (currPos == (board.rank()*board.rank())) {
        return true;
    }

    // if the current cell is already Coloured
    if (board.value(currPos) != Colour::none){
        return solveBoard(board, currPos+1);
    }

    // randomly choosing Colours
    bool r = randomBool();
    Colour::Colour firstColour = r ? Colour::black : Colour::white;
    Colour::Colour secondColour = r ? Colour::white : Colour::black;

    if (isAllowed(board, firstColour, currPos)){
        Colour::Colour oldColour = board.value(currPos);
        board.setValue(currPos, firstColour);
        if (solveBoard(board, currPos+1)) return true;
        board.setValue(currPos, oldColour);
    }

    if (isAllowed(board, secondColour, currPos)){
        Colour::Colour oldColour = board.value(currPos);
        board.setValue(currPos, secondColour);
        if (solveBoard(board, currPos+1)) return true;
        board.setValue(currPos, oldColour);
    }

    return false;
}

// Returns a random solved board of size n x n
// n has to be even
Board randomSolvedBoard(int n){
    Board board(n);
    solveBoard(board);
    return board;
}

// Board has to be of size n x n where n is an even integer
bool isBoardSolvable(Board board){
    return solveBoard(board);
}

Board createPuzzle(int n){
    Board board{randomSolvedBoard(n)};

    std::vector<int> squares{};
    for (int i=0;i<n*n;i++) squares.push_back(i);

    static std::mt19937 mt{ std::random_device{}() };
    std::shuffle(squares.begin(), squares.end(), mt);


    //remove random squares such that the solution remains unique
    Colour::Colour oldColour;
    int a;
    int count{};
    board.setValue(squares[0], Colour::none);

    for (int i=0;i<n*n;i++){
        if (board.value(squares[i]) == Colour::none) continue;

        oldColour = board.value(squares[i]);

        Colour::Colour newColour = (oldColour==Colour::black) ? Colour::white : Colour::black;
        board.setValue(squares[i], newColour);
        
        
        if (isAllowed(board, newColour, squares[i]) && isBoardSolvable(board)){
            board.setValue(squares[i], oldColour);
        } else {
            board.setValue(squares[i], Colour::none);
            count++;
        }

    }   

    
    return board;
}
