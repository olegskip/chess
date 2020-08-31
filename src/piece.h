#pragma once
#include <QMouseEvent>
#include <QPushButton>
#include <QVector2D>
#include <QString>


enum class PlayerColor
{
	NONE,
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

class Piece: public QPushButton
{
	Q_OBJECT

public:
	Piece(QWidget *parent, QRect geometry, QPoint _spawnRelativePosition, PlayerColor _pieceOwner, PieceType _pieceType);

	const QPoint spawnRelativePosition;
	const PlayerColor pieceOwner;
	PieceType pieceType() const;

	void relativeMove(QPoint relativePosition, int currentMove = -1);
	QPoint getRelativePosition() const;
	bool isMoved() const;
	int getLastMove() const;
	unsigned int getMoveCount() const;

	bool isHasCollision = true;

	static QString getImage(PieceType _pieceType, PlayerColor _color);
	void transform(PieceType newPieceType);

	bool isDraggingNow() const;

signals:
	void mouseReleaseSignal() const;
	void moveSignal() const;

private:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

	unsigned int moveCount = 0;
	int lastMove = -1;

	QPoint relativePosition;
	QPoint offset;
	PieceType mPieceType;

	bool mIsDraggingNow = false;
};

