//
//  Queen.h
//  chess
//
//  Created by James Zaghini on 3/04/2015.
//
//

#ifndef __chess__Queen__
#define __chess__Queen__

#include <stdio.h>
#include "Piece.h"

class Queen : public Piece
{
    public:
    Queen(std::string name, std::string meshName, Square *square, std::vector<Piece*>& pieces);
    ~Queen();
};

#endif /* defined(__chess__Queen__) */
