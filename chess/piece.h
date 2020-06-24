#pragma once
#include <QLabel>
#include <QMouseEvent>
#include <QVector2D>
#include <QString>


enum class PlayerColor
{
	WHITE,
	BLACK
};

enum class PieceType
{
	KING,
	QUEEN,
	ROOK,
	BISHOP,
	KNIGHT,
	PAWN
};

class Piece: public QLabel
{
	Q_OBJECT

public:
	Piece(QWidget *parent, QRect geometry, QPoint _spawnRelativePosition, QString backgroundImage, PlayerColor _pieceOwner, PieceType _pieceType);

	const QPoint spawnRelativePosition;
	const PlayerColor pieceOwner;
	const PieceType pieceType;

	void move(QPoint relativePosition);
	QPoint getRelativePosition() const;
	bool isFirstMove() const;

signals:
	void mouseReleaseSignal();

private:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

	bool mIsFirstMove = true;
	QPoint relativePosition;
	QPoint offset;
//	bool event(QEvent *myEvent);
};

