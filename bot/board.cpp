int temp = -1;
class Board
{
    public:
        std::array<char, 16> board;
        std::vector<char> pieces;
        std::vector<char> places;
        char pieceToGive;
        short score = -2;
        bool won;
        bool isLeaf = false;
        unsigned __int128 id;

        Board() {}

        Board(std::array<char, 16> I_board, std::vector<char> I_pieces, std::vector<char> I_places)
        {
            board = I_board;
            pieces = I_pieces;
            places = I_places;
            id = getId();
            won = isWon();
            score = 0;
        }

        Board play(char piece, char i, char toGive)
        {
            std::array<char, 16> nBoard = board;
            std::vector<char> nPieces = pieces;
            std::vector<char> nPlaces = places;

            if(
                std::binary_search( nPieces.begin(), nPieces.end(), piece ) &&
                std::binary_search( nPlaces.begin(), nPlaces.end(), i )
            )
            { // If Found
                nBoard[i] = piece;
                nPieces.erase(std::remove(nPieces.begin(), nPieces.end(), piece), nPieces.end());
                nPlaces.erase(std::remove(nPlaces.begin(), nPlaces.end(), i), nPlaces.end());

            } else // If Not Found
            {
                std::cout << "Error: Piece or Place not found" << std::endl;
                std::exit(1);
            }

            Board newBoard = Board(nBoard, nPieces, nPlaces);
            newBoard.pieceToGive = toGive;
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
        bool isPattern(char a, char b, char c, char d)
        {
            if(a == -1 || b == -1 || c == -1 || d == -1)
                return false;

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
                    std::binary_search(patterns[p].begin(), patterns[p].end(), c) &&
                    std::binary_search(patterns[p].begin(), patterns[p].end(), d)
                )
                {
                    return true;
                } else if(
                    !std::binary_search(patterns[p].begin(), patterns[p].end(), a) &&
                    !std::binary_search(patterns[p].begin(), patterns[p].end(), b) &&
                    !std::binary_search(patterns[p].begin(), patterns[p].end(), c) &&
                    !std::binary_search(patterns[p].begin(), patterns[p].end(), d)
                )
                {
                    return true;
                }
            }

            return false;
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

        bool isWon()
        {
            std::array<std::array<int, 4>, 10> sequences = {{
                {0, 1, 2, 3},
                {4, 5, 6, 7},
                {8, 9, 10, 11},
                {12, 13, 14, 15},
                {0, 4, 8, 12},
                {1, 5, 9, 13},
                {2, 6, 10, 14},
                {3, 7, 11, 15},
                {0, 5, 10, 15},
                {3, 6, 9, 12}
            }};

            for(char s = 0; s < 10; ++s)
                if(isPattern(
                    board[sequences[s][0]],
                    board[sequences[s][1]],
                    board[sequences[s][2]],
                    board[sequences[s][3]]
                ))
                    return true;

            return false;

        }
};
