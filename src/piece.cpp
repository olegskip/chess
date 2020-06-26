#include "piece.h"

Piece::Piece(QWidget *parent, QRect geometry, QPoint _spawnRelativePosition, QString backgroundImage, PlayerColor _pieceOwner, PieceType _pieceType):
	QPushButton(parent), spawnRelativePosition(_spawnRelativePosition), pieceOwner(_pieceOwner), pieceType(_pieceType)
{
	setAcceptDrops(true);
	setGeometry(geometry);
	setStyleSheet(QString("QPushButton{border: none; background-color:transparent; border-image: url(%1)};").arg(backgroundImage));
	relativePosition = spawnRelativePosition;
}

void Piece::move(QPoint relativePosition)
{
	if(this->getRelativePosition() != relativePosition)
		moveCount++;
	this->relativePosition = relativePosition;

	QPushButton::move(QPoint(relativePosition.x() * size().width(), 700 - relativePosition.y() * size().height()));
}

QPoint Piece::getRelativePosition() const
{
	return relativePosition;
}

bool Piece::isMoved() const
{
	return moveCount > 0;
}

void Piece::mousePressEvent(QMouseEvent* event) // override
{
	if(event->buttons() & Qt::LeftButton) {
		offset = QPoint(-size().width() / 2, -size().height() / 2);
		QPushButton::move(mapToParent(event->pos() + offset));
	}
	QPushButton::mousePressEvent(event);
}

void Piece::mouseMoveEvent(QMouseEvent* event) // override
{
	if(event->buttons() & Qt::LeftButton) {
		raise();
		emit moveSignal();
		QPushButton::move(mapToParent(event->pos() + offset));
	}
	QPushButton::mouseMoveEvent(event);
}

void Piece::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() & Qt::LeftButton)
		emit mouseReleaseSignal();
	QPushButton::mouseReleaseEvent(event);
}

