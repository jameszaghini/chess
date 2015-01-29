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
    for(int i = 1; i < 9; i++) {
        for(int j = 0; j < 7; j++) {
            char letter = "ABCDEFG"[j];
            std::string name = letter + std::to_string(i);
            new Square(name, i*2,0,j*2);
        }
    }
}

Board::~Board()
{
    
}

