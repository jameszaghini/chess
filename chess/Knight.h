//
//  Knight.h
//  chess
//
//  Created by James Zaghini on 3/04/2015.
//
//

#ifndef __chess__Knight__
#define __chess__Knight__

#include <stdio.h>
#include "Piece.h"

class Knight : public Piece
{
    public:
    Knight(std::string name, std::string meshName, Square *square, std::vector<Piece*>& pieces);
    ~Knight();
};

#endif /* defined(__chess__Knight__) */
