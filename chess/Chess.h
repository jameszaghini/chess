#ifndef OGRE_DEMO_H
#define OGRE_DEMO_H

#include "OgreFramework.h"
#include "fmod.hpp"
#include "common.h"
#include "Gorilla.h"
#include "Piece.h"
#include "Pawn.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Queen.h"
#include "King.h"
#include "Board.h"
#include "Stockfish.h"

class Chess : public OIS::KeyListener, public OIS::MouseListener
{
public:
	Chess();
	~Chess();
    
	void startDemo();
    void update(double timeSinceLastFrame);
    
	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
    
private:
    void setupChessScene();
    void setupGorilla();
    void setupAudio();
    void setupLights();
    void setupBoard();
    void setupWhitePieces();
    void setupBlackPieces();
	void runDemo();
    bool initialiseRTShaderSystem(Ogre::SceneManager* sceneMgr);
    void destroyRTShaderSystem();
    bool cmpf(float A, float B, float epsilon = 0.005f);

    std::map<std::string, OIS::MouseListener*> mMouseListeners;
    std::map<std::string, OIS::MouseListener*>::iterator itMouseListener;
    std::map<std::string, OIS::MouseListener*>::iterator itMouseListenerEnd;
     
    Pawn *whitePawn1;
    Pawn *whitePawn2;
    Pawn *whitePawn3;
    Pawn *whitePawn4;
    Pawn *whitePawn5;
    Pawn *whitePawn6;
    Pawn *whitePawn7;
    Pawn *whitePawn8;
    Rook *whiteRook1;
    Rook *whiteRook2;
    Knight *whiteKnight1;
    Knight *whiteKnight2;
    Bishop *whiteBishop1;
    Bishop *whiteBishop2;
    Queen *whiteQueen;
    King *whiteKing;
    
    Pawn *blackPawn1;
    Pawn *blackPawn2;
    Pawn *blackPawn3;
    Pawn *blackPawn4;
    Pawn *blackPawn5;
    Pawn *blackPawn6;
    Pawn *blackPawn7;
    Pawn *blackPawn8;
    Rook *blackRook1;
    Rook *blackRook2;
    Knight *blackKnight1;
    Knight *blackKnight2;
    Bishop *blackBishop1;
    Bishop *blackBishop2;
    Queen *blackQueen;
    King *blackKing;
    
    Board *board;
    
    std::vector<Piece*> pieces;
    std::vector<Piece*> piecesToMove;
    
    Ogre::RaySceneQuery *mRayScnQuery;
    
    std::vector<Ogre::Node*> deselectedNodes;
    Ogre::Node *selectedNode;
    
    Ogre::Node *selectedSquareNode;
    
    Piece *selectedPiece;
    Square *selectedSquare;
    
    FMOD::System *system;
    FMOD::Sound *sound1;
    FMOD::Sound *pieceSound1;
    FMOD::Sound *pieceSound2;
    FMOD::Channel *channel = 0;
    FMOD_RESULT result;
    unsigned int version;
    void *extradriverdata = 0;
    
    Ogre::Real mTimer, mTimer2;
    Gorilla::Silverback *mSilverback;
    Gorilla::Screen *mScreen;
    Gorilla::Layer *mLayer;
    
    Gorilla::Polygon *poly;
    Gorilla::LineList *list;
    Gorilla::Caption *caption;
    Gorilla::Rectangle *rect;
    Gorilla::QuadList *quads;
    Gorilla::MarkupText *markup;
    
    Gorilla::Layer* mMousePointerLayer;
    Gorilla::Rectangle *mMousePointer;
    Ogre::Vector2 mNormalizedMousePosition;
    
    // For the smooth movment when moving the mouse
    Ogre::Vector3 cameraDirection;
    
    Stockfish stockfish;
    
	bool m_bShutdown;
};

#endif
