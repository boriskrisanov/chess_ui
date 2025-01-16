#pragma once

#include <QObject>
#include <thread>

#include "Board.hpp"
#include "search.hpp"

class EngineInstance : public QObject
{
    Q_OBJECT

public:
    void startSearch(Board& board, std::chrono::milliseconds timeLimit)
    {
        std::thread thread{[this, &board, timeLimit]
            {
                emit searchDone(timeLimitedSearch(board, timeLimit));
            }
        };
        thread.detach();
    }

signals:
    void searchDone(SearchResult searchResult);
};
