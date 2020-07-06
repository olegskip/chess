#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setGeometry(x(), y(), 800, 800);
	setMinimumSize(size());
	setMaximumSize(size());
	restart();

	victoryLabel.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	victoryLabel.button(QMessageBox::Ok)->setText("Restart");
	victoryLabel.button(QMessageBox::Cancel)->setText("Quit");
	QFont victoryLabelFont = victoryLabel.font();
	victoryLabelFont.setPixelSize(20);
	victoryLabelFont.setFamily("FreeMono Bold");
	victoryLabel.setFont(victoryLabelFont);
	connect(victoryLabel.button(QMessageBox::Ok), &QAbstractButton::clicked, this, [this]()
	{
		restart();
	});
	connect(victoryLabel.button(QMessageBox::Cancel), &QAbstractButton::clicked, this, [this]()
	{
		exit(1);
	});
}

void MainWindow::onPieceMoved(Piece &piece)
{
	QSharedPointer<Cell> &nearestCell = findNearestCell(piece.pos());
	if(piece.getRelativePosition() == nearestCell->relativePosition || !attemptToMove(piece, *nearestCell)) {
		// if the move isn't possible move back the piece
		piece.move(piece.getRelativePosition(), -1);
	}
	else if(activeCell){
		activeCell.reset();
	}
}

void MainWindow::onPieceClicked(Piece &piece)
{
	bool isMoved = false;
	if(activeCell) {
		if(piece.pieceOwner != currentTurn && !isCellEmpty(*cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()])) {
			auto pieceAtacker = getPiece(activeCell->relativePosition);
			QSharedPointer<Cell> lastCell = cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()];
			activeCell->deactivate();
			activeCell.reset();
			if(pieceAtacker)
				isMoved = attemptToMove(*pieceAtacker, *cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()]);
		}
		else {
			activeCell->deactivate();
			activeCell.reset();
		}
	}

	if(!isMoved && piece.pieceOwner == currentTurn && !isCellEmpty(*cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()])) {
		activeCell = cells[piece.getRelativePosition().x()][piece.getRelativePosition().y()];
		activeCell->activate(CellActiveType::SELECT);
	}
}

void MainWindow::onCellClicked(Cell &cell)
{
	QSharedPointer<Piece> pieceOnCell;

	if(activeCell) {
		pieceOnCell = getPiece(activeCell->relativePosition);
		activeCell.reset();
	}
	if(pieceOnCell) {
		attemptToMove(*pieceOnCell, cell);
	}
}

QVector<QPoint> MainWindow::getPointsOnWay(QPoint startPoint, QPoint endPoint)
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
		QVector<int> vecY = getFilledVector(startPoint.y(), endPoint.y());
		for(int i = 0; i < int(vecX.size()); ++i)
			pointsOnWay.push_back(QPoint(vecX[i], vecY[i]));
	}

	return pointsOnWay;
}

bool MainWindow::isWayClear(QVector<QPoint> pointsOnWay)
{
	return std::all_of(pointsOnWay.begin(), pointsOnWay.end(), [this](const QPoint &point) {
		return isCellEmpty(*getCell(point));
	});
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
		lastTurnCells.first->deactivate();
		lastTurnCells.second->deactivate();
	}

	if(oldCell && newCell) {
		lastTurnCells = {oldCell, newCell};
		newCell->activate(CellActiveType::LAST_TURN_CURRENT_CELL);
		oldCell->activate(CellActiveType::LAST_TURN_PREVIOUS_CELL);
	}
}

bool MainWindow::addPiece(QPoint relativePosition, PlayerColor color, PieceType pieceType)
{
	if(!isCellEmpty(*getCell(relativePosition)))
		return false;

	const QString &icon = [](PlayerColor _color, PieceType _pieceType)
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
	}(color, pieceType);
	pieces.push_back(QSharedPointer<Piece>(new Piece(newCentralWidget.data(),
													 cells[relativePosition.x()][relativePosition.y()]->geometry(),
					 relativePosition, icon, color, pieceType)));
	return true;
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
	bool _isCellEmpty = isCellEmpty(cell);
	QSharedPointer<Piece> pieceDefender;
	if(!_isCellEmpty)
		pieceDefender = getPiece(cell.relativePosition);
	QSharedPointer<Piece> enPassantResult = enPassant(piece, cell);
	if(piece.pieceOwner == currentTurn && ((_isCellEmpty && isMovePossible(piece, cell)) ||
										   (!_isCellEmpty && piece.pieceOwner != pieceDefender->pieceOwner && isCanCapture(piece, cell)) ||
										   enPassantResult)) {
		QSharedPointer<Cell> cellPtr = getCell(QPoint(cell.relativePosition.x(), cell.relativePosition.y()));

		// if the move is possible move the piece
		QPoint oldPieceRelPos = piece.getRelativePosition();
		if(enPassantResult)
			pieceDefender = enPassantResult;

		piece.move(cell.relativePosition, currentTurnCount);

		QVector<QSharedPointer<Piece>> kings = getPieces(PieceType::KING);
		if(checkedCell) {
			checkedCell->deactivate();
			checkedCell.reset();
		}
		for(auto &king: kings) {
			if(king) {
				if(getCellAtacker(*cells[king->getRelativePosition().x()][king->getRelativePosition().y()], king->pieceOwner, pieceDefender)) {
					if(king->pieceOwner == piece.pieceOwner) {
						piece.move(oldPieceRelPos, -1);
						checkedCell = cells[king->getRelativePosition().x()][king->getRelativePosition().y()];
						checkedCell->activate(CellActiveType::KING_CHECKED);
						return false;
					}
				}
			}
		}

		if(pieceDefender)
			removePiece(*pieceDefender);

		if(cellPtr)
			highlightLastTurn(cells[oldPieceRelPos.x()][oldPieceRelPos.y()], cellPtr);

		if(currentTurn == PlayerColor::WHITE)
			currentTurn = PlayerColor::BLACK;
		else if(currentTurn == PlayerColor::BLACK) {
			currentTurn = PlayerColor::WHITE;
			currentTurnCount++;
		}

		const PlayerColor _isCheckmate = isCheckmate();
		if(_isCheckmate != PlayerColor::NONE) {
			const QString winnerName = _isCheckmate == PlayerColor::WHITE ? "Black": "White";
			victoryLabel.setInformativeText(QString("<p style='text-align: center;'>Are ya winning son?</p>\n<p style='text-align: center;'> %1 won!</p>").arg(winnerName));
			victoryLabel.show();
		}

		return true;
	}
	return false;
}

bool MainWindow::isMovePossible(Piece &piece, const Cell &cell)
{
	const QPoint piece1RelativePos = piece.getRelativePosition();
	const QPoint cellRelativePos = cell.relativePosition;

	if(!isWayClear(getPointsOnWay(piece.getRelativePosition(), cellRelativePos)))
			return false;

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

	if(piece.pieceType == PieceType::KING) {
		if(checkNearMove())
			return true;

		// castling
		// TO DO
//		else if(!piece.isMoved() && cellRelativePos.y() == piece.getRelativePosition().y()) {
//			QSharedPointer<Piece> rook;
//			if(cellRelativePos.x() == 6) {
//				rook = getPiece(QPoint(7, cellRelativePos.y()));
//				if(rook) {
//					bool isCanCastle = true;
//					for(auto item: getFilledVector(piece.getRelativePosition().x, 7)) {
//						if(getCellAtacker(cells[item][piece.getRelativePosition().y()])) {
//							rook->move(QPoint(5, cellRelativePos.y()));
//							return true;
//						}
//					}
//				}
//				else
//					return false;
//			}
//			else if(cellRelativePos.x() == 2) {
//				rook = getPiece(QPoint(0, cellRelativePos.y()));
//				if(rook && !rook->isMoved()) {
//					rook->move(QPoint(3, cellRelativePos.y()));
//					return true;
//				}
//				else
//					return false;
//			}
//			else
//				return false;
//		}
	}

	else if(piece.pieceType == PieceType::QUEEN && (checkLinearMove() || checkDiagonalMove()))
			return true;

	else if(piece.pieceType == PieceType::ROOK && checkLinearMove())
			return true;

	else if(piece.pieceType == PieceType::BISHOP && checkDiagonalMove())
			return true;

	else if(piece.pieceType == PieceType::KNIGHT) {
		if((std::abs(piece1RelativePos.x() - cellRelativePos.x()) == 1 && std::abs(piece1RelativePos.y() - cellRelativePos.y()) == 2) ||
				(std::abs(piece1RelativePos.x() - cellRelativePos.x()) == 2 && std::abs(piece1RelativePos.y() - cellRelativePos.y()) == 1))
			return true;
	}

	else if(piece.pieceType == PieceType::PAWN) {
		// the white pawn can go only up, the black only down
		if(piece.getRelativePosition().x() == cellRelativePos.x() &&
				((piece.pieceOwner == PlayerColor::WHITE && ((!piece.isMoved() && piece1RelativePos.y() + 2 == cellRelativePos.y()) ||
															 (piece1RelativePos.y() + 1 == cellRelativePos.y()))) ||
				(piece.pieceOwner == PlayerColor::BLACK && ((!piece.isMoved() && piece1RelativePos.y() - 2 == cellRelativePos.y()) ||
															(piece1RelativePos.y() - 1 == cellRelativePos.y())))))
			return true;
	}

	return false;
}

bool MainWindow::isCanCapture(Piece &pieceAtacker, const Cell &cell)
{
	const QPoint pieceAtackerRelativePos = pieceAtacker.getRelativePosition();
	const QPoint cellRelativePos = cell.relativePosition;

	// only the pawn doesn't capture like move
	if(pieceAtacker.pieceType == PieceType::PAWN) {
		if((pieceAtackerRelativePos.x() - 1 == cellRelativePos.x() || pieceAtackerRelativePos.x() + 1 == cellRelativePos.x()) &&
				((pieceAtacker.pieceOwner == PlayerColor::WHITE && pieceAtackerRelativePos.y() + 1 == cellRelativePos.y()) ||
				(pieceAtacker.pieceOwner == PlayerColor::BLACK && pieceAtackerRelativePos.y() - 1 == cellRelativePos.y())))
			return true;


		return false;
	}
	else
		return isMovePossible(pieceAtacker, cell);
}

QSharedPointer<Piece> MainWindow::enPassant(Piece &pieceAtacker, const Cell &cell)
{
	if(pieceAtacker.pieceType != PieceType::PAWN) return QSharedPointer<Piece>();
	const QPoint pieceAtackerRelativePos = pieceAtacker.getRelativePosition();
	const QPoint cellRelativePos = cell.relativePosition;

	// I could use QPair, but then I would haven't could use for each
	const std::array<QSharedPointer<Piece>, 2> enPassantPawns =
	{getPiece(QPoint(pieceAtackerRelativePos.x() + 1, pieceAtackerRelativePos.y())),
	 getPiece(QPoint(pieceAtackerRelativePos.x() - 1, pieceAtackerRelativePos.y()))};

	for(QSharedPointer<Piece> enPassantPawn: enPassantPawns) {
		if(enPassantPawn && enPassantPawn->pieceType == PieceType::PAWN &&
				enPassantPawn->getLastMove() == currentTurnCount - 1 && enPassantPawn->getMoveCount() == 1 &&
				((pieceAtacker.pieceOwner == PlayerColor::WHITE && cellRelativePos.y() == 5) ||
				 (pieceAtacker.pieceOwner == PlayerColor::BLACK && cellRelativePos.y() == 2)) &&
				((pieceAtacker.pieceOwner == PlayerColor::WHITE && pieceAtackerRelativePos.y() == 4) ||
				 (pieceAtacker.pieceOwner == PlayerColor::BLACK && pieceAtackerRelativePos.y() == 3)) &&
				((enPassantPawn->pieceOwner == PlayerColor::WHITE && enPassantPawn->getRelativePosition().y() == 3) ||
				(enPassantPawn->pieceOwner == PlayerColor::BLACK && enPassantPawn->getRelativePosition().y() == 4))) {
			return enPassantPawn;
		}
	}

	return QSharedPointer<Piece>();
}

bool MainWindow::isCellEmpty(const Cell &cell) const
{
	const auto cellRelativePosition = cell.relativePosition;
	const QSharedPointer<Piece> *result = std::find_if(pieces.begin(), pieces.end(), [cellRelativePosition](const QSharedPointer<Piece> &piece)
	{
		return piece->isHasCollision && cellRelativePosition == piece->getRelativePosition();
	});
	return result == pieces.end();
}

PlayerColor MainWindow::isCheckmate()
{
	const auto kings = getPieces(PieceType::KING);

	for(QSharedPointer<Piece> king: kings) {
		if(getCellAtacker(*cells[king->getRelativePosition().x()][king->getRelativePosition().y()], king->pieceOwner)) {
			for(auto &vec: cells) {
				for(auto &cell: vec) {
					if(isCellEmpty(*cell)) {
						// the king can just run away
						king->isHasCollision = false;
						if(!getCellAtacker(*cell, king->pieceOwner, king) && isMovePossible(*king, *cell)) {
							king->isHasCollision = true;
							return PlayerColor::NONE;
						}
						king->isHasCollision = true;
					}
					// if the king can capture the atacker
					if(!getCellAtacker(*cell, king->pieceOwner) && !isCellEmpty(*cell) &&
							king->pieceOwner != getPiece(cell->relativePosition)->pieceOwner && isCanCapture(*king, *cell))
						return PlayerColor::NONE;
				}
			}
			// if the king has nothing to do, maybe the others can help

			QSharedPointer<Piece> atacker = getCellAtacker(*getCell(king->getRelativePosition()),
					king->pieceOwner);
			QVector<QPoint> pointsOnWay = getPointsOnWay(atacker->getRelativePosition(), king->getRelativePosition());
			for(auto &piece: pieces) {
				// only the allies of the king can help
				if(piece->pieceOwner != king->pieceOwner) continue;
				// There are conditions above when the king can save by himself
				if(piece->pieceType == PieceType::KING) continue;

				QSharedPointer<Cell> cell = getCell(atacker->getRelativePosition());
				// if the others can capture the atacker
				if(isCanCapture(*piece, *cell) &&
						getCellAtacker(*cell, king->pieceOwner, atacker))
					return PlayerColor::NONE;
				// or en passant may help
				else {
					const int dy = atacker->pieceOwner == PlayerColor::WHITE ? -1: 1;
					if(atacker->pieceType == PieceType::PAWN &&
							enPassant(*piece, *getCell(QPoint(atacker->getRelativePosition().x(),
															  atacker->getRelativePosition().y() + dy)))) {
						QPoint pieceOldPOsition = piece->getRelativePosition();
						piece->move(cell->relativePosition, -1);
						piece->isHasCollision = false;
						bool cellAtacker = getCellAtacker(*getCell(king->getRelativePosition()), king->pieceOwner, atacker);
						piece->isHasCollision = true;
						piece->move(pieceOldPOsition, -1);
						if(!cellAtacker)
							return PlayerColor::NONE;
					}
				}

				// or somebody can hide the king
				for(QPoint pointOnWay: pointsOnWay) {
					if(isMovePossible(*piece, *getCell(pointOnWay))) {
						QPoint oldPosition = piece->getRelativePosition();
						piece->move(pointOnWay, -1);
						auto isCellChecked = getCellAtacker(*getCell(king->getRelativePosition()), king->pieceOwner);
						piece->move(oldPosition, -1);

						if(!isCellChecked)
							return PlayerColor::NONE;
					}
				}
			}
			return king->pieceOwner;
		}
	}

	return PlayerColor::NONE;
}

void MainWindow::removePiece(const Piece &pieceToRemove)
{
	const auto pieceToRemoveRelativePos = pieceToRemove.getRelativePosition();
	const auto pieceToRemoveOwner = pieceToRemove.pieceOwner;
	auto removeResult = std::find_if(pieces.begin(), pieces.end(), [pieceToRemoveRelativePos, pieceToRemoveOwner](const QSharedPointer<Piece> &piece)
	{
		return pieceToRemoveRelativePos == piece->getRelativePosition() &&
				pieceToRemoveOwner == piece->pieceOwner;
	});

	if(removeResult != pieces.end()) {
		(*removeResult)->disconnect();
		pieces.erase(removeResult);
	}
}

QSharedPointer<Piece> MainWindow::getPiece(QPoint relativePosition)
{
	auto findResult = std::find_if(pieces.begin(), pieces.end(), [relativePosition](QSharedPointer<Piece> &piece){
		return piece->getRelativePosition() == relativePosition;
	});

	if(findResult != pieces.end())
		return *findResult;
	else
		return QSharedPointer<Piece>();
}

QVector<QSharedPointer<Piece>> MainWindow::getPieces(PieceType pieceType)
{
	QVector<QSharedPointer<Piece>> _pieces;
	for(auto &piece: pieces) {
		if(piece->pieceType == pieceType)
			_pieces.push_back(piece);
	}

	return _pieces;
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

QSharedPointer<Piece> MainWindow::getCellAtacker(const Cell &cell, PlayerColor kingColor, const QSharedPointer<Piece> &checkWithout)
{
	QSharedPointer<Piece> pieceOnCell = getPiece(cell.relativePosition);

	for(auto &piece: pieces) {
		if(piece != checkWithout && piece != pieceOnCell && piece->pieceOwner != kingColor &&
				(isCanCapture(*piece, cell)))
			return piece;
	}

	return QSharedPointer<Piece>();
}


void MainWindow::restart()
{
	currentTurnCount = 1;
	currentTurn = PlayerColor::WHITE;
	activeCell.reset();
	lastTurnCells = {QSharedPointer<Cell>(), QSharedPointer<Cell>()};
	checkedCell.reset();

	for(auto &vec: cells) {
		for(auto &cell: vec)
			cell->disconnect();
	}
	cells.clear();
	newCentralWidget = QPointer<QWidget>(new QWidget(this));


	// the chessboard is 8x8
	for(int x = 0; x <= 7; ++x) {
		cells.push_back(QVector<QSharedPointer<Cell>>());
		for(int y = 0; y <= 7; ++y) {
			CellType cellType;
			if((x % 2 == 0 && y % 2 == 0) || (x % 2 != 0 && y % 2 != 0))
				cellType = CellType::WHITE;
			else
				cellType = CellType::BLACK;

			cells[x].push_back(QSharedPointer<Cell>::create(newCentralWidget.data(), QRect(x * config::CELL_SIDE_SIZE, 7 * config::CELL_SIDE_SIZE - y * config::CELL_SIDE_SIZE,
																													config::CELL_SIDE_SIZE, config::CELL_SIDE_SIZE), QPoint(x, y), cellType));

		}
	}
	setCentralWidget(newCentralWidget);

	// connects
	for(auto &vec: cells) {
		for(QSharedPointer<Cell> cell: vec) {
			connect(cell.data(), &Cell::clicked, this, [this, cell]()
			{
				onCellClicked(*cell);
			});
		}
	}


	for(QSharedPointer<Piece> &piece: pieces)
		piece->disconnect();
	pieces.clear();
	// fill the chessboard
	addPiece(QPoint(4, 0), PlayerColor::WHITE, PieceType::KING);
	addPiece(QPoint(4, 7), PlayerColor::BLACK, PieceType::KING);

	addPiece(QPoint(3, 0), PlayerColor::WHITE, PieceType::QUEEN);
	addPiece(QPoint(3, 7), PlayerColor::BLACK, PieceType::QUEEN);

	addPiece(QPoint(0, 0), PlayerColor::WHITE, PieceType::ROOK);
	addPiece(QPoint(7, 0), PlayerColor::WHITE, PieceType::ROOK);
	addPiece(QPoint(0, 7), PlayerColor::BLACK, PieceType::ROOK);
	addPiece(QPoint(7, 7), PlayerColor::BLACK, PieceType::ROOK);

	addPiece(QPoint(2, 0), PlayerColor::WHITE, PieceType::BISHOP);
	addPiece(QPoint(5, 0), PlayerColor::WHITE, PieceType::BISHOP);
	addPiece(QPoint(2, 7), PlayerColor::BLACK, PieceType::BISHOP);
	addPiece(QPoint(5, 7), PlayerColor::BLACK, PieceType::BISHOP);

	addPiece(QPoint(1, 0), PlayerColor::WHITE, PieceType::KNIGHT);
	addPiece(QPoint(6, 0), PlayerColor::WHITE, PieceType::KNIGHT);
	addPiece(QPoint(1, 7), PlayerColor::BLACK, PieceType::KNIGHT);
	addPiece(QPoint(6, 7), PlayerColor::BLACK, PieceType::KNIGHT);

	for(int x = 0; x <= 7; ++x) {
		addPiece(QPoint(x, 1), PlayerColor::WHITE, PieceType::PAWN);
		addPiece(QPoint(x, 6), PlayerColor::BLACK, PieceType::PAWN);
	}
	setCentralWidget(newCentralWidget);
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

MainWindow::~MainWindow()
{
	for(QSharedPointer<Piece> &piece: pieces)
		piece->disconnect();

	for(auto &vec: cells) {
		for(auto &cell: vec)
			cell->disconnect();
	}
}
