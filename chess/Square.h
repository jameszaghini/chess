//
//  Square.h
//  chess
//
//  Created by James Zaghini on 29/01/2015.
//
//

#ifndef __chess__Square__
#define __chess__Square__

#include "OgreFramework.h"
#include <stdio.h>

class Square
{
public:
    Square(std::string name, float x, float y, float z);
    ~Square();
    std::string name;
    Ogre::SceneNode *node;
    Ogre::Entity *entity;
private:

};

#endif /* defined(__chess__Square__) */
