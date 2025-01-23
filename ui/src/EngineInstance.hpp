#pragma once

#include <QObject>
#include <thread>

#include "Board.hpp"
#include "search.hpp"

class EngineInstance : public QObject
{
    Q_OBJECT

public:
    void startSearch(const Board& board)
    {
        // Make copy of board so that the search doesn't modify the original while the widget is rendering
        this->board = board;
        std::thread thread{[this]
            {
                emit searchDone(timeLimitedSearch(this->board, searchTime));
            }
        };
        thread.detach();
    }

    void setSearchTime(std::chrono::milliseconds searchTime)
    {
        this->searchTime = searchTime;
    }

private:
    std::chrono::milliseconds searchTime{1000};
    Board board;

signals:
    void searchDone(SearchResult searchResult);
};
