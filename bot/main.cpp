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

void getInput();
void genTree(short movesAhead, char piece, unsigned __int128 boardIndx);
void minimax(short depth, unsigned __int128 boardId);
unsigned __int128 getBestPlay();

std::string printGragh(unsigned __int128 boardId, int depth)
{
    std::string str = std::string( 2*depth, ' ' ) + "(" + std::to_string(boardMap[boardId].score) + ", " + std::to_string(depth) + ")\n";
    for(unsigned __int128 i : gragh[boardId])
    {
        str += printGragh(i, depth+1);
    }
    return str;
}


int main( void )
{
    getInput();

    if(!B.won)
    {
        std::cout << "Generating Tree" << std::endl;
        genTree(6, toPlay, B.id);
        std::cout << "Tree has " << boardMap.size() << " nodes." << std::endl;

        std::cout << "Searching Tree" << std::endl;
        minimax(0, B.id);
        std::cout << B.score << std::endl;

        unsigned __int128 best = getBestPlay();
        boardMap[best].print();
        std::cout << (int)boardMap[best].pieceToGive << std::endl;

        std::ofstream out;
        out.open("../debug.out");
        out << printGragh(B.id, 0);
        out.close();

    } else
    {
        std::cout << "Opponent Won!" << std::endl;
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

void genTree(short movesAhead, char toPlay, unsigned __int128 boardId)
{
    Board board = boardMap[boardId];
    if(movesAhead > 0)
    {
        for(char place : board.places)
        {
            for(char piece : board.pieces)
            {
                if(piece != toPlay)
                {
                    Board nBoard = board.play(toPlay, place, piece);
                    if(!visited[nBoard.id])
                    {
                        boardMap[nBoard.id] = nBoard;
                        visited[nBoard.id] = true;
                        gragh[boardId].push_back(nBoard.id);

                        if( nBoard.won || movesAhead == 1)
                        {
                            boardMap[nBoard.id].isLeaf = true;
                        } else
                        {
                            genTree(movesAhead-1, piece, nBoard.id);
                        }
                    }
                }
            }
        }
    } else
    {
        board.isLeaf = true;
    }
}

void minimax(short depth, unsigned __int128 boardId)
{
    bool isBotTurn = depth%2;
    Board *board = &boardMap[boardId];

    if((gragh[board->id].size() == 0 || board->isLeaf) && board->won)
    {
        board->score = isBotTurn?1:-1;
    } else if((gragh[board->id].size() == 0 || board->isLeaf) && !board->won)
    {
        board->score = 0;
    } else
    {
        int m = -2;
        for(unsigned __int128 childId : gragh[boardId])
        {
            minimax(depth+1, childId);
            int childScore = boardMap[childId].score;

            if(
                m == -2 ||
                (isBotTurn && childScore < m) ||
                (!isBotTurn && childScore > m)
            )
            {
                m = childScore;
            }
        }

        if(m == -2)
        {
            board->print();
            std::cout << gragh[board->id].size();
        }

        board->score = m;
    }
}

unsigned __int128 getBestPlay()
{
    bool isFirst = true;
    unsigned __int128 max = 0;
    for(unsigned __int128 child : gragh[B.id])
    {
        if(isFirst || boardMap[child].score > boardMap[max].score)
        {
            max = child;
            isFirst = false;
        }
        // std::cout << boardMap[child].score << std::endl;
        // boardMap[child].print();
    }
    return max;
}
