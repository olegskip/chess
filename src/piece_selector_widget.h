#pragma once
#include "config.h"
#include "piece.h"

#include <QPointer>
#include <QVector>
#include <QString>


class ButtonSelector: public QPushButton
{
	Q_OBJECT

public:
	ButtonSelector(QWidget *parent, PieceType _pieceType, PlayerColor playerColor, QRect geo);

	void setColor(PlayerColor color);
	const PieceType pieceType;

private:
	const QString mBackgroundImage;
};


class PieceSelectorWidget: public QWidget
{
	Q_OBJECT

public:
	PieceSelectorWidget(QWidget *parent, int _pieceSideSize);

	void show(QPoint position, PlayerColor color, QPoint relativePosition);

signals:
	void selected(PieceType pieceType, QPoint piecerelativePosition);

private:
	QPoint relativePosition;
	const int pieceSideSize;
	QVector<QPointer<ButtonSelector>> pieces;
};
