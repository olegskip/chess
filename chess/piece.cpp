#include "piece.h"

Piece::Piece(QWidget *parent, QRect geometry, QPoint _spawnRelativePosition, QString backgroundImage, PlayerColor _pieceOwner, PieceType _pieceType):
	QLabel(parent), spawnRelativePosition(_spawnRelativePosition), pieceOwner(_pieceOwner), pieceType(_pieceType)
{
	setAcceptDrops(true);
	setGeometry(geometry);
	setStyleSheet(QString("QLabel{border-image: url(") + backgroundImage + ");}");
	relativePosition = spawnRelativePosition;
}

void Piece::move(QPoint relativePosition)
{
	if(this->getRelativePosition() != relativePosition)
		mIsFirstMove = false;
	this->relativePosition = relativePosition;

	QLabel::move(QPoint(relativePosition.x() * size().width(), 700 - relativePosition.y() * size().height()));
}

QPoint Piece::getRelativePosition() const
{
	return relativePosition;
}

bool Piece::isFirstMove() const
{
	return mIsFirstMove;
}

void Piece::mousePressEvent(QMouseEvent* event) // override
{
	if(event->buttons() & Qt::LeftButton) {
		offset = QPoint(-size().width() / 2, -size().height() / 2);
		QLabel::move(mapToParent(event->pos() + offset));
	}
}

void Piece::mouseMoveEvent(QMouseEvent* event) // override
{
	if(event->buttons() & Qt::LeftButton) {
		raise();
		QLabel::move(mapToParent(event->pos() + offset));
	}
}

void Piece::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() & Qt::LeftButton)
		emit mouseReleaseSignal();
}

