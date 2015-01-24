#ifndef OGRE_DEMO_H
#define OGRE_DEMO_H

#include "OgreFramework.h"
#include "fmod.hpp"
#include "common.h"
#include "Gorilla.h"
#include "Piece.h"

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
    void setupWhitePieces();
    void setupBlackPieces();
	void runDemo();
    bool initialiseRTShaderSystem(Ogre::SceneManager* sceneMgr);
    void destroyRTShaderSystem();

    std::map<std::string, OIS::MouseListener*> mMouseListeners;
    std::map<std::string, OIS::MouseListener*>::iterator itMouseListener;
    std::map<std::string, OIS::MouseListener*>::iterator itMouseListenerEnd;
    
	Ogre::SceneNode *boardNode;
	Ogre::Entity *boardEntity;

    Ogre::SceneNode *whiteQueenNode;
    Ogre::Entity *whiteQueenEntity;
    
    Ogre::SceneNode *whiteKingNode;
    Ogre::Entity *whiteKingEntity;
    
    Piece *whitePawn1;
    Piece *whitePawn2;
    Piece *whitePawn3;
    Piece *whitePawn4;
    Piece *whitePawn5;
    Piece *whitePawn6;
    Piece *whitePawn7;
    Piece *whitePawn8;
    Piece *whiteRook1;
    Piece *whiteRook2;
    Piece *whiteKnight1;
    Piece *whiteKnight2;
    Piece *whiteBishop1;
    Piece *whiteBishop2;
    Piece *whiteQueen;
    Piece *whiteKing;
    
    Ogre::SceneNode *whiteRookNode1;
    Ogre::Entity *whiteRookEntity1;
    
    Ogre::SceneNode *whiteRookNode2;
    Ogre::Entity *whiteRookEntity2;
    
    Ogre::SceneNode *whiteKnightNode1;
    Ogre::Entity *whiteKnightEntity1;
    
    Ogre::SceneNode *whiteKnightNode2;
    Ogre::Entity *whiteKnightEntity2;
    
    Ogre::SceneNode *whiteBishopNode1;
    Ogre::Entity *whiteBishopEntity1;
    
    Ogre::SceneNode *whiteBishopNode2;
    Ogre::Entity *whiteBishopEntity2;
    
    Ogre::SceneNode *blackQueenNode;
    Ogre::Entity *blackQueenEntity;
    
    Ogre::SceneNode *blackKingNode;
    Ogre::Entity *blackKingEntity;
    
    Ogre::SceneNode *blackPawnNode;
    Ogre::Entity *blackPawnEntity;
    
    Ogre::SceneNode *blackPawnNode2;
    Ogre::Entity *blackPawnEntity2;
    
    Ogre::SceneNode *blackPawnNode3;
    Ogre::Entity *blackPawnEntity3;
    
    Ogre::SceneNode *blackPawnNode4;
    Ogre::Entity *blackPawnEntity4;
    
    Ogre::SceneNode *blackPawnNode5;
    Ogre::Entity *blackPawnEntity5;
    
    Ogre::SceneNode *blackPawnNode6;
    Ogre::Entity *blackPawnEntity6;
    
    Ogre::SceneNode *blackPawnNode7;
    Ogre::Entity *blackPawnEntity7;
    
    Ogre::SceneNode *blackPawnNode8;
    Ogre::Entity *blackPawnEntity8;
    
    Ogre::SceneNode *blackRookNode1;
    Ogre::Entity *blackRookEntity1;
    
    Ogre::SceneNode *blackRookNode2;
    Ogre::Entity *blackRookEntity2;
    
    Ogre::SceneNode *blackKnightNode1;
    Ogre::Entity *blackKnightEntity1;
    
    Ogre::SceneNode *blackKnightNode2;
    Ogre::Entity *blackKnightEntity2;
    
    Ogre::SceneNode *blackBishopNode1;
    Ogre::Entity *blackBishopEntity1;
    
    Ogre::SceneNode *blackBishopNode2;
    Ogre::Entity *blackBishopEntity2;
    
    Ogre::RaySceneQuery *mRayScnQuery;
    
    std::vector<Ogre::Node*> deselectedNodes;
    Ogre::Node *selectedNode;
    
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
    
	bool m_bShutdown;
#ifdef INCLUDE_RTSHADER_SYSTEM
    Ogre::RTShader::ShaderGenerator *mShaderGenerator;			// The Shader generator instance.
    ShaderGeneratorTechniqueResolverListener *mMaterialMgrListener;		// Shader generator material manager listener.
#endif // INCLUDE_RTSHADER_SYSTEM

};

#endif
