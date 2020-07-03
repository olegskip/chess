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
#include <QDebug>
class Piece: public QPushButton
{
	Q_OBJECT

public:
	Piece(QWidget *parent, QRect geometry, QPoint _spawnRelativePosition, QString backgroundImage, PlayerColor _pieceOwner, PieceType _pieceType);
//	~Piece() {qDebug() << "destructor";}
	const QPoint spawnRelativePosition;
	const PlayerColor pieceOwner;
	const PieceType pieceType;

	void move(QPoint relativePosition);
	QPoint getRelativePosition() const;
	bool isMoved() const;
	unsigned int getMoveCount() const;

	bool isHasCollision = true;

signals:
	void mouseReleaseSignal() const;
	void moveSignal() const;

private:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

	unsigned int moveCount = 0;
	QPoint relativePosition;
	QPoint offset;
};

