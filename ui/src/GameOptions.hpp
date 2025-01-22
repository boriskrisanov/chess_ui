#pragma once
#include <chrono>

#include "Piece.hpp"

struct GameOptions
{
    int transpositionTableSizeMB;
    std::chrono::milliseconds searchTime;
    PieceColor playerSide;
    std::string startingFen;

    GameOptions(int transpositionTableSizeMB, const std::chrono::milliseconds& searchTime, PieceColor playerSide,
                const std::string& startingFen = STARTING_POSITION_FEN)
        : transpositionTableSizeMB(transpositionTableSizeMB),
          searchTime(searchTime),
          playerSide(playerSide),
          startingFen(startingFen)
    {
    }

};
