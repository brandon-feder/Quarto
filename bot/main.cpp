#include <iostream>
#include <json/json.h>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <algorithm>
#include <climits>
#include <utility>
#include <stdlib.h>
#include <tuple>
#include <math.h>

Json::Value settings; // Contains the settings from the json file in ./settings

#include "helpers.cpp"
#include "board.cpp"

Board B; // The current board
char toPlay; // Th epiece that the opponent wants to be played
std::map<unsigned __int128, std::vector<unsigned __int128>> gragh; // An adjacency list for the tree/directed gragh
std::map<unsigned __int128, Board> boardMap; // Map from id to the actual game state
std::map<unsigned __int128, bool> visited; // Tells whether the id has already been visisted

void getInput(); // Gets the input from play.in and settings/settings.json
void genTree(short movesAhead, char piece, unsigned __int128 boardIndx); // Generates the tree/directed gragh
void minimax(short depth, unsigned __int128 boardId); // Performs a minimax
unsigned __int128 getBestPlay(); // Gets the best play

// TODO: Remove
std::string printGragh(unsigned __int128 boardId, int depth)
{
    std::string str = std::string( 2*depth, ' ' ) + "(" + std::to_string(boardMap[boardId].score) + ", " + std::to_string(depth) + ", " + std::to_string(boardMap[boardId].pieceToGive)  + ")\n";
    for(unsigned __int128 i : gragh[boardId])
        str += printGragh(i, depth+1);

    return str;
}


int main()
{
    // Set random seed 
    srand (time(NULL));

    // Get the input
    getInput();


    debugPrint("Original Board:" + B.toText());

    // If the bot has the first turn
    if(toPlay == -1)
        debugPrint("Giving: " + std::to_string(rand()%16) + "\n");

    // If the game is not already won
    else if(!B.won)
    {
        debugPrint("Generating Tree\n");

        // Get the depth to search from the settings
        short depth = settings["depths"][(int)(16 - B.pieces.size())].asInt();

        debugPrint("Depth Index: " + std::to_string((int)(16 - B.pieces.size())) + "\n");
        debugPrint("Depth: " + std::to_string(depth) + "\n");

        // Generate the tree (Directed gragh)
        genTree(depth, toPlay, B.id);

        debugPrint("Tree has " + std::to_string(boardMap.size()) + " nodes\n");

        debugPrint("Searching Tree\n");

        // Perform a minimax to calculate each gamestate's score
        minimax(0, B.id);

        // Get the best next gamestate
        unsigned __int128 best = getBestPlay();
        debugPrint(boardMap[best].toText());

        // If the best gamestate is a winning state
        if(boardMap[best].won)
            debugPrint("Bot Won\n");

        // If the best is a lossing state
        else if(boardMap[best].score == -1)
            std::cout << "Human Won" << std::endl;

        // Otherwise
        else
            debugPrint("Give Piece: " + std::to_string((int)boardMap[best].pieceToGive) + "\n");
    } else // If human already won 
        debugPrint("Human Won\n");

    return 0;
}


void getInput()
{
    // Setup file for being read
    std::ifstream in;
    in.open("../play.in");

    // Buffer to hold the raw board, pieces that can be played, and places that can be used
    std::array<char, 16> board;
    std::vector<char> pieces = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<char> places;

    // Get the piece that the opponent wants the bot to play
    short temp;
    in >> temp;
    toPlay = (char)temp;

    // Read in the board
    for(int i = 0; i < 16; ++i)
    {
        in >> temp;
        board[i] = (char)temp;
        if(board[i] == -1)
            places.push_back(i);
        else
            pieces.erase(std::remove(pieces.begin(), pieces.end(), board[i]), pieces.end());
    }

    // Sort the pieces and places in sequential order to allow for binary_search later
    sort(pieces.begin(), pieces.end());
    sort(places.begin(), places.end());

    // Create the board object and add to the map
    B = Board(board, pieces, places);
    boardMap[B.id] = B;
    visited[B.id] = true;

    // Read in the settings
    std::ifstream settings_json("../settings/settings.json");
    settings_json >> settings;
}

void genTree(short movesAhead, char toPlay, unsigned __int128 boardId)
{
    Board board = boardMap[boardId]; // Helper variable to store the current board
    bool isChildren = false; // Flag as to whether the current node has any children
    if(movesAhead > 0 && !boardMap[boardId].won) // If not at max depth and not already won
    {
        // For every piece and place that can be played
        for(char place : board.places)
        {
            for(char piece : board.pieces)
            {
                if(piece != toPlay)
                {
                    // Generate a new game state
                    Board nBoard = board.play(toPlay, place, piece);

                    // If the game state is not already visited
                    if(!visited[nBoard.id])
                    {
                        // Add state to all maps
                        boardMap[nBoard.id] = nBoard;
                        visited[nBoard.id] = true;
                        gragh[boardId].push_back(nBoard.id);
                        isChildren = true;

                        // Call this function again on all children
                        genTree(movesAhead-1, piece, nBoard.id);
                    }
                }
            }
        }
    }

    // Set the flat isLeaf depending on whether the current gamestate has children
    boardMap[boardId].isLeaf = !isChildren;
}

void minimax(short depth, unsigned __int128 boardId)
{
    bool isBotTurn = depth%2; // Helper variable for whether it is the bots turn or not
    Board *board = &boardMap[boardId]; // Pointer to board that score is being calculated for

    if(board->isLeaf) // If the board is a leaf
    {
        board->score = board->calcedScore*(isBotTurn?1:-1); // Give the board state its calculated score times -1 or 1 depending on the turn
    } else
    {
        int m = -2; // The max/min depending on the turn

        // For every child
        for(unsigned __int128 childId : gragh[boardId])
        {
            // Get the score for the child
            minimax(depth+1, childId);

            // Helper variable to store childs score
            int childScore = boardMap[childId].score;

            // Set m as new max/min
            if(
                m == -2 ||
                (isBotTurn && childScore < m) ||
                (!isBotTurn && childScore > m)
            )
            {
                m = childScore;
            }
        }

        // Set current boards score

        // Adding many levels will make score > 100 / < -100

        board->score = m;
    }
}

unsigned __int128 getBestPlay()
{
    short maxScore = -2; // Stores max score found 

    // For each child  
    for(unsigned __int128 childId : gragh[B.id])
    {
        // Set new max if child has a higher score
        if(maxScore == -2 || boardMap[childId].score > maxScore)
        {
            maxScore = boardMap[childId].score;
        }
    }

    std::vector<unsigned __int128> highestScores;

    // For each child
    for(unsigned __int128 childId : gragh[B.id])
    {
        // Set push child to vector if its score is one of the highest
        if(boardMap[childId].score == maxScore)
        {
            highestScores.push_back(childId);
            // debugPrint(boardMap[childId].toText());
        }
    }

    int index = rand()%highestScores.size();
    return highestScores[index];
}
