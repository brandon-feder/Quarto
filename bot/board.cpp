
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

        std::string toText()
        {
            std::string txt = "\n____________________________________\n";
            txt += "==============\n";

            for(char i = 0; i < 16; ++i)
            {
                if((i) % 4 == 0)
                {
                    txt += "|";
                }

                txt += (board[i]<10?" ":"") + (board[i]==-1?"-":std::to_string(board[i])) + " ";

                if((i+1) % 4 == 0)
                {
                    txt += "|\n";
                }
            }
            txt += "==============\n";

            std::string p = "Pieces: ";
            std::string pl = "Places: ";

            for(char i = 0; i < pieces.size(); ++i)
            {
                p += std::to_string(pieces[i]) + " ";
            }

            for(char i = 0; i < places.size(); ++i)
            {
                pl += std::to_string((int)places[i]) + " ";
            }

            txt += p + "\n" + pl + "\n";
            txt += "Giving: " + std::to_string(pieceToGive) + "\n";
            txt += "won: " + std::to_string(won) + "\n";
            txt += "score: " + std::to_string(score) + "\n";
            txt += "isLeaf: " + std::to_string(isLeaf) + "\n";
            txt += "Debig Str: " + debugString + "\n";
            txt += "____________________________________\n\n";

            return txt;
        }

        unsigned __int128 getId()
        {
            unsigned __int128  M = 0;
            for(int i = 0; i < 8; ++i)
            {
                unsigned __int128 ID = calcID(symetries[i]);
                M = (ID > M) ? ID : M;
            }

            return M*100 + pieceToGive+1;
        }

    private:
        short getPattern(char a, char b, char c)
        {
            if(a == -1 || b == -1 || c == -1 )
                return -1;

            std::array<std::array<char, 8>, 4> patterns = {{
                {0, 1, 2, 3, 4, 5, 6, 7},
                {0, 1, 2, 3, 8, 9, 10, 11},
                {0, 1, 4, 5, 8, 9, 12, 13},
                {0, 2, 4, 6, 8, 10, 12, 14}
            }};

            for(char p = 0; p < 4; ++p)
            {
                if (
                    std::binary_search(patterns[p].begin(), patterns[p].end(), a) &&
                    std::binary_search(patterns[p].begin(), patterns[p].end(), b) &&
                    std::binary_search(patterns[p].begin(), patterns[p].end(), c)
                )
                {
                    return p;
                } else if(
                    !std::binary_search(patterns[p].begin(), patterns[p].end(), a) &&
                    !std::binary_search(patterns[p].begin(), patterns[p].end(), b) &&
                    !std::binary_search(patterns[p].begin(), patterns[p].end(), c)
                )
                {
                    return 4+p;
                }
            }

            return -1;
        }

        unsigned __int128 calcID(std::array<int, 16> indecies)
        {
            // temp++;
            // return temp;
            unsigned __int128 ID = 0;
            for(int i = 0; i < 16; ++i)
            {
                ID*=100;
                ID += board[indecies[i]]+1;
            }
            return ID;
        }

        short calcScore()
        {
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

            short score = 0;
            for(int i = 0; i < 10; ++i)
            {
                std::array<short, 8> patternsFound = {0, 0, 0, 0, 0, 0, 0, 0};
                for(int j = 0; j < 4; ++j)
                {
                    short p = getPattern(
                        board[sequences[j][i][0]],
                        board[sequences[j][i][1]],
                        board[sequences[j][i][2]]
                    );

                    if(p != -1)
                    {
                        patternsFound[p] += 1;
                    }
                }  

                for(short count : patternsFound)
                {
                    if(count > 1)
                        score += 100;
                    else
                        score += count;
                } 
            }

            return score;
        }
};
