//
//  Piece.cpp
//  chess
//
//  Created by James Zaghini on 24/01/2015.
//
//

#include "Piece.h"
#include "Square.h"

Piece::Piece(std::string name, std::string meshName, Ogre::SceneNode *parent, std::vector<Piece*>& pieces)
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    entity = sManager->createEntity(name, meshName);
    node = parent->createChildSceneNode(name);
    node->attachObject(entity);
    node->translate(0, .25, 0);
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

void moveToSquare(std::string coordinate)
{
    
}