#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "piece_selector_widget.h"
#include "config.h"
#include "piece.h"
#include "cell.h"

#include <QSharedPointer>
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
	bool isStalemate();
	bool castling(Piece &piece, const Cell &cell);

	void removePiece(const Piece &pieceToRemove);
	QSharedPointer<Piece> getPiece(QPoint relativePosition);
	QSharedPointer<Piece> getPiece(PlayerColor color, PieceType pieceType);
	QVector<QSharedPointer<Piece>> getPieces(PieceType pieceType);

	QSharedPointer<Cell> getCell(QPoint relativePosition);
	QSharedPointer<Piece> getCellAtacker(const Cell &cell, PlayerColor kingColor,
										 const QSharedPointer<Piece> &checkWithout = QSharedPointer<Piece>(), QPoint newPosition = QPoint(-1, -1));

	void restart();

	int currentTurnCount = 1;
	PlayerColor currentTurn = PlayerColor::WHITE;
	bool isAllowToMove = true;

	QMessageBox victoryLabel;
	void showVictoryLabel(PlayerColor looserColor);

	QPointer<QWidget> newCentralWidget;
	QPointer<QPushButton> restartButton;

	QPointer<PieceSelectorWidget> pieceSelectorWidget;
};
#endif // MAINWINDOW_H
