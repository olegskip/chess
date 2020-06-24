#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include "cell.h"
#include "piece.h"

#include <QMainWindow>
#include <QVector>
#include <QPointer>
#include <QPair>
#include <QtMath>
#include <numeric>
#include <algorithm>


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

private:
	QVector<QVector<QPointer<Cell>>> cells; // [x][y]
	QVector<QPointer<Piece>> pieces;

	Cell &findNearestCell(QPoint pos);
	bool isWayClear(QPoint startPoint, QPoint endPoint);
	QVector<int> getFilledVector(int from, int to);

	bool isMovePossible(const Piece &piece, const Cell &cell);
	void removePiece(const Piece &pieceToRemove);
	bool isCellEmpty(const Cell &cell) const;
	PlayerColor currentTurn = PlayerColor::WHITE;


	Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
