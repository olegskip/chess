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
	QVector<QPointer<Piece>> pieces;

	QSharedPointer<Cell> &findNearestCell(QPoint pos);
	bool isWayClear(QPoint startPoint, QPoint endPoint);
	QVector<int> getFilledVector(int from, int to);
	void highlightLastTurn(QSharedPointer<Cell> oldCell, QSharedPointer<Cell> newCell);

	bool attemptToMove(Piece &piece, const Cell &cell);
	bool isMovePossible(const Piece &piece, const Cell &cell);
	void removePiece(const Piece &pieceToRemove);
	bool isCellEmpty(const Cell &cell) const;
	QPointer<Piece> getPiece(QPoint relativePosition);
	QSharedPointer<Cell> getCell(QPoint relativePosition);

	PlayerColor currentTurn = PlayerColor::WHITE;


	Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
