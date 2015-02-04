//
//  Square.cpp
//  chess
//
//  Created by James Zaghini on 29/01/2015.
//
//

#include "Square.h"

Square::Square(std::string name, float x, float y, float z)
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    
    entity = sManager->createEntity(name, name + ".mesh");
    node = sManager->getRootSceneNode()->createChildSceneNode(name);
    node->attachObject(entity);
    node->setPosition(x, y, z);
    
    this->name = name;
}

Square::~Square()
{
    
}
