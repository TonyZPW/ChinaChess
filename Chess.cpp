#include "Chess.h"


bool Chess::init()
{
	Layer::init();

	auto s = Director::getInstance()->getWinSize();

	
	_bRedTurn = true;

	// 放一个棋盘
	Sprite* sprite = Sprite::create("chess/background.png");
	addChild(sprite);
	sprite->setPosition(s / 2);

	sprite->setScale(s.height / sprite->getContentSize().height);


	// 计算棋盘格子直径
	_d = s.height / 10;
	_off = Vec2((s.width - sprite->getBoundingBox().size.width) / 2 + _d, _d / 2);

	_selectSprite = Sprite::create("chess/selected.png");
	addChild(_selectSprite);
	_selectSprite->setVisible(false);
	_selectSprite->setScale(_d / _selectSprite->getContentSize().width);

	for (int row = 0; row <= 9; ++row)
	for (int col = 0; col <= 8; ++col)
	{
		stoneIds[row][col] = -1;
	}

	// 创建32个棋子
	for (int i = 0; i < 32; ++i)
	{
		_s[i] = Stone::create(i);
		addChild(_s[i]);
		_s[i]->setPosition(Plane2GL(_s[i]->_row, _s[i]->_col));

		_s[i]->setScale(_d / _s[i]->getContentSize().width);

		stoneIds[_s[i]->_row][_s[i]->_col] = i;
	}

	// 触摸处理
	_selectId = -1;
	auto ev = EventListenerTouchOneByOne::create();
	ev->onTouchBegan = [](Touch*, Event*){return true; };
	ev->onTouchEnded = [&](Touch* touch, Event*){
		if (_selectId != -1)
			tryMoveStone(touch);
		else
			trySelectStone(touch);
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(ev, this);

	return true;
}

bool Chess::canMoveJiang(int id, int row, int col, int killid)
{
	if (killid != -1 && _s[killid] ->_type == Stone::JIANG)
	{
		return canMoveChe(id ,row, col, killid);
	}

	if (col <3 || col >5)return false;
	if (_bRedTurn)
	{
		if (row > 2)return false;
	}
	else
	{
		if (row < 7)return false;
	}

	
	int d = abs(row - _s[id]->_row) * 10 + abs(col - _s[id]->_col);
	if (d == 10 || d == 01) return true;

	return false;
}

bool Chess::canMoveShi(int id, int row, int col, int killid)
{
	if (col <3 || col >5)return false;
	if (_bRedTurn)
	{
		if (row > 2)return false;
	}
	else
	{
		if (row < 7)return false;
	}


	int d = abs(row - _s[id]->_row) * 10 + abs(col - _s[id]->_col);
	if (d == 11 ) return true;

	return false;
}

bool Chess::canMoveXiang(int id, int row, int col, int killid)
{
	if (_bRedTurn)
	{
		if (row > 4)return false;
	}
	else
	{
		if (row < 5)return false;
	}


	int d = abs(row - _s[id]->_row) * 10 + abs(col - _s[id]->_col);
	if (d != 22) return false;

	// 考虑象眼
	int eRow = (row + _s[id]->_row) / 2;
	int eCol = (col + _s[id]->_col) / 2;
	
	if (getStoneId(eRow, eCol) != -1)
	{
		return false;
	}

	return true;
}

bool Chess::canMoveChe(int id, int row, int col, int killid)
{
	int count = getStoneCount(_s[id]->_row, _s[id]->_col, row, col);
	return count == 0;
}

bool Chess::canMoveMa(int id, int row, int col, int killid)
{
	int d = abs(row - _s[id]->_row) * 10 + abs(col - _s[id]->_col);
	if (d != 12 && d!=21) return false;

	// 找马腿
	if (d == 12)
	{
		int eCol = (col + _s[id]->_col) / 2;
		int eRow = _s[id]->_row;

		int id = getStoneId(eRow, eCol);
		if (id != -1)
			return false;
	}
	else
	{
		int eRow = (row + _s[id]->_row) / 2;
		int eCol = _s[id]->_col;

		int id = getStoneId(eRow, eCol);
		if (id != -1)
			return false;

	}

	return true;
}



bool Chess::canMovePao(int id, int row, int col, int killid)
{
	if (killid == -1)
	{
		return getStoneCount(_s[id]->_row, _s[id]->_col, row, col) == 0;
	}
	
	return getStoneCount(_s[id]->_row, _s[id]->_col, row, col) == 1;	
}

bool Chess::canMoveBing(int id, int row, int col, int killid)
{
	int d = abs(row - _s[id]->_row) * 10 + abs(col - _s[id]->_col);
	if (d != 10 && d != 01) return false;

	if (_bRedTurn)
	{
		if (row < _s[id]->_row)return false;
		if (row == _s[id]->_row && _s[id]->_row<=4) return false;
	}
	else
	{
		if (row > _s[id]->_row)return false;
		if (row == _s[id]->_row && _s[id]->_row >= 5) return false;
	}
	

	return true;
}

bool Chess::canMove(int id, int row, int col, int killid)
{
	switch (_s[id]->_type)
	{
	case Stone::JIANG:
		return canMoveJiang(id, row, col, killid);
	case Stone::SHI:
		return canMoveShi(id, row, col, killid);
	case Stone::XIANG:
		return canMoveXiang(id, row, col, killid);
	case Stone::CHE:
		return canMoveChe(id, row, col, killid);
	case Stone::MA:
		return canMoveMa(id, row, col, killid);
	case Stone::PAO:
		return canMovePao(id, row, col, killid);
	case Stone::BING:
		return canMoveBing(id, row, col, killid);
	default:
		break;
	}
	return true;
}

// 如果row1, col1与row2，col2不在一条直线上，那么返回-1
// 要不然返回0 或者1 或者2
int Chess::getStoneCount(int row1, int col1, int row2, int col2)
{
	int ret = 0;
	if (row1 != row2 && col1 != col2)
		return -1;

	
	if (row1 == row2)
	{
		int min = col1 < col2 ? col1 : col2;
		int max = col1 > col2 ? col1 : col2;
		for (int col = min+1; col < max; ++col)
		{
			int id = getStoneId(row1, col);
			if (id != -1) ret++;
			if (ret >= 2) return ret;
		}
	}
	else // 
	{
		int min = row1 < row2 ? row1 : row2;
		int max = row1 > row2 ? row1 : row2;
		for (int row = min + 1; row < max; ++row)
		{
			int id = getStoneId(row, col1);
			if (id != -1)ret++;
			if (ret >= 2)return ret;
		}
	}

	return ret;
}
void Chess::tryMoveStone(Touch* touch)
{
	int row, col;
	if (GL2Plane(row, col, touch->getLocation()))
	{
		// 检查该位置是否有棋子
		int id = getStoneId(row, col);
		if (id != -1)
		{
			if (_s[id]->isRed() == _bRedTurn)
			{
				_selectId = id;

				_selectSprite->setVisible(true);
				_selectSprite->setPosition(_s[id]->getPosition());

				return;
			}
			
		}

		if (this->canMove(_selectId, row, col, id))
		{

			setStoneID(_s[_selectId]->_row, _s[_selectId]->_col, -1);


			_s[_selectId]->setPosition(Plane2GL(row, col));
			_s[_selectId]->_row = row;
			_s[_selectId]->_col = col;

			setStoneID(_s[_selectId]->_row, _s[_selectId]->_col, _selectId);

			_selectSprite->setVisible(false);
			_selectId = -1;

			_bRedTurn = !_bRedTurn;

			if (id != -1)
			{
				_s[id]->_dead = 1;
				_s[id]->setVisible(false);
			}
		}
	}

	// 由黑走
	if (this->_bRedTurn == false)
	{
		DelayTime* d = DelayTime::create(0.02f);
		CallFunc* call = CallFunc::create([&](){
			ComputerMove(5);
		});
		Sequence* seq = Sequence::createWithTwoActions(d, call);
		this->runAction(seq);

		
	}
}

int Chess::calcScore()
{
	int red = 0, black = 0;
	for (int i = 0; i < 16; ++i)
	{
		if (_s[i]->_dead ==0)
			red += _s[i]->_score;
	}
	for (int i = 16; i < 32; ++i)
	{
		if (_s[i]->_dead == 0)
			black += _s[i]->_score;
	}
	return black - red;
}

void Chess::getAllPossibleMoveJiang(Vector<Step*>& steps, int id)
{
	for (int col = 3; col <= 5; ++col)
	{
		for (int row = 0; row <= 2; row++)
		{
			addMove(steps, id, row, col);
		}
		for (int row = 7; row <= 9; row++)
		{
			addMove(steps, id, row, col);
		}
	}
}

void Chess::getAllPossibleMoveShi(Vector<Step*>& steps, int id)
{
	if (_bRedTurn)
	{
		for (int col = 3; col <= 5; ++col)
		{
			for (int row = 0; row <= 2; row++)
			{
				addMove(steps, id, row, col);
			}
			
		}
	}
	else
	{
		for (int col = 3; col <= 5; ++col)
		{
			for (int row = 7; row <= 9; row++)
			{
				addMove(steps, id, row, col);
			}
		}
	}
}

void Chess::getAllPossibleMoveXiang(Vector<Step*>& steps, int id)
{
	int row = _s[id]->_row;
	int col = _s[id]->_col;

	struct
	{
		int row;
		int col;
	} off[4] = 
	{
		{2, 2},
		{-2, -2},
		{2, -2},
		{-2, 2}
	};
	for (int i = 0; i < 4; ++i)
	{
		int rowTo = row + off[i].row;
		int colTo = col + off[i].col;
		if (rowTo < 0 || rowTo>9)
			continue;
		if (colTo<0 || colTo>8)
			continue;

		addMove(steps, id, rowTo, colTo);
	}

}

void Chess::getAllPossibleMoveChe(Vector<Step*>& steps, int id)
{
	for (int row = 0; row <= 9; ++row)
	{
		addMove(steps, id, row, _s[id]->_col);
	}

	for (int col = 0; col <= 8; ++col)
	{
		addMove(steps, id, _s[id]->_row, col);
	}
}

void Chess::getAllPossibleMoveMa(Vector<Step*>& steps, int id)
{
	struct
	{
		int row;
		int col;
	} off[8] =
	{
		{ 2, 1 },
		{ 1, 2 },
		{ -2, 1 },
		{ 1, -2 },

		{ -2, -1 },
		{ -1, -2 },
		{ 2, -1 },
		{ -1, 2 },
	};

	int row = _s[id]->_row;
	int col = _s[id]->_col;

	for (int i = 0; i < 8; ++i)
	{
		int rowTo = row + off[i].row;
		int colTo = col + off[i].col;
		if (rowTo < 0 || rowTo>9)
			continue;
		if (colTo<0 || colTo>8)
			continue;

		addMove(steps, id, rowTo, colTo);
	}
}

void Chess::getAllPossibleMovePao(Vector<Step*>& steps, int id)
{
	for (int row = 0; row <= 9; ++row)
	{
		addMove(steps, id, row, _s[id]->_col);
	}

	for (int col = 0; col <= 8; ++col)
	{
		addMove(steps, id, _s[id]->_row, col);
	}
}

void Chess::getAllPossibleMoveBing(Vector<Step*>& steps, int id)
{
	int row = _s[id]->_row;
	int col = _s[id]->_col;

	struct
	{
		int row;
		int col;
	} off[4] =
	{
		{ 1, 0 },
		{ 0, 1 },
		{ -1, 0 },
		{ 0, -1 }
	};
	for (int i = 0; i < 4; ++i)
	{
		int rowTo = row + off[i].row;
		int colTo = col + off[i].col;
		if (rowTo < 0 || rowTo>9)
			continue;
		if (colTo<0 || colTo>8)
			continue;

		addMove(steps, id, rowTo, colTo);
	}
}

void Chess::getAllPossibleMove(Vector<Step*>& steps)
{
	// 遍历所有的黑棋
	int min = 0;
	int max = 16;
	if (_bRedTurn == false)
	{
		min = 16; max = 32;
	}
	for (int i = min; i < max; ++i)
	{
		Stone* s = _s[i];
		if (s->_dead) continue;

		switch(s->_type)
		{
		case Stone::JIANG:
			getAllPossibleMoveJiang(steps, i);
			break;
		case Stone::SHI:
			getAllPossibleMoveShi(steps, i);
			break;
		case Stone::XIANG:
			getAllPossibleMoveXiang(steps, i);
			break;
		case Stone::CHE:
			getAllPossibleMoveChe(steps, i);
			break;
		case Stone::MA:
			getAllPossibleMoveMa(steps, i);
			break;
		case Stone::PAO:
			getAllPossibleMovePao(steps, i);
			break;
		case Stone::BING:
			getAllPossibleMoveBing(steps, i);
			break;
				
		}
#if 0
		for (int row = 0; row <= 9; ++row)
		for (int col = 0; col <= 8; ++col)
		{
			int killid = getStoneId(row, col);
			if (killid != -1 && _s[killid]->isRed() == s->isRed()) continue;

			bool b = canMove(i, row, col, killid);
			if (b)
			{
				Step* step = Step::create(i, s->_row, s->_col, row, col, killid);
				steps.pushBack(step);
			}
		}
#endif
	}
}

void Chess::fakeMove(Step* step)
{
	setStoneID(step->_rowFrom, step->_colFrom, -1);
	setStoneID(step->_rowTo, step->_colTo, step->_id);

	_s[step->_id]->_row = step->_rowTo;
	_s[step->_id]->_col = step->_colTo;

	

	if (step->_killid != -1)
	{
		_s[step->_killid]->_dead = 1;
	}
	_bRedTurn = !_bRedTurn;
}
void Chess::fakeBack(Step* step)
{

	setStoneID(step->_rowFrom, step->_colFrom, step->_id);
	setStoneID(step->_rowTo, step->_colTo, step->_killid);

	_s[step->_id]->_row = step->_rowFrom;
	_s[step->_id]->_col = step->_colFrom;
	if (step->_killid != -1)
	{
		_s[step->_killid]->_dead = 0;
	}
	_bRedTurn = !_bRedTurn;
}

int Chess::getMaxScore(int level, int curBest)
{
	if (level == 0)
		return calcScore();

	Vector<Step*> steps;
	getAllPossibleMove(steps);

	int maxScore = -300000;

	for (auto it = steps.begin(); it != steps.end(); ++it)
	{
		fakeMove(*it);
		int score = getMinScore(level - 1, maxScore);
		fakeBack(*it);

		// 减枝
		if (score >= curBest)
			return score;

		if (score > maxScore)
			maxScore = score;
		
	}

	return maxScore;
}

int Chess::getMinScore(int level, int curBest)
{
	if (level == 0)
		return calcScore();

	Vector<Step*> steps;
	getAllPossibleMove(steps);

	int minScore = 300000;

	for (auto it = steps.begin(); it != steps.end(); ++it)
	{
		fakeMove(*it);
		int score = getMaxScore(level - 1, minScore);
		fakeBack(*it);

		// 减枝
		if (score <= curBest)
			return score;

		if (score < minScore)
			minScore = score;
		
	}

	return minScore;
}
void Chess::ComputerMove(int level)
{
	// 把所有移动的可能，都走一遍，然后计算局面分，那个分最好就哪个
	Vector<Step*> steps;
	getAllPossibleMove(steps);

	int maxScore= -300000;
	Step* bestMove = NULL;

	for (auto it = steps.begin(); it != steps.end(); ++it)
	{
		Step* step = *it;

		fakeMove(step);

		int score = getMinScore(level - 1, maxScore);
		if (score > maxScore)
		{
			maxScore = score;
			bestMove = step;
		}
#if 0
		int score = calcScore();
		
#endif
		fakeBack(step);
	}

	Move(bestMove);
}

void Chess::Move(Step* step)
{
	setStoneID(step->_rowTo, step->_colTo, step->_id);

	_s[step->_id]->_row = step->_rowTo;
	_s[step->_id]->_col = step->_colTo;
	_s[step->_id]->setPosition(Plane2GL(step->_rowTo, step->_colTo));
	if (step->_killid != -1)
	{
		_s[step->_killid]->_dead = 1;
		_s[step->_killid]->setVisible(false);
	}

	_bRedTurn = !_bRedTurn;
	_selectId = -1;
}

void Chess::trySelectStone(Touch* touch)
{
	// 人不能走黑棋
	if (_bRedTurn == false)
		return;

	// 要将touch的位置转化成棋盘坐标，然后再检查坐标上有没有象棋
	int row, col;
	if (GL2Plane(row, col, touch->getLocation()))
	{
		int id = getStoneId(row, col);
		
		if (id != -1)
		{
			// 当红方走时，只能选择红放的棋子
			if (_s[id]->isRed() != _bRedTurn)
				return;

			_selectSprite->setVisible(true);
			_selectSprite->setPosition(_s[id]->getPosition());
		}

		this->_selectId = id;
	}
}

int Chess::getStoneId(int row, int col)
{
#if 0
	for (int i = 0; i < 32; ++i)
	{
		if (_s[i]->_row == row && _s[i]->_col == col && _s[i]->_dead == 0)
			return i;
	}
	return -1;
#endif

	return stoneIds[row][col];
}

bool Chess::GL2Plane(int& row, int &col, Vec2 gl)
{
	gl -= _off;
	gl += Vec2(_d / 2, _d / 2);

	col = gl.x / _d;
	row = gl.y / _d;

	if (row <0 || row >9) return false;
	if (col < 0 || col >8)return false;
	return true;
}

Vec2 Chess::Plane2GL(int row, int col)
{
	Vec2 ret;
	ret.x = col* _d;
	ret.y = row* _d;

	return ret + _off;
}