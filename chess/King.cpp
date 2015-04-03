//
//  King.cpp
//  chess
//
//  Created by James Zaghini on 3/04/2015.
//
//

#include "King.h"

King::King(std::string name, std::string meshName, Square *square, std::vector<Piece*>& pieces) : Piece(name, meshName, square, pieces)
{
}

King::~King()
{   
}
