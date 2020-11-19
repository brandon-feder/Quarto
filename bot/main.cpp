#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <algorithm>
#include <climits>
#include <utility>
#include <tuple>
#include <math.h>

#include "settings.cpp"
#include "helpers.cpp"
#include "board.cpp"

Board B;
char toPlay;
std::map<unsigned __int128, std::vector<unsigned __int128>> gragh;
std::map<unsigned __int128, Board> boardMap;
std::map<unsigned __int128, bool> visited;

std::vector<long long> DEPTH = {3, 3, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5};

void getInput();
void genTree(int depth, int maxDepth, char piece, unsigned __int128 boardIndx);
void searchTree(int depth, int maxDepth, unsigned __int128 boardId);
std::pair<unsigned __int128, int> getBestPlay(unsigned __int128 boardId);
std::tuple<int, int, int> getBestPiece(unsigned __int128 boardId);

int main( void )
{
    getInput();

    if(!B.won)
    {
        int depth = DEPTH[16-B.pieces.size()];
        B.print();
        std::cout << "The Board ID is: " << B.id << std::endl;
        std::cout << "Depth is: " << depth << std::endl;
        std::cout << "Getting Gragh" << std::endl;
        genTree(0, depth-1, toPlay, B.id);

        std::cout << "Searching Gragh" << std::endl;
        searchTree(0, depth-1, B.id);

        std::cout << "Getting Best Board" << std::endl;
        std::pair<unsigned __int128, char> play = getBestPlay(B.id);
        boardMap[play.first].print();

        if(!boardMap[play.first].won)
        {
            int pieceToGive = play.second;
            std::cout << "Give piece: " << pieceToGive << std::endl;
        }
        else
        {
            std::cout << "Game Won!" << std::endl;
        }
    }
    return 0;
}


void getInput()
{
    // Get board input
    std::ifstream in;
    in.open("../board.in");

    // Read in board
    std::array<char, 16> board;

    std::vector<char> pieces = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<char> places;
    short temp;
    in >> temp;
    toPlay = (char)temp;
    for(int i = 0; i < 16; ++i)
    {
        in >> temp;
        board[i] = (char)temp;
        if(board[i] == -1)
            places.push_back(i);
        else
            pieces.erase(std::remove(pieces.begin(), pieces.end(), board[i]), pieces.end());
    }

    sort(pieces.begin(), pieces.end());
    sort(places.begin(), places.end());

    B = Board(board, pieces, places);
    boardMap[B.id] = B;
}

void genTree(int depth, int maxDepth, char toPlay,unsigned __int128 boardId)
{
    Board board = boardMap[boardId];
    if(!board.won)
    {
        for(int place = 0; place < board.places.size(); ++place)
        {
            Board newBoard = board.playPiece(board.places[place], toPlay);
            // newBoard.print();
            // std::cout << newBoard.won << std::endl;
            gragh[boardId].push_back(newBoard.id); // May cause problem

            if(!visited[newBoard.id])
            {
                boardMap[newBoard.id] = newBoard;
                visited[newBoard.id] = true;

                if(maxDepth-depth > 0)
                {
                    for(int piece = 0; piece < newBoard.pieces.size(); ++piece)
                    {
                        genTree(depth+1, maxDepth, newBoard.pieces[piece], newBoard.id);

                        if(boardMap[gragh[boardId][gragh[boardId].size()-1]].won)
                        {
                            boardMap[gragh[boardId][gragh[boardId].size()-1]].isLeaf = true;
                            // return; // Posibly will break program
                        }
                    }
                } else
                {
                    boardMap[gragh[boardId][gragh[boardId].size()-1]].isLeaf = true;
                }
            }
        }
    }
}

void searchTree(int depth, int maxDepth, unsigned __int128 boardId)
{
    bool isBotTurn = depth%2;

    if(boardMap[boardId].won)
    {
        boardMap[boardId].score = (isBotTurn)?1:-1;
    } else if(!boardMap[boardId].isLeaf)
    {
        int M = -2;
        for(int i = 0; i < gragh[boardId].size(); ++i)
        {
            searchTree(depth+1, maxDepth, gragh[boardId][i]);
            if(
                (M == -2) ||
                (isBotTurn && boardMap[gragh[boardId][i]].score > M) ||
                (!isBotTurn && boardMap[gragh[boardId][i]].score < M)
            )
            {
                M = boardMap[gragh[boardId][i]].score;
            }
        }
        boardMap[boardId].score = M;
    }
}

std::pair<unsigned __int128, int> getBestPlay(unsigned __int128 boardId)
{
    bool isFirst = true;
    unsigned __int128 bestBoard;
    int bestPiece;
    int bestPieceScore;
    int minSum;

    for(int i = 0; i < gragh[boardId].size(); ++i)
    {
        boardMap[gragh[boardId][i]].print();
        std::cout << boardMap[gragh[boardId][i]].score << std::endl;
        // if(boardMap[gragh[boardId][i]].score == 1)
        // {
        //     return std::pair<unsigned __int128, int>(gragh[boardId][i], -1);
        //
        // } else
        // {
            std::tuple<int, int, int> bestPieceFound = getBestPiece(gragh[boardId][i]);
            if(isFirst || std::get<1>(bestPieceFound) > bestPieceScore)
            {
                bestPiece = std::get<0>(bestPieceFound);
                bestPieceScore = std::get<1>(bestPieceFound);
                bestBoard = gragh[boardId][i];
                minSum = std::get<2>(bestPieceFound);
                isFirst = false;
            } else if( std::get<1>(bestPieceFound) == bestPieceScore &&  std::get<2>(bestPieceFound) > minSum)
            {
                bestPiece = std::get<0>(bestPieceFound);
                bestPieceScore = std::get<1>(bestPieceFound);
                bestBoard = gragh[boardId][i];
                minSum = std::get<2>(bestPieceFound);
            }
        // }
    }

    return std::pair<unsigned __int128, char>(bestBoard, bestPiece);
}

std::tuple<int, int, int> getBestPiece(unsigned __int128 boardId)
{
    std::map<int, short> pieceScores;
    for(int i = 0; i < gragh[boardId].size(); ++i)
    {
        pieceScores[boardMap[gragh[boardId][i]].piecePlayed] += boardMap[gragh[boardId][i]].score;
    }

    bool first = true;
    int maxKey = 0;
    int maxValue = 0;
    int sum = 0;
    for( std::map<int, short>::iterator it = pieceScores.begin(); it != pieceScores.end(); ++it )
    {
        // std::cout << it->first << " " << it->second << std::endl;
        if(first || it->second > maxValue)
        {
            maxKey = it->first;
            maxValue = it->second;
        }

        first = false;
        sum += it->second;
        std::cout << "    " << it->first << "  " << it->second << std::endl;
    }

    return std::tuple<int, int, int>{maxKey, maxValue, sum};
}
