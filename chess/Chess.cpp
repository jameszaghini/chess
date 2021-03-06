#include "Chess.h"

#include <OgreLight.h>
#include <OgreWindowEventUtilities.h>
#include "OgreText.h"
#include <boost/algorithm/string.hpp>

bool Chess::cmpf(float A, float B, float epsilon)
{
    return (fabs(A - B) < epsilon);
}

Chess::Chess()
{   
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
    
    stockfish = Stockfish();
    stockfish.sendMessage("isready\n");
    stockfish.sendMessageNoResponse("ucinewgame\n");
    stockfish.sendMessageNoResponse("position startpos\n");
}

Chess::~Chess()
{
    delete OgreFramework::getSingletonPtr();
}

void Chess::startDemo()
{
	new OgreFramework();
    if(!OgreFramework::getSingletonPtr()->initOgre("Chess", this, this)) {
		return;
    }
    
	m_bShutdown = false;
    
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");
	  
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
    
    for(Piece *piece : piecesToMove) {
        
        Ogre::Vector3 pos = piece->node->getPosition();
        Ogre::Real currentX = pos.x;
        Ogre::Real currentY = pos.y;
        Ogre::Real currentZ = pos.z;
//        std::cout << "---------------------------------" << std::endl;
//        std::cout << currentX << ":" << currentY << ":" << currentZ << std::endl;
//        std::cout << piece->destinationX << ":" << piece->destinationY << ":" << piece->destinationZ << std::endl;
//        std::cout << "---------------------------------" << std::endl;
        
        bool removeFromPiecesToMove = true;
        if(!cmpf(currentX, piece->destinationX)) {
            if(currentX > piece->destinationX) {
                currentX -= 0.1;
                removeFromPiecesToMove = false;
            } else {
                currentX += 0.1;
                removeFromPiecesToMove = false;
            }
        }
        if(!cmpf(currentY, piece->destinationY)) {
            if(currentY > piece->destinationY) {
                currentY -= 0.1;
                removeFromPiecesToMove = false;
            } else {
                currentY += 0.1;
                removeFromPiecesToMove = false;
            }
        }
        if(!cmpf(currentZ, piece->destinationZ)) {
            if(currentZ > piece->destinationZ) {
                currentZ -= 0.1;
                removeFromPiecesToMove = false;
            } else {
                currentZ += 0.1;
                removeFromPiecesToMove = false;
            }
        }
        
        if(removeFromPiecesToMove) {
            piecesToMove.erase(std::remove(piecesToMove.begin(), piecesToMove.end(), piece), piecesToMove.end());
        }
        
        piece->node->setPosition(currentX, currentY, currentZ);
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
       
    whitePawn1 = new Pawn("whitePawn1", "PawnA01.mesh", board->getSquareWithCoordinates("a", "2"),  pieces);
    whitePawn2 = new Pawn("whitePawn2", "PawnA01.mesh", board->getSquareWithCoordinates("b", "2"), pieces);
    whitePawn3 = new Pawn("whitePawn3", "PawnA01.mesh", board->getSquareWithCoordinates("c", "2"), pieces);
    whitePawn4 = new Pawn("whitePawn4", "PawnA01.mesh", board->getSquareWithCoordinates("d", "2"), pieces);
    whitePawn5 = new Pawn("whitePawn5", "PawnA01.mesh", board->getSquareWithCoordinates("e", "2"), pieces);
    whitePawn6 = new Pawn("whitePawn6", "PawnA01.mesh", board->getSquareWithCoordinates("f", "2"), pieces);
    whitePawn7 = new Pawn("whitePawn7", "PawnA01.mesh", board->getSquareWithCoordinates("g", "2"), pieces);
    whitePawn8 = new Pawn("whitePawn8", "PawnA01.mesh", board->getSquareWithCoordinates("h", "2"), pieces);
    whiteRook1 = new Rook("whiteRook1", "TowerA01.mesh", board->getSquareWithCoordinates("a", "1"), pieces);
    whiteRook2 = new Rook("whiteRook2", "TowerA01.mesh", board->getSquareWithCoordinates("h", "1"), pieces);
    whiteKnight1 = new Knight("whiteKnight1", "HorseA01.mesh", board->getSquareWithCoordinates("b", "1"), pieces);
    whiteKnight2 = new Knight("whiteKnight2", "HorseA01.mesh", board->getSquareWithCoordinates("g", "1"), pieces);
    whiteBishop1 = new Bishop("whiteBishop1", "Bishop.mesh", board->getSquareWithCoordinates("c", "1"), pieces);
    whiteBishop2 = new Bishop("whiteBishop2", "Bishop.mesh", board->getSquareWithCoordinates("f", "1"), pieces);
    whiteBishop2->node->yaw(Degree(90));
    whiteQueen = new Queen("whiteQueen", "QueenA.mesh", board->getSquareWithCoordinates("e", "1"), pieces);
    whiteKing = new King("whiteKing", "King_black.mesh", board->getSquareWithCoordinates("d", "1"), pieces);
    whiteKing->entity->setMaterialName("white");
}

void Chess::setupBlackPieces()
{
    Ogre::SceneManager *sManager = OgreFramework::getSingletonPtr()->m_pSceneMgr;
    
    blackPawn1 = new Pawn("blackPawn1", "PawnA01_black.mesh", board->getSquareWithCoordinates("a", "7"), pieces);
    blackPawn2 = new Pawn("blackPawn2", "PawnA01_black.mesh", board->getSquareWithCoordinates("b", "7"), pieces);
    blackPawn3 = new Pawn("blackPawn3", "PawnA01_black.mesh", board->getSquareWithCoordinates("c", "7"), pieces);
    blackPawn4 = new Pawn("blackPawn4", "PawnA01_black.mesh", board->getSquareWithCoordinates("d", "7"), pieces);
    blackPawn5 = new Pawn("blackPawn5", "PawnA01_black.mesh", board->getSquareWithCoordinates("e", "7"), pieces);
    blackPawn6 = new Pawn("blackPawn6", "PawnA01_black.mesh", board->getSquareWithCoordinates("f", "7"), pieces);
    blackPawn7 = new Pawn("blackPawn7", "PawnA01_black.mesh", board->getSquareWithCoordinates("g", "7"), pieces);
    blackPawn8 = new Pawn("blackPawn8", "PawnA01_black.mesh", board->getSquareWithCoordinates("h", "7"), pieces);
    blackRook1 = new Rook("blackRook1", "TowerA01_black.mesh", board->getSquareWithCoordinates("a", "8"), pieces);
    blackRook2 = new Rook("blackRook2", "TowerA01_black.mesh", board->getSquareWithCoordinates("h", "8"), pieces);
    blackKnight1 = new Knight("blackKnight1", "HorseA01_black.mesh", board->getSquareWithCoordinates("b", "8"), pieces);
    blackKnight2 = new Knight("blackKnight2", "HorseA01_black.mesh", board->getSquareWithCoordinates("g", "8"), pieces);
    blackBishop1 = new Bishop("blackBishop1", "Bishop_black.mesh", board->getSquareWithCoordinates("c", "8"), pieces);
    blackBishop2 = new Bishop("blackBishop2", "Bishop_black.mesh", board->getSquareWithCoordinates("f", "8"), pieces);
    blackBishop2->node->yaw(Degree(90));
    blackQueen = new Queen("blackQueen", "QueenA_black.mesh", board->getSquareWithCoordinates("e", "8"), pieces);
    blackKing = new King("blackKing", "King_black.mesh", board->getSquareWithCoordinates("d", "8"), pieces);
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
                    
                    std::string from = selectedPiece->square->name;
                    std::string to = selectedSquare->name;
                    
                    stockfish.sendMove(from + to);                    
                    
                    piecesToMove.push_back(selectedPiece);
                    selectedPiece->moveToSquare(selectedSquare);
                    
                    selectedPiece->deselect();
                    selectedPiece = nullptr;
                }
            }
                        
            for(Piece *piece : pieces) {
                
                if(piece->square->name == stockfish.moveFrom) {
                    piecesToMove.push_back(piece);
                    piece->moveToSquare(board->getSquarebyName(stockfish.moveTo));
                }
                
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