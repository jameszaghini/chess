//|||||||||||||||||||||||||||||||||||||||||||||||

#include "OgreChess.h"

#include <OgreLight.h>
#include <OgreWindowEventUtilities.h>
#include "OgreText.h"

Chess::Chess()
{
    selectedNode = 0;
    
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
	if(!OgreFramework::getSingletonPtr()->initOgre("Chess", this, this))
		return;
    
	m_bShutdown = false;
    
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");
	
#ifdef INCLUDE_RTSHADER_SYSTEM
    initialiseRTShaderSystem(OgreFramework::getSingletonPtr()->m_pSceneMgr);
    Ogre::MaterialPtr baseWhite = Ogre::MaterialManager::getSingleton().getByName("BaseWhite", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);				
    baseWhite->setLightingEnabled(false);
    mShaderGenerator->createShaderBasedTechnique("BaseWhite", Ogre::MaterialManager::DEFAULT_SCHEME_NAME, Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, "BaseWhite");
    baseWhite->getTechnique(0)->getPass(0)->setVertexProgram(baseWhite->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
    baseWhite->getTechnique(0)->getPass(0)->setFragmentProgram(baseWhite->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
    
    // creates shaders for base material BaseWhiteNoLighting using the RTSS
    mShaderGenerator->createShaderBasedTechnique("BaseWhiteNoLighting", Ogre::MaterialManager::DEFAULT_SCHEME_NAME, Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, "BaseWhiteNoLighting");
    Ogre::MaterialPtr baseWhiteNoLighting = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setVertexProgram(baseWhiteNoLighting->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setFragmentProgram(baseWhiteNoLighting->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
#endif
    
	setupChessScene();
    
    
#if !((OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__)
	runDemo();
#endif
}

void Chess::update(double timeSinceLastFrame)
{
    if(selectedNode) {
        Vector3 position = selectedNode->getPosition();
        float newY = position.y + 0.1;
        if(newY > 1) {
            return;
        }
        selectedNode->setPosition(position.x, newY, position.z);
    }
}

void Chess::setupChessScene()
{
    setupGorilla();
    setupAudio();    
    setupLights();
    setupWhitePieces();
    setupBlackPieces();
    
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;

    mRayScnQuery = sManager->createRayQuery(Ogre::Ray());
}

void Chess::setupGorilla()
{
    // Create Silverback and load in dejavu
    mSilverback = new Gorilla::Silverback();
    mSilverback->loadAtlas("dejavu");
    mScreen = mSilverback->createScreen(OgreFramework::getSingletonPtr()->m_pCamera->getViewport(), "dejavu");
    mScreen->setOrientation(Ogre::OR_DEGREE_0);
    Ogre::Real vpW = mScreen->getWidth(), vpH = mScreen->getHeight();
    
    // Create our drawing layer
    mLayer = mScreen->createLayer(0);
    rect = mLayer->createRectangle(20, 20, vpW / 4, vpH / 4);
    rect->background_gradient(Gorilla::Gradient_Diagonal, Gorilla::rgb(100,149,237), Gorilla::rgb(65,105,225));
    
    markup = mLayer->createMarkupText(9,25,25, "%@24%Chess\n%@14%Ogre3D :: FMOD :: Gorilla%@9%\nAnnoying drum loop");
    
    mMousePointerLayer = mScreen->createLayer(15);
    mMousePointer = mMousePointerLayer->createRectangle(0,0,10,18);
    mMousePointer->background_image("mousepointer");
    mMousePointer->position(OgreFramework::getSingletonPtr()->m_pRenderWnd->getWidth() / 2, OgreFramework::getSingletonPtr()->m_pRenderWnd->getHeight() / 2);
}

void Chess::setupAudio()
{
    /* Create a System object and initialize */
    Common_Init(&extradriverdata);
    
    result = FMOD::System_Create(&system);
    ERRCHECK(result);
    
    result = system->getVersion(&version);
    ERRCHECK(result);
    if (version < FMOD_VERSION)
    {
        Common_Fatal("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
    }
    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);
    
    result = system->createSound(Common_MediaPath("drumloop.wav"), FMOD_DEFAULT, 0, &sound1);
    ERRCHECK(result);
    
  //  result = sound1->setMode(FMOD_LOOP_OFF);    /* drumloop.wav has embedded loop points which automatically makes looping turn on, */
  //  ERRCHECK(result);                           /* so turn it off here.  We could have also just put FMOD_LOOP_OFF in the above CreateSound call. */
    
    result = system->playSound(sound1, 0, false, &channel);
    ERRCHECK(result);
}

void Chess::setupLights()
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;

    Ogre::Light* spotLight = sManager->createLight("spotLight");
    spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
    spotLight->setDiffuseColour(0.8, 0.8, 0.8);
    spotLight->setSpecularColour(0.8, 0.8, 0.8);
    spotLight->setDirection(-1, -1, 0);
    spotLight->setPosition(Ogre::Vector3(40, 40, -10));
    spotLight->setSpotlightRange(Ogre::Degree(10), Ogre::Degree(30));
    
    Ogre::Light* spotLight2 = sManager->createLight("spotLight2");
    spotLight2->setType(Ogre::Light::LT_SPOTLIGHT);
    spotLight2->setDiffuseColour(0.7, 0.2, 0.1);
    spotLight2->setSpecularColour(0.8, 0.8, 0.8);
    spotLight2->setDirection(-1, -1, 0);
    spotLight2->setPosition(Ogre::Vector3(60, 40, -10));
    spotLight2->setSpotlightRange(Ogre::Degree(10), Ogre::Degree(30));
}

void Chess::setupWhitePieces()
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    
    boardEntity = sManager->createEntity("boardEntity", "board.mesh");
    boardNode = sManager->getRootSceneNode()->createChildSceneNode("boardNode");
    boardNode->attachObject(boardEntity);
    boardNode->setPosition(0, 0, 0);
    
    whiteQueenEntity = sManager->createEntity("whiteQueenEntity", "QueenA.mesh");
    whiteQueenNode = boardNode->createChildSceneNode("whiteQueenNode");
    whiteQueenNode->attachObject(whiteQueenEntity);
    whiteQueenNode->translate(7.1, 1.5, -1);
    
    whiteKingEntity = sManager->createEntity("whiteKingEntity", "KingA.mesh");
    whiteKingNode = boardNode->createChildSceneNode("whiteKingNode");
    whiteKingNode->attachObject(whiteKingEntity);
    whiteKingNode->translate(9.1, 1.35, -1);
    
    float pawnX = 1.1;
    float pawnZ = -3.f;
    float pawnY = 0.25;
    whitePawnEntity = sManager->createEntity("whitePawnEntity", "PawnA01.mesh");
    whitePawnNode = boardNode->createChildSceneNode("whitePawnNode");
    whitePawnNode->attachObject(whitePawnEntity);
    whitePawnNode->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    whitePawnEntity2 = sManager->createEntity("whitePawnEntity2", "PawnA01.mesh");
    whitePawnNode2 = boardNode->createChildSceneNode("whitePawnNode2");
    whitePawnNode2->attachObject(whitePawnEntity2);
    whitePawnNode2->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    whitePawnEntity3 = sManager->createEntity("whitePawnEntity3", "PawnA01.mesh");
    whitePawnNode3 = boardNode->createChildSceneNode("whitePawnNode3");
    whitePawnNode3->attachObject(whitePawnEntity3);
    whitePawnNode3->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    whitePawnEntity4 = sManager->createEntity("whitePawnEntity4", "PawnA01.mesh");
    whitePawnNode4 = boardNode->createChildSceneNode("whitePawnNode4");
    whitePawnNode4->attachObject(whitePawnEntity4);
    whitePawnNode4->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    whitePawnEntity5 = sManager->createEntity("whitePawnEntity5", "PawnA01.mesh");
    whitePawnNode5 = boardNode->createChildSceneNode("whitePawnNode5");
    whitePawnNode5->attachObject(whitePawnEntity5);
    whitePawnNode5->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    whitePawnEntity6 = sManager->createEntity("whitePawnEntity6", "PawnA01.mesh");
    whitePawnNode6 = boardNode->createChildSceneNode("whitePawnNode6");
    whitePawnNode6->attachObject(whitePawnEntity6);
    whitePawnNode6->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    whitePawnEntity7 = sManager->createEntity("whitePawnEntity7", "PawnA01.mesh");
    whitePawnNode7 = boardNode->createChildSceneNode("whitePawnNode7");
    whitePawnNode7->attachObject(whitePawnEntity7);
    whitePawnNode7->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    whitePawnEntity8 = sManager->createEntity("whitePawnEntity8", "PawnA01.mesh");
    whitePawnNode8 = boardNode->createChildSceneNode("whitePawnNode8");
    whitePawnNode8->attachObject(whitePawnEntity8);
    whitePawnNode8->translate(pawnX, pawnY, pawnZ);
    
    whiteRookEntity1 = sManager->createEntity("whiteRookEntity1", "TowerA01.mesh");
    whiteRookNode1 = boardNode->createChildSceneNode("whiteRookNode1");
    whiteRookNode1->attachObject(whiteRookEntity1);
    whiteRookNode1->translate(1.1, 0.8, -1);
    
    whiteRookEntity2 = sManager->createEntity("whiteRookEntity2", "TowerA01.mesh");
    whiteRookNode2 = boardNode->createChildSceneNode("whiteRookNode2");
    whiteRookNode2->attachObject(whiteRookEntity2);
    whiteRookNode2->translate(15.1, 0.8, -1);
    
    whiteKnightEntity1 = sManager->createEntity("whiteKnightEntity1", "HorseA01.mesh");
    whiteKnightNode1 = boardNode->createChildSceneNode("whiteKnightNode1");
    whiteKnightNode1->attachObject(whiteKnightEntity1);
    whiteKnightNode1->translate(3.1, 0.7f, -1);
    
    whiteKnightEntity2 = sManager->createEntity("whiteKnightEntity2", "HorseA01.mesh");
    whiteKnightNode2 = boardNode->createChildSceneNode("whiteKnightNode2");
    whiteKnightNode2->attachObject(whiteKnightEntity2);
    whiteKnightNode2->translate(13.1, 0.7, -1);
    whiteKnightNode2->roll(Degree(-90));
    
    whiteBishopEntity1 = sManager->createEntity("whiteBishopEntity1", "Bishop.mesh");
    whiteBishopNode1 = boardNode->createChildSceneNode("whiteBishopNode1");
    whiteBishopNode1->attachObject(whiteBishopEntity1);
    whiteBishopNode1->translate(5.1, 0.6, -1);
    
    whiteBishopEntity2 = sManager->createEntity("whiteBishopEntity2", "Bishop.mesh");
    whiteBishopNode2 = boardNode->createChildSceneNode("whiteBishopNode2");
    whiteBishopNode2->attachObject(whiteBishopEntity2);
    whiteBishopNode2->translate(11.1, 0.6, -1);
    whiteBishopNode2->yaw(Degree(90));
}

void Chess::setupBlackPieces()
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    
    float pawnX = 1.1;
    float pawnZ = -13.0f;
    float pawnY = 0.65;
    blackPawnEntity = sManager->createEntity("blackPawnEntity", "PawnA01_black.mesh");
    blackPawnNode = boardNode->createChildSceneNode("blackPawnNode");
    blackPawnNode->attachObject(blackPawnEntity);
    blackPawnNode->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    blackPawnEntity2 = sManager->createEntity("blackPawnEntity2", "PawnA01_black.mesh");
    blackPawnNode2 = boardNode->createChildSceneNode("blackPawnNode2");
    blackPawnNode2->attachObject(blackPawnEntity2);
    blackPawnNode2->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    blackPawnEntity3 = sManager->createEntity("blackPawnEntity3", "PawnA01_black.mesh");
    blackPawnNode3 = boardNode->createChildSceneNode("blackPawnNode3");
    blackPawnNode3->attachObject(blackPawnEntity3);
    blackPawnNode3->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    blackPawnEntity4 = sManager->createEntity("blackPawnEntity4", "PawnA01_black.mesh");
    blackPawnNode4 = boardNode->createChildSceneNode("blackPawnNode4");
    blackPawnNode4->attachObject(blackPawnEntity4);
    blackPawnNode4->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    blackPawnEntity5 = sManager->createEntity("blackPawnEntity5", "PawnA01_black.mesh");
    blackPawnNode5 = boardNode->createChildSceneNode("blackPawnNode5");
    blackPawnNode5->attachObject(blackPawnEntity5);
    blackPawnNode5->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    blackPawnEntity6 = sManager->createEntity("blackPawnEntity6", "PawnA01_black.mesh");
    blackPawnNode6 = boardNode->createChildSceneNode("blackPawnNode6");
    blackPawnNode6->attachObject(blackPawnEntity6);
    blackPawnNode6->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    blackPawnEntity7 = sManager->createEntity("blackPawnEntity7", "PawnA01_black.mesh");
    blackPawnNode7 = boardNode->createChildSceneNode("blackPawnNode7");
    blackPawnNode7->attachObject(blackPawnEntity7);
    blackPawnNode7->translate(pawnX, pawnY, pawnZ);
    pawnX+=2;
    
    blackPawnEntity8 = sManager->createEntity("blackPawnEntity8", "PawnA01_black.mesh");
    blackPawnNode8 = boardNode->createChildSceneNode("blackPawnNode8");
    blackPawnNode8->attachObject(blackPawnEntity8);
    blackPawnNode8->translate(pawnX, pawnY, pawnZ);
    
    blackRookEntity1 = sManager->createEntity("blackRookEntity1", "TowerA01_black.mesh");
    blackRookNode1 = boardNode->createChildSceneNode("blackRookNode1");
    blackRookNode1->attachObject(blackRookEntity1);
    blackRookNode1->translate(1.1, 0.8, -15);
    
    blackRookEntity2 = sManager->createEntity("blackRookEntity2", "TowerA01_black.mesh");
    blackRookNode2 = boardNode->createChildSceneNode("blackRookNode2");
    blackRookNode2->attachObject(blackRookEntity2);
    blackRookNode2->translate(15.1, 0.8, -15);
    
    blackKnightEntity1 = sManager->createEntity("blackKnightEntity1", "HorseA01_black.mesh");
    blackKnightNode1 = boardNode->createChildSceneNode("blackKnightNode1");
    blackKnightNode1->attachObject(blackKnightEntity1);
    blackKnightNode1->translate(3.1, 0.7f, -15);
    
    blackKnightEntity2 = sManager->createEntity("blackKnightEntity2", "HorseA01_black.mesh");
    blackKnightNode2 = boardNode->createChildSceneNode("blackKnightNode2");
    blackKnightNode2->attachObject(blackKnightEntity2);
    blackKnightNode2->translate(13.1, 0.7, -15);
    blackKnightNode2->roll(Degree(-90));
    
    blackBishopEntity1 = sManager->createEntity("blackBishopEntity1", "Bishop_black.mesh");
    blackBishopNode1 = boardNode->createChildSceneNode("blackBishopNode1");
    blackBishopNode1->attachObject(blackBishopEntity1);
    blackBishopNode1->translate(5.1, 0.6, -15);
    
    blackBishopEntity2 = sManager->createEntity("blackBishopEntity2", "Bishop_black.mesh");
    blackBishopNode2 = boardNode->createChildSceneNode("blackBishopNode2");
    blackBishopNode2->attachObject(blackBishopEntity2);
    blackBishopNode2->translate(11.1, 0.6, -15);
    blackBishopNode2->yaw(Degree(90));
    
    blackQueenEntity = sManager->createEntity("blackQueenEntity", "QueenA_black.mesh");
    blackQueenNode = boardNode->createChildSceneNode("blackQueenNode");
    blackQueenNode->attachObject(blackQueenEntity);
    blackQueenNode->translate(7.1, 1.5, -15);
    
    blackKingEntity = sManager->createEntity("blackKingEntity", "King_black.mesh");
    blackKingNode = boardNode->createChildSceneNode("blackKingNode");
    blackKingNode->attachObject(blackKingEntity);
    blackKingNode->translate(9.1, .25, -15);
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
            
            Axis x = OgreFramework::getSingletonPtr()->m_pMouse->getMouseState().X;
            
            m_pMouse->getMouseState().height = m_pRenderWnd->getHeight();
            m_pMouse->getMouseState().width	 = m_pRenderWnd->getWidth();
            
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

bool Chess::mouseMoved(const OIS::MouseEvent &e) {
 
    OgreFramework::getSingletonPtr()->mouseMoved(e);

    printf("x: %d, y: %d\n", e.state.X.abs, e.state.Y.abs);

    mMousePointer->position(e.state.X.abs, e.state.Y.abs);
    
//    itMouseListener    = mMouseListeners.begin();
//    itMouseListenerEnd = mMouseListeners.end();
//    for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
//        if(!itMouseListener->second->mouseMoved( e ))
//            break;
//    }
    
    return true;
}

bool Chess::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    return true;
}

bool Chess::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    Ogre::Vector2 mousePosition = mMousePointer->position();
    
    Real x = (mousePosition.x / OgreFramework::getSingletonPtr()->m_pRenderWnd->getWidth());
    Real y = (mousePosition.y / OgreFramework::getSingletonPtr()->m_pRenderWnd->getHeight());
    Ray mouseRay = OgreFramework::getSingletonPtr()->m_pCamera->getCameraToViewportRay(x, y);

    mRayScnQuery->setRay(mouseRay);
    mRayScnQuery->setSortByDistance(true);
    
    /*
     This next chunk finds the results of the raycast
     If the mouse is pointing at world geometry we spawn a robot at that position
     */
    Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
    Ogre::RaySceneQueryResult::iterator iter = result.begin();
    
    for (iter = result.begin( ); iter != result.end(); iter++) {
        Node *clickedNode = iter->movable->getParentNode();
        
        if(clickedNode->getName() != "boardNode") {
            printf("%s", clickedNode->getName().c_str());
            
            if(selectedNode) {
//                selectedNode->setScale(1,1,1);
            }
            
//            clickedNode->setScale(1.2,1.2,1.2);
            selectedNode = clickedNode;
           
            break;
        }
    }
    
    return true;
}

bool Chess::keyPressed(const OIS::KeyEvent &keyEventRef)
{
#if !defined(OGRE_IS_IOS)
	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_F))
	{
        OgreText *textItem = new OgreText;
        
        Ogre::Vector3 cameraPosition = OgreFramework::getSingletonPtr()->m_pCamera->getPosition();
        
        printf("Camera x: %f, y: %f, z: %f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        
        //textItem->setText("Camera x: %f, y: %f, z: %f", cameraPosition.x, cameraPosition.y, cameraPosition.z);    // Text to be displayed
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