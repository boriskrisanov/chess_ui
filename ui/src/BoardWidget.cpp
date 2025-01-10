#include "BoardWidget.hpp"
#include <QSvgWidget>
#include <QPainter>
#include <QMouseEvent>
#include <search.hpp>

BoardWidget::BoardWidget(QWidget* parent)
    : QWidget(parent)
{
    board.loadFen("8/1k4P1/8/1K6/8/8/8/8 w - - 0 1");
    // board.loadFen(STARTING_POSITION_FEN);
    updateLegalMoves();
    drawPieces();
}

void BoardWidget::paintEvent(QPaintEvent* event)
{
    squareSize = std::min(size().width(), size().height()) / 8;
    QPainter painter(this);
    QColor squareColor = lightSquareColor;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            const int index = 8 * i + j;
            squareColor = squareColor == lightSquareColor ? darkSquareColor : lightSquareColor;
            QRect square(j * squareSize, i * squareSize, squareSize, squareSize);
            painter.fillRect(square, squareColor);
            const auto legalMovesForSquare = legalMoves[moveStartIndex];
            if (isPieceBeingMoved && std::find(legalMovesForSquare.begin(), legalMovesForSquare.end(), index) !=
                legalMovesForSquare.end())
            {
                painter.fillRect(square, legalMoveSquareColor);
            }
            if ((board.whiteKing & bitboards::withSquare(index)) != 0 && board.isSideInCheck(WHITE)
                || (board.blackKing & bitboards::withSquare(index)) != 0 && board.isSideInCheck(BLACK))
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
    if (board[index].isNone())
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
    const bool isPawn = board[moveStartIndex].kind == PieceKind::PAWN;
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

    movePieceWidgets(move);
    board.makeMove(move);
    updateLegalMoves();
    repaint();

    // TODO: Do this is in a separate thread so the app doesn't freeze
    SearchResult searchResult = timeLimitedSearch(board, static_cast<std::chrono::milliseconds>(1000));
    movePieceWidgets(searchResult.bestMove);
    board.makeMove(searchResult.bestMove);
    updateLegalMoves();
    repaint();
}

void BoardWidget::resizeEvent(QResizeEvent* event)
{
    // drawPieces();
    QWidget::resizeEvent(event);
}

int BoardWidget::coordinatesToBoardIndex(QPoint coordinates) const
{
    int file = std::floor(coordinates.x() / static_cast<double>(squareSize));
    int rank = std::floor(coordinates.y() / static_cast<double>(squareSize));

    return 8 * rank + file;
}

QPoint BoardWidget::boardIndexToCoordinates(int index) const
{
    const int file = index % 8;
    const int rank = std::floor(index / 8);

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

void BoardWidget::movePieceWidgets(const Move& move)
{
    // Remove piece if it was captured
    if (pieceWidgets[move.end()] != nullptr)
    {
        delete pieceWidgets[move.end()];
        // No need to set to nullptr because it will be overwritten anyway
    }
    if (move.moveFlag() == MoveFlag::EnPassant)
    {
        Square capturedPieceIndex = board.sideToMove == WHITE ? move.end() + 8 : move.end() - 8;
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
        addPieceWidget({promotedPiece, board.sideToMove}, move.end());
        delete pieceWidgets[move.start()];
        pieceWidgets[move.start()] = nullptr;
    }

    if (!move.isPromotion())
    {
        pieceWidgets[move.end()] = pieceWidgets[move.start()];
        pieceWidgets[move.end()]->move(boardIndexToCoordinates(move.end()));
        pieceWidgets[move.start()] = nullptr;
    }


    // Move rook if castling
    if (board.sideToMove == WHITE)
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

Move BoardWidget::getMoveFromIndexes(Square start, Square end, MoveFlag promotion) const
{
    MoveFlag moveFlag = promotion; // Will be NONE by default
    // En Passant
    if (board[start].kind == PieceKind::PAWN && end == board.getEnPassantTargetSquare())
    {
        moveFlag = MoveFlag::EnPassant;
    }
    // Castling
    else if (board[start].kind == PieceKind::KING)
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
    if (piece.color == WHITE)
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
