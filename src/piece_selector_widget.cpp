#include "piece_selector_widget.h"

// ---ButtonSelector---
ButtonSelector::ButtonSelector(QWidget *parent, PieceType _pieceType, PlayerColor playerColor, QRect geo):
	QPushButton(parent), pieceType(_pieceType)
{
	setParent(parent);
	setGeometry(geo);
	setColor(playerColor);
}

void ButtonSelector::setColor(PlayerColor color)
{
	setStyleSheet(QString("QPushButton{border-image: url(%1); background-color: #41c3cc;}").arg(Piece::getImage(pieceType, color)));
}

// ---PieceSelectorWidget---
PieceSelectorWidget::PieceSelectorWidget(QWidget *parent, int _pieceSideSize):
	QWidget(parent), pieceSideSize(_pieceSideSize)
{
	resize(pieceSideSize, pieceSideSize * 4);

	pieces.push_back(QPointer<ButtonSelector>(new ButtonSelector(this, PieceType::QUEEN, PlayerColor::WHITE,
																 QRect(0, 0, pieceSideSize, pieceSideSize))));
	pieces.push_back(QPointer<ButtonSelector>(new ButtonSelector(this, PieceType::ROOK, PlayerColor::WHITE,
																 QRect(0, pieceSideSize, pieceSideSize, pieceSideSize))));
	pieces.push_back(QPointer<ButtonSelector>(new ButtonSelector(this, PieceType::BISHOP, PlayerColor::WHITE,
																 QRect(0, pieceSideSize * 2, pieceSideSize, pieceSideSize))));
	pieces.push_back(QPointer<ButtonSelector>(new ButtonSelector(this, PieceType::KNIGHT, PlayerColor::WHITE,
																 QRect(0, pieceSideSize * 3, pieceSideSize, pieceSideSize))));

	for(auto &piece: pieces) {
		piece->connect(piece, &QPushButton::clicked, [this, piece]()
		{
			emit selected(piece->pieceType, relativePosition);
			relativePosition = QPoint();
			hide();
		});
	}
	hide();
}

void PieceSelectorWidget::show(QPoint position, PlayerColor color, QPoint relativePosition)
{
	if(position.y() + height() >= 800)
		move(position.x(), 800 - height());
	else
		move(position);
	for(auto &piece: pieces)
		piece->setColor(color);

	this->relativePosition = relativePosition;
	raise();
	QWidget::show();
}
