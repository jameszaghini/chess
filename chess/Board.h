//
//  Board.h
//  chess
//
//  Created by James Zaghini on 29/01/2015.
//
//

#ifndef __chess__Board__
#define __chess__Board__

#include <stdio.h>
#include "Square.h"

class Board
{
    
public:
    Board();
    ~Board();
    
    Square * getSquareWithCoordinates(std::string letter, std::string number);
    
    std::vector<Square*> squares;
};

#endif /* defined(__chess__Board__) */
