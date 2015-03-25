//|||||||||||||||||||||||||||||||||||||||||||||||

#include "Chess.h"

#include <OgreLight.h>
#include <OgreWindowEventUtilities.h>
#include "OgreText.h"

/* since pipes are unidirectional, we need two pipes.
 one for data to flow from parent's stdout to child's
 stdin and the other for child's stdout to flow to
 parent's stdin */

#define NUM_PIPES          2

#define PARENT_WRITE_PIPE  0
#define PARENT_READ_PIPE   1

int pipes[NUM_PIPES][2];

/* always in a pipe[], pipe[0] is for read and
 pipe[1] is for write */
#define READ_FD  0
#define WRITE_FD 1

#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )

#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )

void setupPipes()
{
    int outfd[2];
    int infd[2];
    
    // pipes for parent to write and read
    pipe(pipes[PARENT_READ_PIPE]);
    pipe(pipes[PARENT_WRITE_PIPE]);
    
    if(!fork()) {
        char *argv[]={ "/Users/james/Downloads/stockfish-6-mac/Mac/stockfish-6-64", 0};
        
        dup2(CHILD_READ_FD, STDIN_FILENO);
        dup2(CHILD_WRITE_FD, STDOUT_FILENO);
        
        /* Close fds not required by child. Also, we don't
         want the exec'ed program to know these existed */
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(PARENT_READ_FD);
        close(PARENT_WRITE_FD);
        
        execv(argv[0], argv);
    } else {
        char buffer[4096];
        int count;
        
        /* close fds not required by parent */
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        
        count = read(PARENT_READ_FD, buffer, sizeof(buffer)-1);
        if (count >= 0) {
            buffer[count] = 0;
            printf("\n\n-----------------------\n%s\n-----------------------\n\n", buffer);
        } else {
            printf("IO Error\n");
        }
        
        // Write to child’s stdin
        write(PARENT_WRITE_FD, "uci\n", 4);

        // Read from child’s stdout
        count = read(PARENT_READ_FD, buffer, sizeof(buffer)-1);
        if (count >= 0) {
            buffer[count] = 0;
            printf("---%s---", buffer);
        } else {
            printf("IO Error\n");
        }
       
        write(PARENT_WRITE_FD, "go depth 1\n", 11);
        count = read(PARENT_READ_FD, buffer, sizeof(buffer)-1);
        if (count >= 0) {
            buffer[count] = 0;
            printf("\n\n-----------------------\n%s\n-----------------------\n\n", buffer);
        } else {
            printf("IO Error\n");
        }
       
        write(PARENT_WRITE_FD, "go depth 1\n", 11);
        close(PARENT_WRITE_FD);
        count = read(PARENT_READ_FD, buffer, sizeof(buffer)-1);
        if (count >= 0) {
            buffer[count] = 0;
            printf("\n\n-----------------------\n%s\n-----------------------\n\n", buffer);
        } else {
            printf("IO Error\n");
        }
    }
}

Chess::Chess()
{
    setupPipes();
    
    selectedNode = 0;

    board = 0;
    
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
            if(position.y < 0.26) {
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
    spotLight->setPosition(Ogre::Vector3(10, 28, 8));
    spotLight->setSpotlightRange(Ogre::Degree(10), Ogre::Degree(40));
    
    Ogre::Light* spotLight2 = sManager->createLight("spotLight2");
    spotLight2->setType(Ogre::Light::LT_SPOTLIGHT);
    spotLight2->setDiffuseColour(0.7, 0.2, 0.1);
    spotLight2->setSpecularColour(0.8, 0.8, 0.8);
    spotLight2->setDirection(-1, -1, 0);
    spotLight2->setPosition(Ogre::Vector3(18, 28, 8));
    spotLight2->setSpotlightRange(Ogre::Degree(10), Ogre::Degree(40));
    
    sManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

}

void Chess::setupBoard()
{
    board = new Board();
}

void Chess::setupWhitePieces()
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
       
    whitePawn1 = new Piece("whitePawn1", "PawnA01.mesh", board->getSquareWithCoordinates("A", "2"),  pieces);
    whitePawn2 = new Piece("whitePawn2", "PawnA01.mesh", board->getSquareWithCoordinates("B", "2"), pieces);
    whitePawn3 = new Piece("whitePawn3", "PawnA01.mesh", board->getSquareWithCoordinates("C", "2"), pieces);
    whitePawn4 = new Piece("whitePawn4", "PawnA01.mesh", board->getSquareWithCoordinates("D", "2"), pieces);
    whitePawn5 = new Piece("whitePawn5", "PawnA01.mesh", board->getSquareWithCoordinates("E", "2"), pieces);
    whitePawn6 = new Piece("whitePawn6", "PawnA01.mesh", board->getSquareWithCoordinates("F", "2"), pieces);
    whitePawn7 = new Piece("whitePawn7", "PawnA01.mesh", board->getSquareWithCoordinates("G", "2"), pieces);
    whitePawn8 = new Piece("whitePawn8", "PawnA01.mesh", board->getSquareWithCoordinates("H", "2"), pieces);
    whiteRook1 = new Piece("whiteRook1", "TowerA01.mesh", board->getSquareWithCoordinates("A", "1"), pieces);
    whiteRook2 = new Piece("whiteRook2", "TowerA01.mesh", board->getSquareWithCoordinates("H", "1"), pieces);
    whiteKnight1 = new Piece("whiteKnight1", "HorseA01.mesh", board->getSquareWithCoordinates("B", "1"), pieces);
    whiteKnight2 = new Piece("whiteKnight2", "HorseA01.mesh", board->getSquareWithCoordinates("G", "1"), pieces);
    whiteBishop1 = new Piece("whiteBishop1", "Bishop.mesh", board->getSquareWithCoordinates("C", "1"), pieces);
    whiteBishop2 = new Piece("whiteBishop2", "Bishop.mesh", board->getSquareWithCoordinates("F", "1"), pieces);
    whiteBishop2->node->yaw(Degree(90));
    whiteQueen = new Piece("whiteQueen", "QueenA.mesh", board->getSquareWithCoordinates("E", "1"), pieces);
    whiteKing = new Piece("whiteKing", "King_black.mesh", board->getSquareWithCoordinates("D", "1"), pieces);
    whiteKing->entity->setMaterialName("white");
}

void Chess::setupBlackPieces()
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    
    blackPawn1 = new Piece("blackPawn1", "PawnA01_black.mesh", board->getSquareWithCoordinates("A", "7"), pieces);
    blackPawn2 = new Piece("blackPawn2", "PawnA01_black.mesh", board->getSquareWithCoordinates("B", "7"), pieces);
    blackPawn3 = new Piece("blackPawn3", "PawnA01_black.mesh", board->getSquareWithCoordinates("C", "7"), pieces);
    blackPawn4 = new Piece("blackPawn4", "PawnA01_black.mesh", board->getSquareWithCoordinates("D", "7"), pieces);
    blackPawn5 = new Piece("blackPawn5", "PawnA01_black.mesh", board->getSquareWithCoordinates("E", "7"), pieces);
    blackPawn6 = new Piece("blackPawn6", "PawnA01_black.mesh", board->getSquareWithCoordinates("F", "7"), pieces);
    blackPawn7 = new Piece("blackPawn7", "PawnA01_black.mesh", board->getSquareWithCoordinates("G", "7"), pieces);
    blackPawn8 = new Piece("blackPawn8", "PawnA01_black.mesh", board->getSquareWithCoordinates("H", "7"), pieces);
    blackRook1 = new Piece("blackRook1", "TowerA01_black.mesh", board->getSquareWithCoordinates("A", "8"), pieces);
    blackRook2 = new Piece("blackRook2", "TowerA01_black.mesh", board->getSquareWithCoordinates("H", "8"), pieces);
    blackKnight1 = new Piece("blackKnight1", "HorseA01_black.mesh", board->getSquareWithCoordinates("B", "8"), pieces);
    blackKnight2 = new Piece("blackKnight2", "HorseA01_black.mesh", board->getSquareWithCoordinates("G", "8"), pieces);
    blackBishop1 = new Piece("blackBishop1", "Bishop_black.mesh", board->getSquareWithCoordinates("C", "8"), pieces);
    blackBishop2 = new Piece("blackBishop2", "Bishop_black.mesh", board->getSquareWithCoordinates("F", "8"), pieces);
    blackBishop2->node->yaw(Degree(90));
    blackQueen = new Piece("blackQueen", "QueenA_black.mesh", board->getSquareWithCoordinates("E", "8"), pieces);
    blackKing = new Piece("blackKing", "King_black.mesh", board->getSquareWithCoordinates("D", "8"), pieces);
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
    
    using namespace std;
    for (iter = result.begin( ); iter != result.end(); iter++) {
        
        Node *clickedNode = iter->movable->getParentNode();
        
        if(clickedNode->getName() != "boardNode") {
            
            string clickedNodeName = clickedNode->getName();
            cout << clickedNodeName << endl;

            int squareNameLength = 2;
            
            if(clickedNodeName.length() == squareNameLength) {
                if(selectedPiece) {
                    selectedSquare = board->getSquarebyName(clickedNodeName);
                    selectedPiece->moveToSquare(selectedSquare);
                    selectedPiece->deselect();
                    selectedPiece = nullptr;
                }
            }
            
            for(Piece *piece : pieces) {
                
                string pieceName = piece->node->getName();
                if(selectedPiece && pieceName.compare(selectedPiece->name) == 0) {
                    piece->deselect();
                    continue;
                }
                
                if(pieceName.compare(clickedNodeName) == 0) {
                    selectedPiece = piece;
                    piece->select();
                }
            }

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