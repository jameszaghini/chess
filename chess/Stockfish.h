//
//  Stockfish.h
//  chess
//
//  Created by James Zaghini on 27/03/2015.
//
//

#ifndef __chess__Stockfish__
#define __chess__Stockfish__

#include <string>
#include <unistd.h>

#define NUM_PIPES 2

class Stockfish {

public:
    Stockfish();
    ~Stockfish();
    
    std::string sendMessage(const std::string &toWrite);
    void sendMessageNoResponse(std::string toWrite);

    void sendMove(const std::string &move);
    void readMove();
    
    void outputGameState();
    std::string toLower(const std::string& s);
    
    std::string readResponse();
    
    char buffer[4096];
    int count;
    
    pid_t pid;
    
    int outfd[2];
    int infd[2];
    
    int pipes[NUM_PIPES][2];
    
    std::string gameState;
    std::string moveFrom;
    std::string moveTo;
    
    
};

#endif /* defined(__chess__Stockfish__) */
