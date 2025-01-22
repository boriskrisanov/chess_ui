#pragma once

#include <QObject>
#include <thread>

#include "Board.hpp"
#include "search.hpp"

class EngineInstance : public QObject
{
    Q_OBJECT

public:
    void startSearch(Board& board)
    {
        std::thread thread{[this, &board]
            {
                emit searchDone(timeLimitedSearch(board, searchTime));
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

signals:
    void searchDone(SearchResult searchResult);
};
