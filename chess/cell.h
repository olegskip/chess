#pragma once
#include <QLabel>
#include <QPoint>


enum class CellType
{
	WHITE,
	BLACK
};

class Cell: public QLabel
{
	Q_OBJECT

public:
	Cell(QWidget *parent, QRect geometry, QPoint _relativePosition, CellType _cellType);
	using QLabel::pos;
	using QLabel::geometry;

	const QPoint relativePosition;
	const CellType cellType;
	void setBackgroundColor(QColor color);
};
