//
//  Piece.h
//  chess
//
//  Created by James Zaghini on 24/01/2015.
//
//

#ifndef __chess__Piece__
#define __chess__Piece__

#include "OgreFramework.h"
#include "Square.h"

#include <stdio.h>

class Piece
{
    
public:
    Piece(std::string name, std::string meshName, Square *square, std::vector<Piece*>& pieces);
    ~Piece();
    
    void select();
    void deselect();
    void moveToSquare(Square *destinationSquare);
    Piece * pieceByName(std::string *name);
    
    Ogre::SceneNode *node;
    Ogre::Entity *entity;
    
    Square *square;

    std::string name;
    
    
private:

    float y;
    float z;    
    
    float originalY;
    float originalZ;
    
};

#endif /* defined(__chess__Piece__) */
