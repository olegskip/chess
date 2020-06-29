#include "cell.h"

#include <QPalette>

Cell::Cell(QWidget *parent, QRect geometry, QPoint _relativePosition, CellType _cellType):
	QPushButton(parent), relativePosition(_relativePosition), cellType(_cellType),
	color(cellType == CellType::BLACK ? QColor(210, 100, 50) : QColor(240, 200, 140))
{
	setGeometry(geometry);
	setAutoFillBackground(true);
	setBackgroundColor(color);
}

void Cell::activate(CellActiveType cellActiveType)
{
	if(cellActiveType == CellActiveType::SELECT)
		setBackgroundColor(QColor(250, 200, 100, 150));
	else if(cellActiveType == CellActiveType::LAST_TURN_CURRENT_CELL)
		setBackgroundColor(QColor(120, 200, 80, 180));
	else if(cellActiveType == CellActiveType::LAST_TURN_PREVIOUS_CELL)
		setBackgroundColor(QColor(130, 250, 100, 180));
	else if(cellActiveType == CellActiveType::KING_CHECKED)
		setBackgroundColor(QColor(210, 20, 50, 180));
}

void Cell::deactivate()
{
	setBackgroundColor(color);
}

bool Cell::operator ==(const Cell &secondCell)
{
	return this->relativePosition == secondCell.relativePosition;
}

void Cell::setBackgroundColor(QColor color)
{
	setStyleSheet(QString("QPushButton{border: none; background-color: %1;}").arg(color.name()));
}
