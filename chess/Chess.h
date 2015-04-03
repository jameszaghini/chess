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
#include "Board.h"
#include "Stockfish.h"

#ifdef INCLUDE_RTSHADER_SYSTEM

#include "OgreRTShaderSystem.h"


/** This class demonstrates basic usage of the RTShader system.
 It sub class the material manager listener class and when a target scheme callback
 is invoked with the shader generator scheme it tries to create an equivalent shader
 based technique based on the default technique of the given material.
 */
class ShaderGeneratorTechniqueResolverListener : public Ogre::MaterialManager::Listener
{
public:
    
	ShaderGeneratorTechniqueResolverListener(Ogre::RTShader::ShaderGenerator* pShaderGenerator)
	{
		mShaderGenerator = pShaderGenerator;			
	}
    
	/** This is the hook point where shader based technique will be created.
     It will be called whenever the material manager won't find appropriate technique
     that satisfy the target scheme name. If the scheme name is out target RT Shader System
     scheme name we will try to create shader generated technique for it. 
     */
	virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex, 
                                                  const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, 
                                                  const Ogre::Renderable* rend)
	{	
		Ogre::Technique* generatedTech = NULL;
        
		// Case this is the default shader generator scheme.
		if (schemeName == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
		{
			bool techniqueCreated;
            
			// Create shader generated technique for this material.
			techniqueCreated = mShaderGenerator->createShaderBasedTechnique(
                                                                            originalMaterial->getName(), 
                                                                            Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
                                                                            schemeName);	
            
			// Case technique registration succeeded.
			if (techniqueCreated)
			{
				// Force creating the shaders for the generated technique.
				mShaderGenerator->validateMaterial(schemeName, originalMaterial->getName());
				
				// Grab the generated technique.
				Ogre::Material::TechniqueIterator itTech = originalMaterial->getTechniqueIterator();
                
				while (itTech.hasMoreElements())
				{
					Ogre::Technique* curTech = itTech.getNext();
                    
					if (curTech->getSchemeName() == schemeName)
					{
						generatedTech = curTech;
						break;
					}
				}				
			}
		}
        
		return generatedTech;
	}
    
protected:	
	Ogre::RTShader::ShaderGenerator *mShaderGenerator; // The shader generator instance.
};
#endif

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
    Piece *whiteKnight1;
    Piece *whiteKnight2;
    Bishop *whiteBishop1;
    Bishop *whiteBishop2;
    Piece *whiteQueen;
    Piece *whiteKing;
    
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
    Piece *blackKnight1;
    Piece *blackKnight2;
    Bishop *blackBishop1;
    Bishop *blackBishop2;
    Piece *blackQueen;
    Piece *blackKing;
    
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
#ifdef INCLUDE_RTSHADER_SYSTEM
    Ogre::RTShader::ShaderGenerator *mShaderGenerator;			// The Shader generator instance.
    ShaderGeneratorTechniqueResolverListener *mMaterialMgrListener;		// Shader generator material manager listener.
#endif // INCLUDE_RTSHADER_SYSTEM

};

#endif
