//
//  Board.cpp
//  chess
//
//  Created by James Zaghini on 29/01/2015.
//
//

#include "Board.h"
#include "OgreFramework.h"

Board::Board()
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    for(int i = 1; i < 9; i++) {
        for(int j = 0; j < 7; j++) {
            char letter = "ABCDEFG"[j];
            std::string s = letter + std::to_string(i);
            
            Ogre::Entity *e = sManager->createEntity(s, s + ".mesh");
            Ogre::SceneNode *n = sManager->getRootSceneNode()->createChildSceneNode(s);
            n->attachObject(e);
            n->setPosition(i*2,0,j*2);
            
        }
    }
}

Board::~Board()
{
    
}