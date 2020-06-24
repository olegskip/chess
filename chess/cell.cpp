#include "cell.h"

#include <QPalette>

Cell::Cell(QWidget *parent, QRect geometry, QPoint _relativePosition, CellType _cellType):
	QLabel(parent), relativePosition(_relativePosition), cellType(_cellType)
{
	setGeometry(geometry);
	setAutoFillBackground(true);
	if(cellType == CellType::BLACK)
		setBackgroundColor(QColor(210, 100, 50));
	else if(cellType == CellType::WHITE)
		setBackgroundColor(QColor(240, 200, 140));
//	setText(QString::number(relativePosition.x()) + " " + QString::number(relativePosition.y()));
}

void Cell::setBackgroundColor(QColor color)
{
	QPalette pal = this->palette();
	pal.setColor(QPalette::Window, color);
	setPalette(pal);
}
