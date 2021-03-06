#include "piece.h"
#include "config.h"

Piece::Piece(QWidget *parent, QRect geometry, QPoint _spawnRelativePosition,
			 PlayerColor _pieceOwner, PieceType _pieceType):
	QPushButton(parent), spawnRelativePosition(_spawnRelativePosition), pieceOwner(_pieceOwner)
{
	mPieceType = _pieceType;
	setAcceptDrops(true);
	setGeometry(geometry);
	setStyleSheet(QString("QPushButton{border: none; background-color:transparent; border-image: url(%1);}").arg(getImage(pieceType(), pieceOwner)));
	relativePosition = spawnRelativePosition;
}

PieceType Piece::pieceType() const
{
	return mPieceType;
}

void Piece::relativeMove(QPoint relativePosition, int currentMove)
{
	if(currentMove != -1 && this->getRelativePosition() != relativePosition) {
		lastMove = currentMove;
		moveCount++;
	}
	this->relativePosition = relativePosition;

	move(QPoint(relativePosition.x() * size().width(), 7 * height() - relativePosition.y() * size().height()));
}

QPoint Piece::getRelativePosition() const
{
	return relativePosition;
}

bool Piece::isMoved() const
{
	return moveCount > 0;
}

int Piece::getLastMove() const
{
	return lastMove;
}

unsigned int Piece::getMoveCount() const
{
	return moveCount;
}

QString Piece::getImage(PieceType _pieceType, PlayerColor _color)
{
	if(_pieceType == PieceType::KING)
		return _color == PlayerColor::WHITE ? config::pieces_paths::WHITE_KING: config::pieces_paths::BLACK_KING;
	else if(_pieceType == PieceType::QUEEN)
		return _color == PlayerColor::WHITE ? config::pieces_paths::WHITE_QUEEN: config::pieces_paths::BLACK_QUEEN;
	else if(_pieceType == PieceType::ROOK)
		return _color == PlayerColor::WHITE ? config::pieces_paths::WHITE_ROOK: config::pieces_paths::BLACK_ROOK;
	else if(_pieceType == PieceType::BISHOP)
		return _color == PlayerColor::WHITE ? config::pieces_paths::WHITE_BISHOP: config::pieces_paths::BLACK_BISHOP;
	else if(_pieceType == PieceType::KNIGHT)
		return _color == PlayerColor::WHITE ? config::pieces_paths::WHITE_KNIGHT: config::pieces_paths::BLACK_KNIGHT;
	else if(_pieceType == PieceType::PAWN)
		return _color == PlayerColor::WHITE ? config::pieces_paths::WHITE_PAWN: config::pieces_paths::BLACK_PAWN;

	return QString();
}

void Piece::transform(PieceType newPieceType)
{
	mPieceType = newPieceType;
	setStyleSheet(QString("QPushButton{border: none; background-color:transparent; border-image: url(%1);}").arg(getImage(pieceType(), pieceOwner)));
}

void Piece::mousePressEvent(QMouseEvent *event) // override
{
	if(event->buttons() & Qt::LeftButton) {
		offset = QPoint(-size().width() / 2, -size().height() / 2);
//		QPushButton::move(mapToParent(QPoint(event->pos().x() + offset.x(), event->pos().y() + offset.y())));
		mouseMoveEvent(event);
	}

	QPushButton::mousePressEvent(event);
}

void Piece::mouseMoveEvent(QMouseEvent *event) // override
{
	if(event->buttons() & Qt::LeftButton) {
		raise();
		emit moveSignal();

		const QPoint curPosWithOffset = mapToParent(event->pos() + offset);

		if(curPosWithOffset.x() < offset.x())
			QPushButton::move(offset.x(), y());
		else if(curPosWithOffset.x() > parentWidget()->width() + offset.x())
			QPushButton::move(parentWidget()->width() + offset.x(), y());
		else
			QPushButton::move(curPosWithOffset.x(), y());

		if(curPosWithOffset.y() < offset.y())
			QPushButton::move(x(), offset.y());
		else if(curPosWithOffset.y() > parentWidget()->height() + offset.x())
			QPushButton::move(x(), parentWidget()->height() + offset.y());
		else
			QPushButton::move(x(), curPosWithOffset.y());
	}

	QPushButton::mouseMoveEvent(event);
}

void Piece::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() & Qt::LeftButton)
		emit mouseReleaseSignal();
	QPushButton::mouseReleaseEvent(event);
}

