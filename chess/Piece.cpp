//
//  Piece.cpp
//  chess
//
//  Created by James Zaghini on 24/01/2015.
//
//

#include "Piece.h"

Piece::Piece(std::string name, std::string meshName, Square *square, std::vector<Piece*>& pieces)
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    entity = sManager->createEntity(name, meshName);
    node = square->node->createChildSceneNode(name);
    this->square = square;
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

void Piece::moveToSquare(Square *destinationSquare)
{
    Ogre::Vector3 pos1 = square->node->getPosition();
    Ogre::Vector3 pos2 = destinationSquare->node->getPosition();
    
    this->node->setPosition(pos2.x - pos1.x, .25, pos2.z - pos1.z);
}