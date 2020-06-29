#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include "cell.h"
#include "piece.h"

#include <QMainWindow>
#include <QVector>
#include <QPointer>
#include <QSharedPointer>
#include <QPair>
#include <QtMath>
#include <numeric>
#include <algorithm>
#include <functional>
#include <QPair>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


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

	QSharedPointer<Cell> &findNearestCell(QPoint pos);
	bool isWayClear(QPoint startPoint, QPoint endPoint);
	QVector<int> getFilledVector(int from, int to);
	void highlightLastTurn(QSharedPointer<Cell> oldCell, QSharedPointer<Cell> newCell);

	bool attemptToMove(Piece &piece, const Cell &cell);
	bool isMovePossible(const Piece &piece, const Cell &cell);
	bool isCanCapture(const Piece &pieceAtacker, const Cell &cell);
	bool isCellEmpty(const Cell &cell) const;

	void removePiece(const Piece &pieceToRemove);
	QSharedPointer<Piece> getPiece(QPoint relativePosition);
	QVector<QSharedPointer<Piece>> getPieces(PieceType pieceType);

	QSharedPointer<Cell> getCell(QPoint relativePosition);
	bool isCellChecked(const Cell &cell, PlayerColor kingColor, const QSharedPointer<Piece> &checkWithout = QSharedPointer<Piece>());

	unsigned int currentTurnCount = 1;
	PlayerColor currentTurn = PlayerColor::WHITE;


	Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
