//
//  Piece.cpp
//  chess
//
//  Created by James Zaghini on 24/01/2015.
//
//

#include "Piece.h"

Piece::Piece(std::string name, std::string meshName, float x, float y, float z, Ogre::SceneNode *parent, std::vector<Piece*>& pieces)
{
    originalX = x;
    originalY = y;
    originalZ = z;
    
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    entity = sManager->createEntity(name, meshName);
    node = parent->createChildSceneNode(name);
    node->attachObject(entity);
    node->translate(x, y, z);
    pieces.push_back(this);
}

Piece::~Piece()
{
    
}

void Piece::select()
{
    entity->setMaterialName("red");
}

void Piece::deselect()
{
    std::string pieceName = node->getName();
    std::string color = (pieceName.find("white") != std::string::npos ? "white" : "black");
    entity->setMaterialName(color);
}