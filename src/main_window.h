#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "piece_selector_widget.h"
#include "config.h"
#include "piece.h"
#include "cell.h"

#include <QSharedPointer>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <functional>
#include <algorithm>
#include <QPointer>
#include <numeric>
#include <QVector>
#include <QTimer>
#include <QtMath>
#include <QPair>
#include <array>


class SimplePiece: public QObject
{
public:
	SimplePiece(QPoint _relativePosition, PieceType _pieceType, PlayerColor _pieceColor):
		relativePosition(_relativePosition), pieceType(_pieceType), pieceColor(_pieceColor)
	{
	}

	const QPoint relativePosition;
	const PieceType pieceType;
	const PlayerColor pieceColor;

	bool operator ==(const SimplePiece &second)
	{
		return relativePosition == second.relativePosition && pieceType == second.pieceType && pieceColor == second.pieceColor;
	}
};

class ChessboardPosition: private QVector<QPointer<SimplePiece>>
{
public:
	using QVector<QPointer<SimplePiece>>::push_back;
	bool operator ==(const ChessboardPosition &second)
	{
		for(auto &firstItem: *this) {
			bool isEqual = false;
			for(auto &secondItem: second) {
				if(*firstItem == *secondItem) {
					isEqual = true;
					break;
				}
			}
			if(!isEqual)
				return false;
		}
		return true;
	}
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void onPieceMoved(Piece &piece);
	void onPieceClicked(Piece &piece);
	void onCellClicked(Cell &cell);

private:
	QVector<QVector<QSharedPointer<Cell>>> cells; // [x][y]
	QSharedPointer<Cell> activeCell;
	QPair<QSharedPointer<Cell>, QSharedPointer<Cell>> lastTurnCells;
	QSharedPointer<Cell> checkedCell;
	QVector<QSharedPointer<Piece>> pieces;

	QPointer<QVBoxLayout> mainVerticalLayout;
	QPointer<QHBoxLayout> piecesHorizontalLayouts;
	QVector<QPointer<QVBoxLayout>> piecesVerticalLayouts;

	bool addPiece(QPoint relativePosition, PlayerColor color, PieceType pieceType);

	QSharedPointer<Cell> &findNearestCell(QPoint pos);
	QVector<QPoint> getPointsOnWay(QPoint startPoint, QPoint endPoint);
	bool isWayClear(QVector<QPoint> pointsOnWay);
	QVector<int> getFilledVector(int from, int to);
	void highlightLastTurn(QSharedPointer<Cell> oldCell, QSharedPointer<Cell> newCell);

	bool attemptToMove(Piece &piece, const Cell &cell);
	bool isMovePossible(Piece &piece, const Cell &cell);
	bool isCanCapture(Piece &pieceAtacker, const Cell &cell);
	bool isCellEmpty(const Cell &cell) const;
	QSharedPointer<Piece> enPassant(Piece &pieceAtacker, const Cell &cell);
	PlayerColor isCheckmate();
	bool castling(Piece &piece, const Cell &cell);

	// ---draw---
	bool isDraw();
	bool isStalemate();
	bool isSufficientMaterial();
	bool threefoldRepetition();
	QVector<ChessboardPosition> chessboardPositions;

	// This function is used for save position for preventing threefold repetition
	void appendChessboardPosition();
	// ---draw---

	void removePiece(const Piece &pieceToRemove);
	QSharedPointer<Piece> getPiece(QPoint relativePosition);
	QSharedPointer<Piece> getPiece(PlayerColor color, PieceType pieceType);
	QVector<QSharedPointer<Piece>> getPieces(PieceType pieceType);
	QVector<QSharedPointer<Piece>> getPieces(PieceType pieceType, PlayerColor pieceColor);

	QSharedPointer<Cell> getCell(QPoint relativePosition);
	QSharedPointer<Piece> getCellAtacker(const Cell &cell, PlayerColor kingColor,
										 const QSharedPointer<Piece> &checkWithout = QSharedPointer<Piece>(), QPoint newPosition = QPoint(-1, -1));

	void restart();

	void resizeEvent(QResizeEvent *event) override;
	void fixPiecesPos();

	int movesCount = 0;
	PlayerColor currentTurn = PlayerColor::WHITE;
	bool isAllowedToMove = true;

	QMessageBox victoryLabel;
	void showVictoryLabel(PlayerColor looserColor);

	QPointer<PieceSelectorWidget> pieceSelectorWidget;

	QPointer<QWidget> mCentralWidget;
	QPointer<QPushButton> restartButton;

};
#endif // MAINWINDOW_H
