#include "BoardWidget.hpp"
#include <QSvgWidget>
#include <QPainter>
#include <QMouseEvent>
#include <search.hpp>
#include <QPropertyAnimation>

#include "GameOptions.hpp"

using enum PieceColor;

BoardWidget::BoardWidget(EngineInstance* engineInstance, PieceColor playerSide,
                         std::string startingFen, QWidget* parent)
    : QWidget(parent), engineInstance(engineInstance), playerSide(playerSide)
{
    // board.loadFen("8/1k4P1/8/1K6/8/8/8/8 w - - 0 1");
    newGame(startingFen, playerSide);
}

void BoardWidget::paintEvent(QPaintEvent* event)
{
    squareSize = std::min(size().width(), size().height()) / 8;
    QPainter painter(this);
    QColor squareColor = flipBoard ? lightSquareColor : darkSquareColor;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int index = 8 * i + j;
            if (flipBoard)
            {
                index = 63 - index;
            }
            squareColor = squareColor == lightSquareColor ? darkSquareColor : lightSquareColor;
            QRect square(j * squareSize, i * squareSize, squareSize, squareSize);
            painter.fillRect(square, squareColor);
            const auto legalMovesForSquare = legalMoves[moveStartIndex];
            if (isPieceBeingMoved && std::find(legalMovesForSquare.begin(), legalMovesForSquare.end(), index) !=
                legalMovesForSquare.end())
            {
                painter.fillRect(square, squareColor == lightSquareColor ? lightLegalMoveSquareColor : darkLegalMoveSquareColor);
            }
            if ((board.bitboards[pieceIndexes::WHITE_KING] & bitboards::withSquare(index)) != 0 && board.
                isSideInCheck(WHITE)
                || (board.bitboards[pieceIndexes::BLACK_KING] & bitboards::withSquare(index)) != 0 && board.
                isSideInCheck(BLACK))
            {
                painter.fillRect(square, checkSquareColor);
            }
        }
        squareColor = squareColor == lightSquareColor ? darkSquareColor : lightSquareColor;
    }
}

void BoardWidget::mouseMoveEvent(QMouseEvent* event)
{
    // Called when mouse is moved while pressed
    if (!isPieceBeingMoved)
    {
        return;
    }
    const QPoint mousePosition = event->pos();
    QSvgWidget* pieceWidget = pieceWidgets[moveStartIndex];
    // Subtract half of square size so that the cursor is in the middle of the piece
    const QPoint position = mousePosition - QPoint{squareSize / 2, squareSize / 2};
    pieceWidget->move(position);
}

void BoardWidget::mousePressEvent(QMouseEvent* event)
{
    if (isPieceBeingMoved)
    {
        // Probably not needed
        return;
    }
    const int index = coordinatesToBoardIndex(event->pos());
    if (board[index].isNone() || board[index].color() != playerSide)
    {
        return;
    }
    isPieceBeingMoved = true;
    moveStartIndex = index;
    pieceBeingMoved = board[index];
    repaint();
}

void BoardWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (!isPieceBeingMoved)
    {
        return;
    }
    isPieceBeingMoved = false;
    const QPoint mousePosition = event->pos();
    const int newIndex = coordinatesToBoardIndex(mousePosition);

    const auto legalMovesForSquare = legalMoves[moveStartIndex];
    const bool isIllegalMove = std::ranges::find(legalMovesForSquare, newIndex) == legalMovesForSquare.end();
    if (isIllegalMove)
    {
        // Ignore move
        pieceWidgets[moveStartIndex]->move(boardIndexToCoordinates(moveStartIndex));
        isPieceBeingMoved = false;
        repaint();
        return;
    }

    MoveFlag promotionFlag = MoveFlag::None;
    const bool isPawn = board[moveStartIndex].kind() == PieceKind::PAWN;
    const bool isWhitePromotion = isPawn && square::rank(newIndex) == 8 && board.sideToMove == WHITE;
    const bool isBlackPromotion = isPawn && square::rank(newIndex) == 1 && board.sideToMove == BLACK;

    // Promotion
    if (isWhitePromotion || isBlackPromotion)
    {
        promotionFlag = static_cast<MoveFlag>(promotionSelector->exec());
        if (promotionFlag == MoveFlag::None)
        {
            // Dialog was closed, cancel move
            pieceWidgets[moveStartIndex]->move(boardIndexToCoordinates(moveStartIndex));
            isPieceBeingMoved = false;
            repaint();
            return;
        }
    }

    const Move move = getMoveFromIndexes(moveStartIndex, newIndex, promotionFlag);

    emit movePlayed(move, board);

    // Don't animate user moves
    movePieceWidgets(move, false);
    board.makeMove(move);
    updateLegalMoves();
    repaint();

    if (!undoneMoves.empty())
    {
        // Start new move sequence from current position
        while (!undoneMoves.empty())
        {
            undoneMoves.pop();
        }
        emit setRedoMoveEnabled(false);
    }

    engineInstance->startSearch(board);

    emit setUndoMoveEnabled(true);
}

void BoardWidget::resizeEvent(QResizeEvent* event)
{
    repaint();
    drawPieces();
    QWidget::resizeEvent(event);
}

int BoardWidget::coordinatesToBoardIndex(QPoint coordinates) const
{
    int file = std::floor(coordinates.x() / static_cast<double>(squareSize));
    int rank = std::floor(coordinates.y() / static_cast<double>(squareSize));

    if (flipBoard)
    {
        file = 7 - file;
        rank = 7 - rank;
    }

    return 8 * rank + file;
}

QPoint BoardWidget::boardIndexToCoordinates(int index) const
{
    int file = index % 8;
    int rank = std::floor(index / 8);
    if (flipBoard)
    {
        file = 7 - file;
        rank = 7 - rank;
    }

    return {squareSize * file, squareSize * rank};
}

void BoardWidget::updateLegalMoves()
{
    for (int i = 0; i < 64; i++)
    {
        legalMoves[i].clear();
    }
    for (Move move : board.getLegalMoves())
    {
        legalMoves[move.start()].push_back(move.end());
    }
}

void BoardWidget::movePieceWidgets(const Move& move, bool animate)
{
    // We can't use board.sideToMove because we might be redoing an undone move
    const PieceColor sideToMove = board[move.start()].color();
    // Remove piece if it was captured
    if (pieceWidgets[move.end()] != nullptr)
    {
        delete pieceWidgets[move.end()];
        // No need to set to nullptr because it will be overwritten anyway
    }
    if (animate)
    {
        animatePieceMovement(move.start(), move.start(), move.end());
    }
    if (move.moveFlag() == MoveFlag::EnPassant)
    {
        Square capturedPieceIndex = sideToMove == WHITE ? move.end() + 8 : move.end() - 8;
        delete pieceWidgets[capturedPieceIndex];
        pieceWidgets[capturedPieceIndex] = nullptr;
    }
    else if (move.isPromotion())
    {
        PieceKind promotedPiece = PieceKind::NONE;
        switch (move.moveFlag())
        {
        case MoveFlag::PromotionKnight:
            promotedPiece = PieceKind::KNIGHT;
            break;
        case MoveFlag::PromotionBishop:
            promotedPiece = PieceKind::BISHOP;
            break;
        case MoveFlag::PromotionRook:
            promotedPiece = PieceKind::ROOK;
            break;
        case MoveFlag::PromotionQueen:
            promotedPiece = PieceKind::QUEEN;
            break;
        default:
            break;
        }
        addPieceWidget({promotedPiece, sideToMove}, move.end());
        delete pieceWidgets[move.start()];
        pieceWidgets[move.start()] = nullptr;
    }

    if (!move.isPromotion())
    {
        pieceWidgets[move.end()] = pieceWidgets[move.start()];
        pieceWidgets[move.start()] = nullptr;
        if (!animate)
        {
            pieceWidgets[move.end()]->move(boardIndexToCoordinates(move.end()));
        }
    }

    // Move rook if castling
    if (sideToMove == WHITE)
    {
        const Square a1 = square::fromString("a1");
        const Square d1 = square::fromString("d1");
        const Square f1 = square::fromString("f1");
        const Square h1 = square::fromString("h1");
        if (move.moveFlag() == MoveFlag::ShortCastling)
        {
            pieceWidgets[f1] = pieceWidgets[h1];
            pieceWidgets[h1]->move(boardIndexToCoordinates(f1));
            pieceWidgets[h1] = nullptr;
        }
        else if (move.moveFlag() == MoveFlag::LongCastling)
        {
            pieceWidgets[d1] = pieceWidgets[a1];
            pieceWidgets[a1]->move(boardIndexToCoordinates(d1));
            pieceWidgets[a1] = nullptr;
        }
    }
    else
    {
        const Square a8 = square::fromString("a8");
        const Square d8 = square::fromString("d8");
        const Square f8 = square::fromString("f8");
        const Square h8 = square::fromString("h8");
        if (move.moveFlag() == MoveFlag::ShortCastling)
        {
            pieceWidgets[f8] = pieceWidgets[h8];
            pieceWidgets[h8]->move(boardIndexToCoordinates(f8));
            pieceWidgets[h8] = nullptr;
        }
        else if (move.moveFlag() == MoveFlag::LongCastling)
        {
            pieceWidgets[d8] = pieceWidgets[a8];
            pieceWidgets[a8]->move(boardIndexToCoordinates(d8));
            pieceWidgets[a8] = nullptr;
        }
    }
}

// Move must have been made for capturedPiece to be set
void BoardWidget::undoMovePieceWidgets(const Board boardAfterMoveUndone, const Move& move)
{
    pieceWidgets[move.start()] = pieceWidgets[move.end()];
    pieceWidgets[move.start()]->move(boardIndexToCoordinates(move.end())); // Will be moved by animation later
    pieceWidgets[move.end()] = nullptr;
    if (move.isPromotion())
    {
        // Add pawn on end square because movement back to original square will be animated later
        addPieceWidget(boardAfterMoveUndone[move.start()], move.end());
    }
    animatePieceMovement(move.start(), move.end(), move.start());

    // Restore captured piece
    if (move.capturedPiece.kind() != PieceKind::NONE)
    {
        Square capturedPiecePosition = move.end();
        if (move.moveFlag() == MoveFlag::EnPassant)
        {
            auto color = boardAfterMoveUndone[move.start()].color();
            if (color == WHITE)
            {
                capturedPiecePosition = move.end() + 8;
            }
            else
            {
                capturedPiecePosition = move.end() - 8;
            }
        }
        addPieceWidget(move.capturedPiece, capturedPiecePosition);
    }

    // Undo rook movement if castling
    if (board.sideToMove == WHITE)
    {
        const Square a1 = square::fromString("a1");
        const Square d1 = square::fromString("d1");
        const Square f1 = square::fromString("f1");
        const Square h1 = square::fromString("h1");
        if (move.moveFlag() == MoveFlag::ShortCastling)
        {
            animatePieceMovement(f1, f1, h1);
            pieceWidgets[h1] = pieceWidgets[f1];
            pieceWidgets[f1] = nullptr;
        }
        else if (move.moveFlag() == MoveFlag::LongCastling)
        {
            animatePieceMovement(d1, d1, a1);
            pieceWidgets[a1] = pieceWidgets[d1];
            pieceWidgets[d1] = nullptr;
        }
    }
    else
    {
        const Square a8 = square::fromString("a8");
        const Square d8 = square::fromString("d8");
        const Square f8 = square::fromString("f8");
        const Square h8 = square::fromString("h8");
        if (move.moveFlag() == MoveFlag::ShortCastling)
        {
            animatePieceMovement(f8, f8, h8);
            pieceWidgets[h8] = pieceWidgets[f8];
            pieceWidgets[f8] = nullptr;
        }
        else if (move.moveFlag() == MoveFlag::LongCastling)
        {
            animatePieceMovement(d8, d8, a8);
            pieceWidgets[a8] = pieceWidgets[d8];
            pieceWidgets[d8] = nullptr;
        }
    }
}

Move BoardWidget::getMoveFromIndexes(Square start, Square end, MoveFlag promotion) const
{
    MoveFlag moveFlag = promotion; // Will be NONE by default
    // En Passant
    if (board[start].kind() == PieceKind::PAWN && end == board.getEnPassantTargetSquare())
    {
        moveFlag = MoveFlag::EnPassant;
    }
    // Castling
    else if (board[start].kind() == PieceKind::KING)
    {
        if (board.sideToMove == WHITE)
        {
            const Square c1 = square::fromString("c1");
            const Square g1 = square::fromString("g1");
            if (board.canWhiteShortCastle() && end == g1)
            {
                moveFlag = MoveFlag::ShortCastling;
            }
            else if (board.canWhiteLongCastle() && end == c1)
            {
                moveFlag = MoveFlag::LongCastling;
            }
        }
        else
        {
            const Square c8 = square::fromString("c8");
            const Square g8 = square::fromString("g8");
            if (board.canBlackShortCastle() && end == g8)
            {
                moveFlag = MoveFlag::ShortCastling;
            }
            else if (board.canBlackLongCastle() && end == c8)
            {
                moveFlag = MoveFlag::LongCastling;
            }
        }
    }
    return Move{start, end, moveFlag};
}

void BoardWidget::addPieceWidget(Piece piece, Square position)
{
    QString fileName = QString::fromStdString(piece.toString()).toLower() + ".svg";
    if (piece.color() == WHITE)
    {
        fileName.prepend("_");
    }
    fileName.prepend("pieces/");
    auto* pieceWidget = new QSvgWidget(fileName, this);
    pieceWidgets[position] = pieceWidget;
    const auto [x, y] = boardIndexToCoordinates(position);
    pieceWidget->setGeometry(x, y, squareSize, squareSize);
    pieceWidget->show();
}

void BoardWidget::animatePieceMovement(Square pieceIndex, Square start, Square end) const
{
    auto* animation = new QPropertyAnimation(pieceWidgets[pieceIndex], "pos");
    animation->setDuration(120);
    animation->setStartValue(boardIndexToCoordinates(start));
    animation->setEndValue(boardIndexToCoordinates(end));
    animation->start();
}

void BoardWidget::onEngineSearchDone(SearchResult move)
{
    movePieceWidgets(move.bestMove, true);
    emit movePlayed(move.bestMove, board);
    board.makeMove(move.bestMove);
    updateLegalMoves();
    repaint();
    emit setUndoMoveEnabled(true);
}

void BoardWidget::onUndoMove()
{
    // Prevents crash if user clicks button before it is disabled
    if (board.getMoveHistory().empty())
    {
        return;
    }

    Move move = board.getMoveHistory().back();
    undoneMoves.push(move);
    std::cout << undoneMoves.size() << "\n";
    board.unmakeMove();
    undoMovePieceWidgets(board, move);
    updateLegalMoves();
    repaint();
    if (board.getMoveHistory().empty())
    {
        emit setUndoMoveEnabled(false);
    }
    emit setRedoMoveEnabled(true);
    emit moveUndone();
}

void BoardWidget::onRedoMove()
{
    // Prevents crash if user clicks button before it is disabled
    if (undoneMoves.empty())
    {
        return;
    }

    Move move = undoneMoves.top();
    undoneMoves.pop();
    board.makeMove(move);
    movePieceWidgets(move, true);
    updateLegalMoves();
    repaint();
    if (undoneMoves.empty())
    {
        emit setRedoMoveEnabled(false);
    }
    emit setUndoMoveEnabled(true);
    emit moveRedone();
}

void BoardWidget::flipBoardSlot()
{
    flipBoard = !flipBoard;
    repaint();
    drawPieces();
    updateLegalMoves();
    repaint();
}

void BoardWidget::drawPieces()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            const int index = 8 * i + j;
            if (pieceWidgets[index] != nullptr)
            {
                delete pieceWidgets[index];
                pieceWidgets[index] = nullptr;
            }
            if (board.isSquareEmpty(index))
            {
                continue;
            }
            addPieceWidget(board[index], index);
        }
    }
}
