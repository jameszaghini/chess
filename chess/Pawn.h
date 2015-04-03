//
//  Pawn.h
//  chess
//
//  Created by James Zaghini on 3/04/2015.
//
//

#ifndef __chess__Pawn__
#define __chess__Pawn__

#include <stdio.h>
#include "Piece.h"

class Pawn : public Piece
{
    public:
    Pawn(std::string name, std::string meshName, Square *square, std::vector<Piece*>& pieces);
    ~Pawn();
};

#endif /* defined(__chess__Pawn__) */
