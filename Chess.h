#ifndef __Chess_H__
#define __Chess_H__

#include "cocos2d.h"
USING_NS_CC;

// 记录可能的步骤
class Step : public Ref
{
public:

	static Step* create(int id, int rowFrom, int colFrom, int rowTo, int colTo, int killid)
	{
		Step* s = new Step;

		s->_id = id;
		s->_rowFrom = rowFrom;
		s->_colFrom = colFrom;
		s->_rowTo = rowTo;
		s->_colTo = colTo;
		s->_killid = killid;
		s->autorelease();
		return s;
	}
	int _id;
	int _rowFrom;
	int _colFrom;
	int _rowTo;
	int _colTo;
	int _killid;
};

class Stone : public Sprite
{
public:
	static Stone* create(int id)
	{
		Stone* ret = new Stone;
		ret->init(id);
		ret->autorelease();
		return ret;
	}
	enum Type{ JIANG, SHI, XIANG, CHE, MA, PAO, BING };
	Type _type;

	bool init(int id)
	{
		struct StoneInfo
		{
			int row;
			int col;
			Type type;
		} infos[32]
			= {
				{0, 0, CHE},
				{0, 1, MA},
				{0, 2, XIANG},
				{0, 3, SHI},
				{0, 4, JIANG},
				{0, 5, SHI},
				{0, 6, XIANG},
				{0, 7, MA},
				{0, 8, CHE},
				{2, 1, PAO},
				{2, 7, PAO},
				{3, 0, BING},
				{3, 2, BING},
				{3, 4, BING},
				{ 3, 6, BING },
				{ 3, 8, BING }
		};

		StoneInfo info;
		if (id< 16)
			info = infos[id];
		else
		{
			info = infos[id - 16];
			info.row = 9 - info.row;
			info.col = 8 - info.col;
		}

		const char* files[] = {
			"chess/bjiang.png",
			"chess/bshi.png",
			"chess/bxiang.png",
			"chess/bche.png",
			"chess/bma.png",
			"chess/bpao.png",
			"chess/bzu.png",

			"chess/rshuai.png",
			"chess/rshi.png",
			"chess/rxiang.png",
			"chess/rche.png",
			"chess/rma.png",
			"chess/rpao.png",
			"chess/rbing.png"
		};

		int fileIndex = info.type;
		if (id < 16) fileIndex += 7;
		Sprite::initWithFile(files[fileIndex]);

		this->_row = info.row;
		this->_col = info.col;
		this->_dead = 0;
		this->_id = id;
		this->_type = info.type;

		static int score[] = { 15000, 10, 10, 100, 50, 50, 20 };
		this->_score = score[_type];
		
		return true;
	}

	

	int _id; /* 0 ~ 31 */
	int _row, _col; // 棋盘坐标
	int _dead; // 是不是死掉了
	int _score;


	bool isRed()
	{
		return _id < 16;
	}

	
};

class Chess : public Layer
{
public:
	CREATE_FUNC(Chess);
	bool init();


	void getAllPossibleMove(Vector<Step*>& out);

	void addMove(Vector<Step*>& out, int id, int row, int col)
	{
		int killid = getStoneId(row, col);
		if (killid != -1 && _s[killid]->isRed() == _s[id]->isRed())
			return;

		if (canMove(id, row, col, killid))
		{
			Step* step = Step::create(id, _s[id]->_row, _s[id]->_col, row, col, killid);
			out.pushBack(step);
		}
	}

	void getAllPossibleMoveJiang(Vector<Step*>& out, int id);
	void getAllPossibleMoveShi(Vector<Step*>& out, int id);
	void getAllPossibleMoveXiang(Vector<Step*>& out, int id);
	void getAllPossibleMoveChe(Vector<Step*>& out, int id);
	void getAllPossibleMoveMa(Vector<Step*>& out, int id);
	void getAllPossibleMovePao(Vector<Step*>& out, int id);
	void getAllPossibleMoveBing(Vector<Step*>& out, int id);

	int stoneIds[10][9];
	void setStoneID(int row, int col, int value)
	{
		stoneIds[row][col] = value;
	}

	Stone* _s[32];

	bool _bRedTurn;

	Vec2 Plane2GL(int row, int col);
	bool GL2Plane(int& row, int&col, Vec2 gl);

	// 棋盘格子的直径
	int _d;
	Vec2 _off;

	void trySelectStone(Touch*);
	void tryMoveStone(Touch*);
	int _selectId;
	Sprite* _selectSprite;

	int getStoneId(int row, int col);

	bool canMove(int id, int row, int col, int killid);

	bool canMoveJiang(int id, int row, int col, int killid);
	bool canMoveShi(int id, int row, int col, int killid);
	bool canMoveXiang(int id, int row, int col, int killid);
	bool canMoveChe(int id, int row, int col, int killid);
	bool canMoveMa(int id, int row, int col, int killid);
	bool canMovePao(int id, int row, int col, int killid);
	bool canMoveBing(int id, int row, int col, int killid);

	int getStoneCount(int row1, int col1, int row2, int col2);

	void ComputerMove(int level);
	int calcScore();
	void fakeMove(Step*);
	void fakeBack(Step*);
	void Move(Step* step);

	int getMinScore(int level, int curBest);
	int getMaxScore(int level, int curBest);
};

#endif

