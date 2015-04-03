//
//  King.h
//  chess
//
//  Created by James Zaghini on 3/04/2015.
//
//

#ifndef __chess__King__
#define __chess__King__

#include <stdio.h>
#include "Piece.h"

class King : public Piece
{
    public:
    King(std::string name, std::string meshName, Square *square, std::vector<Piece*>& pieces);
    ~King();
};

#endif /* defined(__chess__King__) */
