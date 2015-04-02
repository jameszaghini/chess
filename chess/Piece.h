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
    Ogre::Real originalX;
    Ogre::Real originalY;
    Ogre::Real originalZ;
    
    Ogre::Real destinationX;
    Ogre::Real destinationY;
    Ogre::Real destinationZ;
    
private:
    
    bool isBlack();

    
};

#endif /* defined(__chess__Piece__) */
