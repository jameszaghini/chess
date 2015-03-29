//
//  Stockfish.cpp
//  chess
//
//  Created by James Zaghini on 27/03/2015.
//
//

#include "Stockfish.h"
#include <stdio.h>
#include <iostream>
#include <cctype>

// https://jineshkj.wordpress.com/2006/12/22/how-to-capture-stdin-stdout-and-stderr-of-child-program/

/* since pipes are unidirectional, we need two pipes.
 one for data to flow from parent's stdout to child's
 stdin and the other for child's stdout to flow to
 parent's stdin */

#define PARENT_WRITE_PIPE  0
#define PARENT_READ_PIPE   1

// always in a pipe[], pipe[0] is for read and pipe[1] is for write
#define READ_FD  0
#define WRITE_FD 1

#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )

#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )

Stockfish::Stockfish() {
    
    gameState = "position startpos moves";
    
    // pipes for parent to write and read
    pipe(pipes[PARENT_READ_PIPE]);
    pipe(pipes[PARENT_WRITE_PIPE]);
    
    pid = fork();
    
    if(!pid) {
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
        
        /* close fds not required by parent */
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        
        readResponse();        
    }
}

std::string Stockfish::readResponse()
{
    int count = read(PARENT_READ_FD, buffer, sizeof(buffer)-1);
    if (count >= 0) {
        buffer[count] = 0;
        std::string stringBuffer = std::string(buffer);
        std::cout << stringBuffer << std::endl;
        return stringBuffer;
    } else {
        printf("IO Error\n");
    }
    
}

void Stockfish::sendMessageNoResponse(std::string toWrite)
{
    write(PARENT_WRITE_FD, toWrite.c_str(), toWrite.length());
}

std::string Stockfish::sendMessage(const std::string &toWrite)
{
    write(PARENT_WRITE_FD, toWrite.c_str(), toWrite.length());
    readResponse();
}

void Stockfish::sendMove(const std::string &move)
{
    gameState += " " + toLower(move);
    std::cout << gameState << std::endl;
    sendMessageNoResponse(gameState + "\n");
    readMove();
}

void Stockfish::readMove()
{
    sendMessageNoResponse("go depth 1\n");
    moveEngineSaidToDo = readResponse();
}


Stockfish::~Stockfish() {
    
}

std::string Stockfish::toLower(const std::string& s)
{
    std::string result;
    
    std::locale loc;
    for (unsigned int i = 0; i < s.length(); ++i)
    {
        result += std::tolower(s.at(i), loc);
    }
    
    return result;
}