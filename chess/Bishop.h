//
//  Bishop.h
//  chess
//
//  Created by James Zaghini on 3/04/2015.
//
//

#ifndef __chess__Bishop__
#define __chess__Bishop__

#include <stdio.h>
#include "Piece.h"

class Bishop : public Piece
{
    public:
    Bishop(std::string name, std::string meshName, Square *square, std::vector<Piece*>& pieces);
    ~Bishop();
};

#endif /* defined(__chess__Bishop__) */
