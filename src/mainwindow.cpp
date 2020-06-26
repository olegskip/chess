#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	setGeometry(x(), y(), 800, 800);

	// the chessboard is 8x8
	for(int x = 0; x <= 7; ++x) {
		cells.push_back(QVector<QSharedPointer<Cell>>());
		for(int y = 0; y <= 7; ++y) {
			CellType cellType;
			if((x % 2 == 0 && y % 2 == 0) || (x % 2 != 0 && y % 2 != 0))
				cellType = CellType::WHITE;
			else
				cellType = CellType::BLACK;

//			cells[x].push_back(new Cell);
			cells[x].push_back(QSharedPointer<Cell>::create(this, QRect(x * config::CELL_SIDE_SIZE, 7 * config::CELL_SIDE_SIZE - y * config::CELL_SIDE_SIZE,
																													config::CELL_SIDE_SIZE, config::CELL_SIDE_SIZE), QPoint(x, y), cellType));
		}
	}

	// fill the chessboard
	pieces.push_back(new Piece(this, cells[4][0]->geometry(), QPoint(4, 0), config::pieces_paths::WHITE_KING, PlayerColor::WHITE, PieceType::KING));
	pieces.push_back(new Piece(this, cells[4][7]->geometry(), QPoint(4, 7), config::pieces_paths::BLACK_KING, PlayerColor::BLACK, PieceType::KING));

	pieces.push_back(new Piece(this, cells[3][0]->geometry(), QPoint(3, 0), config::pieces_paths::WHITE_QUEEN, PlayerColor::WHITE, PieceType::QUEEN));
	pieces.push_back(new Piece(this, cells[3][7]->geometry(), QPoint(3, 7), config::pieces_paths::BLACK_QUEEN, PlayerColor::BLACK, PieceType::QUEEN));

	pieces.push_back(new Piece(this, cells[0][0]->geometry(), QPoint(0, 0), config::pieces_paths::WHITE_ROOK, PlayerColor::WHITE, PieceType::ROOK));
	pieces.push_back(new Piece(this, cells[7][0]->geometry(), QPoint(7, 0), config::pieces_paths::WHITE_ROOK, PlayerColor::WHITE, PieceType::ROOK));
	pieces.push_back(new Piece(this, cells[0][7]->geometry(), QPoint(0, 7), config::pieces_paths::BLACK_ROOK, PlayerColor::BLACK, PieceType::ROOK));
	pieces.push_back(new Piece(this, cells[7][7]->geometry(), QPoint(7, 7), config::pieces_paths::BLACK_ROOK, PlayerColor::BLACK, PieceType::ROOK));

	pieces.push_back(new Piece(this, cells[2][0]->geometry(), QPoint(2, 0), config::pieces_paths::WHITE_BISHOP, PlayerColor::WHITE, PieceType::BISHOP));
	pieces.push_back(new Piece(this, cells[5][0]->geometry(), QPoint(5, 0), config::pieces_paths::WHITE_BISHOP, PlayerColor::WHITE, PieceType::BISHOP));
	pieces.push_back(new Piece(this, cells[2][7]->geometry(), QPoint(2, 7), config::pieces_paths::BLACK_BISHOP, PlayerColor::BLACK, PieceType::BISHOP));
	pieces.push_back(new Piece(this, cells[5][7]->geometry(), QPoint(5, 7), config::pieces_paths::BLACK_BISHOP, PlayerColor::BLACK, PieceType::BISHOP));

	pieces.push_back(new Piece(this, cells[1][0]->geometry(), QPoint(1, 0), config::pieces_paths::WHITE_KNIGHT, PlayerColor::WHITE, PieceType::KNIGHT));
	pieces.push_back(new Piece(this, cells[6][0]->geometry(), QPoint(6, 0), config::pieces_paths::WHITE_KNIGHT, PlayerColor::WHITE, PieceType::KNIGHT));
	pieces.push_back(new Piece(this, cells[1][7]->geometry(), QPoint(1, 7), config::pieces_paths::BLACK_KNIGHT, PlayerColor::BLACK, PieceType::KNIGHT));
	pieces.push_back(new Piece(this, cells[6][7]->geometry(), QPoint(6, 7), config::pieces_paths::BLACK_KNIGHT, PlayerColor::BLACK, PieceType::KNIGHT));

	for(int x = 0; x <= 7; ++x) {
		pieces.push_back(new Piece(this, cells[x][1]->geometry(), QPoint(x, 1), config::pieces_paths::WHITE_PAWN, PlayerColor::WHITE, PieceType::PAWN));
		pieces.push_back(new Piece(this, cells[x][6]->geometry(), QPoint(x, 6), config::pieces_paths::BLACK_PAWN, PlayerColor::BLACK, PieceType::PAWN));
	}


	// connects
	for(auto &vec: cells) {
		for(QSharedPointer<Cell> cell: vec) {
			connect(cell.data(), &Cell::clicked, this, [this, cell]()
			{
				onCellClicked(*cell);
			});
		}
	}

	for(auto &piece: pieces) {
		connect(piece.data(), &Piece::mouseReleaseSignal, this, [this, piece]()
		{
			onPieceMoved(*piece);
		});
		connect(piece.data(), &Piece::pressed, this, [this, piece]()
		{
			onPieceClicked(*piece);
		});
	}
}

void MainWindow::onPieceMoved(Piece &piece)
{
	QSharedPointer<Cell> lastCell = cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()];
	QSharedPointer<Cell> nearestCell = findNearestCell(piece.pos());
	if(piece.getRelativePosition() == nearestCell->relativePosition || !attemptToMove(piece, *nearestCell)) {
		// if the move isn't possible move back the piece
		piece.move(piece.getRelativePosition());
	}
	else {
		if(activeCell) {
			activeCell.reset();
		}
	}
}

void MainWindow::onPieceClicked(Piece &piece)
{
	if(activeCell) {
		if(piece.pieceOwner != currentTurn && !isCellEmpty(*cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()])) {
			auto pieceAtacker = getPiece(activeCell->relativePosition);
			QSharedPointer<Cell> lastCell = cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()];
			if(pieceAtacker)
				attemptToMove(*pieceAtacker, *cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()]);
		}
		activeCell->deactive();
		activeCell.reset();
	}

	if(piece.pieceOwner == currentTurn && !isCellEmpty(*cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()])) {
		activeCell = cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()];
		activeCell->active(CellActiveType::SELECT);
	}
}

void MainWindow::onCellClicked(Cell &cell)
{
	QPointer<Piece> pieceOnCell;

	if(activeCell) {
		pieceOnCell = getPiece(activeCell->relativePosition);
		activeCell.reset();
	}
	if(pieceOnCell) {
		attemptToMove(*pieceOnCell, cell);
	}
}

bool MainWindow::isWayClear(QPoint startPoint, QPoint endPoint)
{
	QVector<QPoint> pointsOnWay;
	if(startPoint.x() == endPoint.x()) {
		for(auto item: getFilledVector( startPoint.y(), endPoint.y()))
			pointsOnWay.push_back(QPoint(startPoint.x(), item));
	}
	// if startPoint and endPoint in a row
	else if(startPoint.y() == endPoint.y()) {
		for(auto item: getFilledVector(startPoint.x(), endPoint.x()))
			pointsOnWay.push_back(QPoint(item, startPoint.y()));;
	}
	else if(std::abs(endPoint.x() - startPoint.x()) == std::abs(endPoint.y() - startPoint.y())){
		QVector<int> vecX = getFilledVector(startPoint.x(), endPoint.x());
		QVector<int> vecY = getFilledVector( startPoint.y(), endPoint.y());
		for(int i = 0; i < int(vecX.size()); ++i)
			pointsOnWay.push_back(QPoint(vecX[i], vecY[i]));
	}

	for(QPoint item: pointsOnWay) {
		if(!isCellEmpty(*cells[item.x()][item.y()]))
			return false;
	}

	return true;
}

QVector<int> MainWindow::getFilledVector(int from, int to)
{
	QVector<int> vec;

	if(from > to) {
		for(int i = from - 1; i > to; --i)
			vec.push_back(i);
	}
	else {
		for(int i = from + 1; i < to; ++i)
			vec.push_back(i);
	}

	return vec;
}

void MainWindow::highlightLastTurn(QSharedPointer<Cell> oldCell, QSharedPointer<Cell> newCell)
{
	if(lastTurnCells.first && lastTurnCells.second) {
		lastTurnCells.first->deactive();
		lastTurnCells.second->deactive();
	}

	if(oldCell && newCell) {
		lastTurnCells = {oldCell, newCell};
		newCell->active(CellActiveType::LAST_TURN_CURRENT_CELL);
		oldCell->active(CellActiveType::LAST_TURN_PREVIOUS_CELL);
	}
}

QSharedPointer<Cell> &MainWindow::findNearestCell(QPoint pos)
{
	QPair<QPoint, int> nearestCell = {QPoint(-1, -1), /* distance */ std::numeric_limits<int>::max()};
	for(auto &vec: cells) {
		for(auto &cell: vec) {
			int distance = std::abs(qPow((cell->pos().x() - pos.x()), 2) + qPow((cell->pos().y() - pos.y()), 2));
			if(distance < nearestCell.second)
				nearestCell = {QPoint(cells.indexOf(vec), vec.indexOf(cell)), distance};
		}
	}
	return cells[nearestCell.first.x()][nearestCell.first.y()];
}

bool MainWindow::attemptToMove(Piece &piece, const Cell &cell)
{
	if(!isMovePossible(piece, cell))
		return false;

	QSharedPointer<Cell> cellPtr = getCell(QPoint(cell.relativePosition.x(), cell.relativePosition.y()));
	if(cellPtr)
		highlightLastTurn(cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()], cellPtr);

	// if the move is possible move the piece
	piece.move(cell.relativePosition);

	if(currentTurn == PlayerColor::WHITE)
		currentTurn = PlayerColor::BLACK;
	else if(currentTurn == PlayerColor::BLACK)
		currentTurn = PlayerColor::WHITE;
	return true;
}

bool MainWindow::isMovePossible(const Piece &piece, const Cell &cell)
{
	if(piece.pieceOwner != currentTurn)
		return false;

	const QPoint piece1RelativePos = piece.getRelativePosition();
	const QPoint cellRelativePos = cell.relativePosition;

	auto checkLinearMove = [piece1RelativePos, cellRelativePos]()
	{
		return piece1RelativePos.x() == cellRelativePos.x() || piece1RelativePos.y() == cellRelativePos.y();
	};
	auto checkDiagonalMove = [piece1RelativePos, cellRelativePos]()
	{
		return std::abs(piece1RelativePos.x() - cellRelativePos.x()) == std::abs(piece1RelativePos.y() - cellRelativePos.y());
	};
	auto checkNearMove = [piece1RelativePos, cellRelativePos]()
	{
		return std::abs(piece1RelativePos.x() - cellRelativePos.x()) <= 1 && std::abs(piece1RelativePos.y() - cellRelativePos.y()) <= 1;
	};

	bool isMoved = false;

	if(piece.pieceType == PieceType::KING && checkNearMove())
			isMoved = true;

	else if(piece.pieceType == PieceType::QUEEN && (checkLinearMove() || checkNearMove() || checkDiagonalMove()))
			isMoved = true;

	else if(piece.pieceType == PieceType::ROOK && checkLinearMove())
			isMoved = true;

	else if(piece.pieceType == PieceType::BISHOP && checkDiagonalMove())
			isMoved = true;

	else if(piece.pieceType == PieceType::KNIGHT) {
			if((std::abs(piece1RelativePos.x() - cellRelativePos.x()) == 1 && std::abs(piece1RelativePos.y() - cellRelativePos.y()) == 2) ||
					(std::abs(piece1RelativePos.x() - cellRelativePos.x()) == 2 && std::abs(piece1RelativePos.y() - cellRelativePos.y()) == 1))
				isMoved = true;
	}

	else if(piece.pieceType == PieceType::PAWN) {
		// the white pawn can go only up, the black only down
		if(piece.getRelativePosition().x() == cellRelativePos.x() &&
				((piece.pieceOwner == PlayerColor::WHITE && ((!piece.isMoved() && piece1RelativePos.y() + 2 == cellRelativePos.y()) ||
															 (piece1RelativePos.y() + 1 == cellRelativePos.y()))) ||
				(piece.pieceOwner == PlayerColor::BLACK && ((!piece.isMoved() && piece1RelativePos.y() - 2 == cellRelativePos.y()) ||
															(piece1RelativePos.y() - 1 == cellRelativePos.y())))))
			isMoved = true;
	}

	if(!isWayClear(piece.getRelativePosition(), cellRelativePos))
		return false;

	if((isMoved || piece.pieceType == PieceType::PAWN) && !isCellEmpty(cell)) {

		const Piece &piece2 = **std::find_if(pieces.begin(), pieces.end(), [cellRelativePos]
												(const Piece *_piece){return cellRelativePos == _piece->getRelativePosition();});
		if(piece.pieceOwner == piece2.pieceOwner) {
			return false;
		}
		const QPoint piece2RelativePos = piece2.getRelativePosition();

		// only the pawn doesn't capture like move
		if(piece.pieceType != PieceType::PAWN ||
				((piece1RelativePos.x() - 1 == piece2RelativePos.x() || piece1RelativePos.x() + 1 == piece2RelativePos.x()) &&
				((piece.pieceOwner == PlayerColor::WHITE && piece1RelativePos.y() + 1 == piece2RelativePos.y()) ||
				(piece.pieceOwner == PlayerColor::BLACK && piece1RelativePos.y() - 1 == piece2RelativePos.y())))) {
			removePiece(piece2);
			isMoved = true;
		}
		else
			isMoved = false;
	}

	return isMoved;
}

void MainWindow::removePiece(const Piece &pieceToRemove)
{
	const auto secondPieceRelativePos = pieceToRemove.getRelativePosition();
	pieces.erase(std::remove_if(pieces.begin(), pieces.end(), [secondPieceRelativePos](const Piece *piece)
	{
		return secondPieceRelativePos == piece->getRelativePosition();
	}));
	delete &pieceToRemove;
}

bool MainWindow::isCellEmpty(const Cell &cell) const
{
	const auto cellRelativePosition = cell.relativePosition;
	const auto result = std::find_if(pieces.begin(), pieces.end(), [cellRelativePosition](const Piece *piece)
	{
		return cellRelativePosition == piece->getRelativePosition();
	});
	return result == pieces.end();
}

QPointer<Piece> MainWindow::getPiece(QPoint relativePosition)
{
	return *std::find_if(pieces.begin(), pieces.end(), [relativePosition](const Piece *piece){
		return piece->getRelativePosition() == relativePosition;
	});
}

QSharedPointer<Cell> MainWindow::getCell(QPoint relativePosition)
{
	for(auto &vec: cells) {
		QSharedPointer<Cell> *findResult = std::find_if(vec.begin(), vec.end(), [relativePosition](QSharedPointer<Cell> cell){
			return cell->relativePosition == relativePosition;
		});
		if(findResult != vec.end())
			return *findResult;
	}
	return QSharedPointer<Cell>();
}

MainWindow::~MainWindow()
{
	for(QPointer<Piece> &piece: pieces)
		piece->disconnect();

	for(auto &vec: cells) {
		for(auto &cell: vec)
			cell->disconnect();
	}
	delete ui;
}
