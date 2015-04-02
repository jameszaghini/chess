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
    this->name = name;
    node->attachObject(entity);
    node->translate(0, .25, 0);
    pieces.push_back(this);
    
    Ogre::Vector3 pos = square->node->getPosition();
    this->originalX = pos.x;
    this->originalY = pos.y;
    this->originalZ = pos.z;
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
    Ogre::Vector3 pos2 = destinationSquare->node->getPosition();
    
    Ogre::Real x = pos2.x - this->originalX;
    Ogre::Real y = .25;
    Ogre::Real z = pos2.z - this->originalZ;

    std::cout << "-----------" << this->name << "-------------" << std::endl;
    std::cout << "x: " << this->originalX << std::endl;
    std::cout << "y: " << this->originalY << std::endl;
    std::cout << "z: " << this->originalZ << std::endl;
    std::cout << "dest-x: " << pos2.x << std::endl;
    std::cout << "dest-y: " << pos2.y << std::endl;
    std::cout << "dest-z: " << pos2.z << std::endl;
    
    destinationX = x;
    destinationY = y;
    destinationZ = z;
    
    if(this->isBlack()) {
        this->node->setPosition(x, y, z);
    }
    this->square = destinationSquare;
}

bool Piece::isBlack() {
    std::string pieceName = node->getName();
    return pieceName.find("black") != std::string::npos ? true : false;

}
