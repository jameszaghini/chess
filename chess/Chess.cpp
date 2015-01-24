//|||||||||||||||||||||||||||||||||||||||||||||||

#include "Chess.h"

#include <OgreLight.h>
#include <OgreWindowEventUtilities.h>
#include "OgreText.h"

Chess::Chess()
{
    selectedNode = 0;
    
	boardNode = 0;
	boardEntity	= 0;

    whitePawn1 = 0;
    whitePawn2 = 0;
    whitePawn3 = 0;
    whitePawn4 = 0;
    whitePawn5 = 0;
    whitePawn6 = 0;
    whitePawn7 = 0;
    whitePawn8 = 0;
    whiteRook1 = 0;
    whiteRook2 = 0;
    whiteKnight1 = 0;
    whiteKnight2 = 0;
    whiteBishop1 = 0;
    whiteBishop2 = 0;
    whiteQueen = 0;
    whiteKing = 0;
    
    blackPawn1 = 0;
    blackPawn2 = 0;
    blackPawn3 = 0;
    blackPawn4 = 0;
    blackPawn5 = 0;
    blackPawn6 = 0;
    blackPawn7 = 0;
    blackPawn8 = 0;
    blackRook1 = 0;
    blackRook2 = 0;
    blackKnight1 = 0;
    blackKnight2 = 0;
    blackBishop1 = 0;
    blackBishop2 = 0;
    blackQueen = 0;
    blackKing = 0;
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
    if(deselectedNodes.size()) {
        int j = 0;
        int toRemove = -1;
        for ( auto &i : deselectedNodes ) {
            Ogre::Node *node = (Ogre::Node*)i;
            Vector3 position = node->getPosition();
            node->setPosition(position.x, position.y -0.1, position.z);
            if(position.y < 1) {
                toRemove = j;

            }
            j++;
        }
        if(toRemove >= 0) {
            system->playSound(pieceSound1, 0, false, &channel);
            deselectedNodes.erase(deselectedNodes.begin() + toRemove);
        }
    }
    
    if(selectedNode) {
        Vector3 position = selectedNode->getPosition();
        float newY = position.y + 0.1;
        if(newY > 3) {
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
    setupBoard();
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
    rect = mLayer->createRectangle(20, 20, vpW / 6, vpH / 6);
    rect->background_gradient(Gorilla::Gradient_Diagonal, Gorilla::rgb(100,149,237), Gorilla::rgb(65,105,225));
    
//    markup = mLayer->createMarkupText(9,25,25, "%@24%Chess\n%@14%Ogre3D :: FMOD :: Gorilla%@9%\nAnnoying drum loop");
    markup = mLayer->createMarkupText(9,25,25, "%@24%Chess\n");
    
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
    if (version < FMOD_VERSION) {
        Common_Fatal("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
    }
    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);
    
    result = system->createSound(Common_MediaPath("thud10.wav"), FMOD_DEFAULT, 0, &pieceSound1);
    ERRCHECK(result);

    result = system->createSound(Common_MediaPath("thud1.wav"), FMOD_DEFAULT, 0, &pieceSound2);
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
    
    sManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

}

void Chess::setupBoard()
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    
    boardEntity = sManager->createEntity("boardEntity", "board.mesh");
    boardNode = sManager->getRootSceneNode()->createChildSceneNode("boardNode");
    boardNode->attachObject(boardEntity);
    boardNode->setPosition(0, 0, 0);
}

void Chess::setupWhitePieces()
{

    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;

    float pawnX = 1.1;
    float pawnZ = -3.f;
    float pawnY = 0.25;
    
    whitePawn1 = new Piece("whitePawn1", "PawnA01.mesh", pawnX, pawnY, pawnZ, boardNode);
    whitePawn2 = new Piece("whitePawn2", "PawnA01.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    whitePawn3 = new Piece("whitePawn3", "PawnA01.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    whitePawn4 = new Piece("whitePawn4", "PawnA01.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    whitePawn5 = new Piece("whitePawn5", "PawnA01.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    whitePawn6 = new Piece("whitePawn6", "PawnA01.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    whitePawn7 = new Piece("whitePawn7", "PawnA01.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    whitePawn8 = new Piece("whitePawn8", "PawnA01.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    whiteRook1 = new Piece("whiteRook1", "TowerA01.mesh", 1.1, 0.8, -1, boardNode);
    whiteRook2 = new Piece("whiteRook2", "TowerA01.mesh", 15.1, 0.8, -1, boardNode);
    whiteKnight1 = new Piece("whiteKnight1", "HorseA01.mesh", 3.1, 0.7f, -1, boardNode);
    whiteKnight2 = new Piece("whiteKnight2", "HorseA01.mesh", 13.1, 0.7, -1, boardNode);
//    whiteKnight2->node->roll(Degree(-90));
    whiteBishop1 = new Piece("whiteBishop1", "Bishop.mesh", 5.1, 0.6, -1, boardNode);
    whiteBishop2 = new Piece("whiteBishop2", "Bishop.mesh", 11.1, 0.6, -1, boardNode);
//    whiteBishop2->node->yaw(Degree(90));
    whiteQueen = new Piece("whiteQueen", "QueenA.mesh", 7.1, 1.5, -1, boardNode);
    whiteKing = new Piece("whiteKing", "KingA.mesh", 9.1, 1.35, -1, boardNode);
}

void Chess::setupBlackPieces()
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    
    float pawnX = 1.1;
    float pawnZ = -13.0f;
    float pawnY = 0.65;
    
    blackPawn1 = new Piece("blackPawn1", "PawnA01_black.mesh", pawnX, pawnY, pawnZ, boardNode);
    blackPawn2 = new Piece("blackPawn2", "PawnA01_black.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    blackPawn3 = new Piece("blackPawn3", "PawnA01_black.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    blackPawn4 = new Piece("blackPawn4", "PawnA01_black.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    blackPawn5 = new Piece("blackPawn5", "PawnA01_black.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    blackPawn6 = new Piece("blackPawn6", "PawnA01_black.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    blackPawn7 = new Piece("blackPawn7", "PawnA01_black.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    blackPawn8 = new Piece("blackPawn8", "PawnA01_black.mesh", pawnX+=2, pawnY, pawnZ, boardNode);
    blackRook1 = new Piece("blackRook1", "TowerA01_black.mesh", 1.1, 0.8, -15, boardNode);
    blackRook2 = new Piece("blackRook2", "TowerA01_black.mesh", 15.1, 0.8, -15, boardNode);
    blackKnight1 = new Piece("blackKnight1", "HorseA01_black.mesh", 3.1, 0.7f, -15, boardNode);
    blackKnight2 = new Piece("blackKnight2", "HorseA01_black.mesh", 13.1, 0.7, -15, boardNode);
    //    blackKnight2->node->roll(Degree(-90));
    blackBishop1 = new Piece("blackBishop1", "Bishop_black.mesh", 5.1, 0.6, -15, boardNode);
    blackBishop2 = new Piece("blackBishop2", "Bishop_black.mesh", 11.1, 0.6, -15, boardNode);
    //    blackBishop2->node->yaw(Degree(90));
    blackQueen = new Piece("blackQueen", "QueenA_black.mesh", 7.1, 1.5, -15, boardNode);
    blackKing = new Piece("blackKing", "King_black.mesh", 9.1, .25, -15, boardNode);
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

//    printf("x: %d, y: %d\n", e.state.X.abs, e.state.Y.abs);

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

    Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
    Ogre::RaySceneQueryResult::iterator iter = result.begin();
    
    for (iter = result.begin( ); iter != result.end(); iter++) {
        
        Node *clickedNode = iter->movable->getParentNode();
        
        if(clickedNode->getName() != "boardNode") {
            printf("%s", clickedNode->getName().c_str());
            
            if(selectedNode) {
                deselectedNodes.push_back(selectedNode);
            }

            selectedNode = clickedNode;
            
//            Entity *entity = selectedNode->
//            entity->setMaterialName("red");
            
            system->playSound(pieceSound2, 0, false, &channel);
           
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