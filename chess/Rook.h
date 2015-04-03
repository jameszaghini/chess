//
//  Rook.h
//  chess
//
//  Created by James Zaghini on 3/04/2015.
//
//

#ifndef __chess__Rook__
#define __chess__Rook__

#include <stdio.h>
#include "Piece.h"

class Rook : public Piece
{
    public:
    Rook(std::string name, std::string meshName, Square *square, std::vector<Piece*>& pieces);
    ~Rook();
};

#endif /* defined(__chess__Rook__) */
