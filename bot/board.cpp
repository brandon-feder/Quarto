
// Board class that holds all the info about each game state
class Board
{
    public:
        std::array<char, 16> board; // The board itself
        std::vector<char> pieces; // A list of available pieces
        std::vector<char> places; // A list of available places
        char pieceToGive; // The piece that will be given this move
        short score = 0; // The score of this game state
        short calcedScore = 0; // The |score|
        bool won; // Boolean to as to whether this is a winning state
        bool isLeaf = false; // Whether it is a leaf of the tree
        unsigned __int128 id; // The id of the state

        Board() {} // Default constructor
        ~Board() {} // Default destructor

        // Constructor to create the board and populate the proper variables
        Board(std::array<char, 16> I_board, std::vector<char> I_pieces, std::vector<char> I_places)
        {
            board = I_board;
            pieces = I_pieces;
            places = I_places;
            id = getId();
            calcedScore = calcScore();
            won = calcedScore >= 100;
        }

        // Creates a new board and returns it from the given play
        Board play(char piece, char i, char toGive)
        {
            // New boards board, available pieces, and available places
            std::array<char, 16> nBoard = board;
            std::vector<char> nPieces = pieces;
            std::vector<char> nPlaces = places;

            // Update the board, pieces, and places
            nBoard[i] = piece;
            nPieces.erase(std::remove(nPieces.begin(), nPieces.end(), piece), nPieces.end());
            nPlaces.erase(std::remove(nPlaces.begin(), nPlaces.end(), i), nPlaces.end());

            // Create the new board
            Board newBoard = Board(nBoard, nPieces, nPlaces);
            newBoard.pieceToGive = toGive;
            
            // Return the new board
            return newBoard;
        }

        // Gets the id of the current board
        unsigned __int128 getId()
        {
            unsigned __int128  M = 0; // Will hold the id with the largest value found

            // For every symetry
            for(std::array<int, 16> symetry : symetries)
            {   
                // Get the ID given that symetry
                unsigned __int128 ID = calcID(symetry);
                
                // Replace the new max if the found ID is greater
                M = (ID > M) ? ID : M;
            }

            // Return the id with a extra info about the piece given
            return M*100 + pieceToGive+1;   
        }

    private:
        // Determines if there is a pattern in the three pieces given
        short getPattern(char a, char b, char c)
        {
            if(a == -1 || b == -1 || c == -1 ) // If any of the pieces are not pieces (empty locations)
                return -1; // Return -1 meaning no pattern

            // Array of all patterns
            std::array<std::array<char, 8>, 4> patterns = {{
                {0, 1, 2, 3, 4, 5, 6, 7},
                {0, 1, 2, 3, 8, 9, 10, 11},
                {0, 1, 4, 5, 8, 9, 12, 13},
                {0, 2, 4, 6, 8, 10, 12, 14}
            }};

            // For every pattern
            for(char p = 0; p < 4; ++p)
            {
                if (
                    std::binary_search(patterns[p].begin(), patterns[p].end(), a) &&
                    std::binary_search(patterns[p].begin(), patterns[p].end(), b) &&
                    std::binary_search(patterns[p].begin(), patterns[p].end(), c)
                ) // If all pieces are in the same pattern
                {
                    return p; // Return the patterns id
                } else if(
                    !std::binary_search(patterns[p].begin(), patterns[p].end(), a) &&
                    !std::binary_search(patterns[p].begin(), patterns[p].end(), b) &&
                    !std::binary_search(patterns[p].begin(), patterns[p].end(), c)
                ) // If all pieces are not in the same pattern
                {
                    return 4+p; // Return the patterns id
                }
            }

            // If at this point, return -1 meaning no pattern
            return -1;
        }

        // Calculates the id given a list of indecies that correspond to some symetry
        unsigned __int128 calcID(std::array<int, 16> indecies)
        {
            unsigned __int128 ID = 0; // The ID itself

            // For every indecie, add i to the ID
            for(int i = 0; i < 16; ++i)
            {
                ID*=100;
                ID += board[indecies[i]]+1;
            }
        
            return ID; // Return the ID
        }

        // Calculates the abolute value of the score of the state
        short calcScore()
        {
            // Array of all sequences in the board:
                // 0  1  2  3
                // 4  5  6  7
                // 8  9  10 11
                // 12 13 14 15
            std::array<std::array<std::array<int, 3>, 10>, 4> sequences = {{
                {{
                    {1, 2, 3},
                    {5, 6, 7},
                    {9, 10, 11},
                    {13, 14, 15},
                    {4, 8, 12},
                    {5, 9, 13},
                    {6, 10, 14},
                    {7, 11, 15},
                    {5, 10, 15},
                    {6, 9, 12}
                }}, {{
                    {0, 1, 2},
                    {4, 5, 6},
                    {8, 9, 10},
                    {12, 13, 14},
                    {0, 4, 8},
                    {1, 5, 9},
                    {2, 6, 10},
                    {3, 7, 11},
                    {0, 5, 10},
                    {3, 6, 9}
                }}, {{ 
                    {0, 2, 3},
                    {4, 6, 7},
                    {8, 10, 11},
                    {12, 14, 15},
                    {0, 8, 12},
                    {1, 9, 13},
                    {2, 10, 14},
                    {3, 11, 15},
                    {0, 10, 15},
                    {3, 9, 12}
                }}, {{
                    {0, 1, 3},
                    {4, 5, 7},
                    {8, 9, 11},
                    {12, 13, 15},
                    {0, 4, 12},
                    {1, 5, 13},
                    {2, 6, 14},
                    {3, 7, 15},
                    {0, 5, 15},
                    {3, 6, 12}
                }}
            }};

            short score = 0; // The score itself

            // For every 10 sequences
            for(int i = 0; i < 10; ++i)
            {
                // Array containing the patterns that were found
                std::array<short, 8> patternsFound = {0, 0, 0, 0, 0, 0, 0, 0};

                // For every 4 in that sequence
                for(int j = 0; j < 4; ++j)
                {   
                    // Test if there is a pattern and get the id of that pattern
                    short p = getPattern(
                        board[sequences[j][i][0]],
                        board[sequences[j][i][1]],
                        board[sequences[j][i][2]]
                    );

                    // If there is a pattern
                    if(p != -1)
                    {
                        patternsFound[p] += 1; // Add 1 to the correct index
                    }
                }  

                // For every count of some pattern id
                for(short count : patternsFound)
                {   
                
                    if(count > 1) // If is a pattern of 4 (winning state), add 100 to the score
                        score += 100;
                    else // Otherwise, just add the count (0 or 1)
                        score += count;
                } 
            }

            return score; // Return the score
        }
};
