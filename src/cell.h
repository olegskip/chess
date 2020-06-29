#pragma once
#include <QPushButton>
#include <QPoint>


enum class CellType
{
	WHITE,
	BLACK
};

enum class CellActiveType
{
	SELECT,
	LAST_TURN_CURRENT_CELL,
	LAST_TURN_PREVIOUS_CELL,
	KING_CHECKED
};

class Cell: public QPushButton
{
	Q_OBJECT

public:
	Cell(QWidget *parent, QRect geometry, QPoint _relativePosition, CellType _cellType);

	const QPoint relativePosition;
	const CellType cellType;
	void setBackgroundColor(QColor color);

	void activate(CellActiveType cellActiveType);
	void deactivate();

	bool operator ==(const Cell &secondCell);

private:
	const QColor color;
};
