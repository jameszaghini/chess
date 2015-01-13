//|||||||||||||||||||||||||||||||||||||||||||||||

#include "OgreChess.h"

#include <OgreLight.h>
#include <OgreWindowEventUtilities.h>
#include "OgreText.h"

Chess::Chess()
{
	boardNode = 0;
	boardEntity	= 0;

    whiteQueenNode = 0;
    whiteQueenEntity = 0;
    
    whiteKingNode = 0;
    whiteKingEntity = 0;
    
    whitePawnNode = 0;
    whitePawnEntity = 0;
    
    whitePawnNode2 = 0;
    whitePawnEntity2 = 0;
}

Chess::~Chess()
{
#ifdef INCLUDE_RTSHADER_SYSTEM
    mShaderGenerator->removeSceneManager(OgreFramework::getSingletonPtr()->m_pSceneMgr);
    
    destroyRTShaderSystem();
#endif
    
    delete OgreFramework::getSingletonPtr();
}

#ifdef INCLUDE_RTSHADER_SYSTEM

/*-----------------------------------------------------------------------------
 | Initialize the RT Shader system.	
 -----------------------------------------------------------------------------*/
bool Chess::initialiseRTShaderSystem(Ogre::SceneManager* sceneMgr)
{			
    if (Ogre::RTShader::ShaderGenerator::initialize())
    {
        mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        
        mShaderGenerator->addSceneManager(sceneMgr);
        
        // Setup core libraries and shader cache path.
        Ogre::StringVector groupVector = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
        Ogre::StringVector::iterator itGroup = groupVector.begin();
        Ogre::StringVector::iterator itGroupEnd = groupVector.end();
        Ogre::String shaderCoreLibsPath;
        Ogre::String shaderCachePath;
        
        for (; itGroup != itGroupEnd; ++itGroup)
        {
            Ogre::ResourceGroupManager::LocationList resLocationsList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(*itGroup);
            Ogre::ResourceGroupManager::LocationList::iterator it = resLocationsList.begin();
            Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationsList.end();
            bool coreLibsFound = false;
            
            // Try to find the location of the core shader lib functions and use it
            // as shader cache path as well - this will reduce the number of generated files
            // when running from different directories.
            for (; it != itEnd; ++it)
            {
                if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos)
                {
                    shaderCoreLibsPath = (*it)->archive->getName() + "/";
                    shaderCachePath = shaderCoreLibsPath;
                    coreLibsFound = true;
                    break;
                }
            }
            // Core libs path found in the current group.
            if (coreLibsFound) 
                break; 
        }
        
        // Core shader libs not found -> shader generating will fail.
        if (shaderCoreLibsPath.empty())			
            return false;			
        
        // Create and register the material manager listener.
        mMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(mShaderGenerator);				
        Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
    }
    
    return true;
}

/*-----------------------------------------------------------------------------
 | Destroy the RT Shader system.
 -----------------------------------------------------------------------------*/
void Chess::destroyRTShaderSystem()
{
    // Restore default scheme.
    Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
    
    // Unregister the material manager listener.
    if (mMaterialMgrListener != NULL)
    {			
        Ogre::MaterialManager::getSingleton().removeListener(mMaterialMgrListener);
        delete mMaterialMgrListener;
        mMaterialMgrListener = NULL;
    }
    
    // Destroy RTShader system.
    if (mShaderGenerator != NULL)
    {				
        Ogre::RTShader::ShaderGenerator::destroy();
        mShaderGenerator = NULL;
    }
}
#endif // INCLUDE_RTSHADER_SYSTEM

void Chess::startDemo()
{
	new OgreFramework();
	if(!OgreFramework::getSingletonPtr()->initOgre("Chess", this, 0))
		return;
    
	m_bShutdown = false;
    
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");
	
#ifdef INCLUDE_RTSHADER_SYSTEM
    initialiseRTShaderSystem(OgreFramework::getSingletonPtr()->m_pSceneMgr);
    Ogre::MaterialPtr baseWhite = Ogre::MaterialManager::getSingleton().getByName("BaseWhite", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);				
    baseWhite->setLightingEnabled(false);
    mShaderGenerator->createShaderBasedTechnique(
                                                 "BaseWhite", 
                                                 Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
                                                 Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);	
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, 
                                       "BaseWhite");
    baseWhite->getTechnique(0)->getPass(0)->setVertexProgram(
                                                             baseWhite->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
    baseWhite->getTechnique(0)->getPass(0)->setFragmentProgram(
                                                               baseWhite->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
    
    // creates shaders for base material BaseWhiteNoLighting using the RTSS
    mShaderGenerator->createShaderBasedTechnique(
                                                 "BaseWhiteNoLighting", 
                                                 Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
                                                 Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);	
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, 
                                       "BaseWhiteNoLighting");
    Ogre::MaterialPtr baseWhiteNoLighting = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setVertexProgram(
                                                                       baseWhiteNoLighting->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setFragmentProgram(
                                                                         baseWhiteNoLighting->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
#endif
    
	setupChessScene();
#if !((OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__)
	runDemo();
#endif
}

void Chess::setupChessScene()
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
	sManager->setSkyBox(true, "Examples/SceneCubeMap2");

//	sManager->createLight("Light")->setPosition(75,75,75);

    Ogre::Light* spotLight = sManager->createLight("spotLight");
    spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
    spotLight->setDiffuseColour(0.8, 0.8, 0.8);
    spotLight->setSpecularColour(0.8, 0.8, 0.8);
    
    spotLight->setDirection(-1, -1, 0);
    spotLight->setPosition(Ogre::Vector3(40, 40, 0));
    
    spotLight->setSpotlightRange(Ogre::Degree(10), Ogre::Degree(28));
    
	boardEntity = sManager->createEntity("boardEntity", "board.mesh");
	boardNode = sManager->getRootSceneNode()->createChildSceneNode("boardNode");
	boardNode->attachObject(boardEntity);
    boardNode->setPosition(0,0,0);
    
    whiteQueenEntity = sManager->createEntity("whiteQueenEntity", "QueenB.mesh");
    whiteQueenNode = boardNode->createChildSceneNode("whiteQueenNode");
    whiteQueenNode->attachObject(whiteQueenEntity);
    whiteQueenNode->scale(1,1,1);
    whiteQueenNode->translate(0,1,0);    
    
    whiteKingEntity = sManager->createEntity("whiteKingEntity", "KingA.mesh");
    whiteKingNode = boardNode->createChildSceneNode("whiteKingNode");
    whiteKingNode->attachObject(whiteKingEntity);
    whiteKingNode->scale(1,1,1);
    whiteKingNode->translate(0,2,0);
    
    whitePawnEntity = sManager->createEntity("whitePawnEntity", "PawnA01.mesh");
    whitePawnNode = boardNode->createChildSceneNode("whitePawnNode");
    whitePawnNode->attachObject(whitePawnEntity);
    whitePawnNode->translate(-0.8,0.5,.5);
    
    whitePawnEntity2 = sManager->createEntity("whitePawnEntity2", "PawnA01.mesh");
    whitePawnNode2 = boardNode->createChildSceneNode("whitePawnNode2");
    whitePawnNode2->attachObject(whitePawnEntity2);
    whitePawnNode2->translate(1.2,0.5,.5);
    
    whitePawnEntity3 = sManager->createEntity("whitePawnEntity3", "PawnA01.mesh");
    whitePawnNode3 = boardNode->createChildSceneNode("whitePawnNode3");
    whitePawnNode3->attachObject(whitePawnEntity3);
    whitePawnNode3->translate(3.2,0.5,.5);
    
    whitePawnEntity4 = sManager->createEntity("whitePawnEntity4", "PawnA01.mesh");
    whitePawnNode4 = boardNode->createChildSceneNode("whitePawnNode4");
    whitePawnNode4->attachObject(whitePawnEntity4);
    whitePawnNode4->translate(5.2,0.5,.5);
    
    whitePawnEntity5 = sManager->createEntity("whitePawnEntity5", "PawnA01.mesh");
    whitePawnNode5 = boardNode->createChildSceneNode("whitePawnNode5");
    whitePawnNode5->attachObject(whitePawnEntity5);
    whitePawnNode5->translate(7.2,0.5,.5);
    
    whitePawnEntity6 = sManager->createEntity("whitePawnEntity6", "PawnA01.mesh");
    whitePawnNode6 = boardNode->createChildSceneNode("whitePawnNode6");
    whitePawnNode6->attachObject(whitePawnEntity6);
    whitePawnNode6->translate(9.2,0.5,.5);
    
    whitePawnEntity7 = sManager->createEntity("whitePawnEntity7", "PawnA01.mesh");
    whitePawnNode7 = boardNode->createChildSceneNode("whitePawnNode7");
    whitePawnNode7->attachObject(whitePawnEntity7);
    whitePawnNode7->translate(11.2,0.5,.5);
    
    whitePawnEntity8 = sManager->createEntity("whitePawnEntity8", "PawnA01.mesh");
    whitePawnNode8 = boardNode->createChildSceneNode("whitePawnNode8");
    whitePawnNode8->attachObject(whitePawnEntity8);
    whitePawnNode8->translate(13.2,0.5,.5);
}

void Chess::runDemo()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Start main loop...");
	
	double timeSinceLastFrame = 0;
	double startTime = 0;
    
    OgreFramework::getSingletonPtr()->m_pRenderWnd->resetStatistics();
    
#if (!defined(OGRE_IS_IOS)) && !((OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__)
	while(!m_bShutdown && !OgreFramework::getSingletonPtr()->isOgreToBeShutDown()) 
	{
		if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isClosed())m_bShutdown = true;
        
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_LINUX || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		Ogre::WindowEventUtilities::messagePump();
#endif	
		if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isActive())
		{
			startTime = OgreFramework::getSingletonPtr()->m_pTimer->getMillisecondsCPU();
            
#if !OGRE_IS_IOS
			OgreFramework::getSingletonPtr()->m_pKeyboard->capture();
#endif
			OgreFramework::getSingletonPtr()->m_pMouse->capture();
            
			OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
			OgreFramework::getSingletonPtr()->m_pRoot->renderOneFrame();
            
			timeSinceLastFrame = OgreFramework::getSingletonPtr()->m_pTimer->getMillisecondsCPU() - startTime;
		}
		else
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
		}
	}
#endif
    
#if !defined(OGRE_IS_IOS)
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Main loop quit");
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
#endif
}

bool Chess::keyPressed(const OIS::KeyEvent &keyEventRef)
{
#if !defined(OGRE_IS_IOS)
	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_F))
	{
        OgreText *textItem = new OgreText;
        textItem->setText("Hello World!");    // Text to be displayed
        // Now it is possible to use the Ogre::String as parameter too
        textItem->setPos(0.1f,0.1f);        // Text position, using relative co-ordinates
        textItem->setCol(1.0f,1.0f,1.0f,0.5f);    // Text colour (Red, Green, Blue, Alpha)
    }
#endif
	return true;
}

bool Chess::keyReleased(const OIS::KeyEvent &keyEventRef)
{
#if !defined(OGRE_IS_IOS)
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
#endif

	return true;
}