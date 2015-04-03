//
//  Board.cpp
//  chess
//
//  Created by James Zaghini on 29/01/2015.
//
//

#include "Board.h"
#include "OgreFramework.h"

Board::Board()
{
    using namespace std;
    for(int i = 0; i < 8; i++) {
        for(int j = 1; j < 9; j++) {
            char letter = "abcdefgh"[i];
            string name = letter + to_string(j);
            float x = i * -2;
            float z = j * 2;
            cout << "placing square " << name << " at x: " << x << " at z: " << z;
            
            squares.push_back(new Square(name, x, 0, z));
        }
    }
}

Board::~Board()
{
    
}

Square * Board::getSquareWithCoordinates(std::string letter, std::string number)
{
    for(Square *square : squares) {
        if(square->name.compare(letter + number) == 0) {
            return square;
        }
    }
}


Square * Board::getSquarebyName(std::string name)
{
    for(Square *square : squares) {
        if(square->name.compare(name) == 0) {
            return square;
        }
    }
}

