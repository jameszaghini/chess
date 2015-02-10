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

#include <stdio.h>

class Piece
{
    
public:
    Piece(std::string name, std::string meshName, Ogre::SceneNode *parent, std::vector<Piece*>& pieces);
    ~Piece();
    
    void select();
    void deselect();
    void moveToSquare(std::string coordinate);
    
    Ogre::SceneNode *node;
    Ogre::Entity *entity;
    
private:

    float y;
    float z;
    
    float originalY;
    float originalZ;
    
};

#endif /* defined(__chess__Piece__) */
