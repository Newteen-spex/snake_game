#include <iostream>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <set>
#include <windows.h>
#include <conio.h>
#include <time.h>
#define HEIGHT 15
#define WIDTH 35
#define INIT_FLUSHRATE 200
#define INIT_SCORE 0
#define BODY '@'
#define EMPTY ' '
#define FOOD '*'
#define EDGE '#'
#define RANKFILE "rank.txt"
using namespace std;
bool keyIsput = false;

enum class Orient {UP, DOWN, LEFT, RIGHT};

typedef struct node {
	int row;
	int col;
	Orient direction;
	struct node* next;
} Node;

typedef struct {
	int frow;
	int fcol;
} Food;

bool FoodInSnake(const int& r, const int& c, Node* q) {
	while (q != NULL) {
		if (q->row == r && q->col == c)
			return true;
		q = q->next;
	}
	return false;
}

void printRank(){
	ifstream fin(RANKFILE);
	if(!fin){
		cerr << RANKFILE << " does not exit or open file error!!";
		exit(1);
	}
	if(fin.peek()==ifstream::traits_type::eof()){cout << "file is empty";exit(1);}
	for(int i=0; i<5; ++i){
		int tmp;
		if(fin.eof())break;
		fin >> tmp;
		cout << "No." << i+1 << ": " << tmp << '\n';
	}
}

class Snake {
private:
	Node* snakeHead;

public:
	static Orient tmp_1, tmp_2;
	Snake(int r, int c, Orient ori) {
		snakeHead = new Node;
		snakeHead->row = r;
		snakeHead->col = c;
		snakeHead->direction = ori;
		snakeHead->next = NULL;
	};

	Node* getHead() {
		return snakeHead;
	}

	void setOrient(Orient ori) {
		tmp_1 = snakeHead->direction;
		snakeHead->direction = ori;
	}

	void move() {
		Node* p = snakeHead;
		while (p != NULL) {
			if (p == snakeHead) {
				if(keyIsput==false)
					tmp_1 = p->direction;
			}
			else if (p != snakeHead) {
				tmp_2 = p->direction;
				p->direction = tmp_1;
				tmp_1 = tmp_2;
			}
			switch (p->direction) {
			case Orient::UP:(p->row)--; break;
			case Orient::DOWN:(p->row)++; break;
			case Orient::LEFT:(p->col)--; break;
			case Orient::RIGHT:(p->col)++; break;
			default:break;
			}
			p = p->next;
		}
		keyIsput = false;
	};

	void longer(int row, int col, Orient direction) {
		Node* newbody = new Node;
		newbody->row = row;
		newbody->col = col;
		newbody->direction = direction;
		newbody->next = snakeHead;
		snakeHead = newbody;
	}

	virtual ~Snake() {
		Node *p = snakeHead, *q=NULL;
		while (p != NULL) {
			q = p->next;
			delete p;
			p = q;
		}
	};
};

class Board {
private:
	Snake* s;
	int playground[HEIGHT][WIDTH] = {0};
	Food* food;
	bool foodIsEaten;
	bool gameOver;
	int score;
	int flushRate;

public:
	Board(int sr=HEIGHT/2, int sc=WIDTH/2, Orient sori=Orient::RIGHT):
	gameOver(false),foodIsEaten(false),score(INIT_SCORE),flushRate(INIT_FLUSHRATE){
		s = new Snake(sr, sc, sori);
		playground[sr][sc] = 1;
		food = new Food;
		while ((food->frow = rand() % HEIGHT) == sr);
		while ((food->fcol = rand() % WIDTH) == sc);
		playground[food->frow][food->fcol] = 2;
	};

	void refresh() {
		s->move();
		Node* shead = s->getHead();
		if((gameOver = isCollision(shead))==true)return;
		if (shead->row == food->frow && shead->col == food->fcol) {
			foodIsEaten = true;
			switch (shead->direction)
			{
			case Orient::UP: s->longer(shead->row - 1, shead->col, shead->direction); break;
			case Orient::DOWN: s->longer(shead->row + 1, shead->col, shead->direction); break;
			case Orient::LEFT: s->longer(shead->row, shead->col - 1, shead->direction); break;
			case Orient::RIGHT: s->longer(shead->row, shead->col + 1, shead->direction); break;
			default:
				break;
			}
		}
		if (foodIsEaten)
			refreshFood();
		memset(playground, 0, sizeof(playground));
		Node* p = s->getHead();
		while (p != NULL) {
			playground[p->row][p->col] = 1;
			p = p->next;
		}
		foodIsEaten == false ? playground[food->frow][food->fcol] = 2 : NULL;
	};

	void printBoard() {
		for (int i = 0; i < HEIGHT; ++i) {
			int j;
			for (j = 0; j < WIDTH; ++j)
				playground[i][j] == 1 ? cout << BODY : (playground[i][j]==2? cout << FOOD:cout << EMPTY);
			i==0?cout << EDGE << "  Your score: " << score << '\n':cout << EDGE << '\n';
		}
		for (int k = 0; k < WIDTH; ++k) {
			cout << EDGE;
		}
	}

	void setSnakeOrient(Orient ori) {
		s->setOrient(ori);
	}

	void refreshFood() {
		score++;if(flushRate>50)flushRate-=5;
		Node* p = s->getHead();
		while (FoodInSnake(food->frow = rand() % HEIGHT, food->fcol = rand() % WIDTH, p));
		foodIsEaten = false;
	}

	void keyCheck() {
		if (_kbhit()) {
			keyIsput = true;
			char key = _getch();
			switch (key)
			{
			case 'w':case'W':setSnakeOrient(Orient::UP); break;
			case 'a':case'A':setSnakeOrient(Orient::LEFT); break;
			case 's':case'S':setSnakeOrient(Orient::DOWN); break;
			case 'd':case'D':setSnakeOrient(Orient::RIGHT); break;
			default:
				break;
			}
		}
	}

	bool isCollision(Node*& head){
		if(head->row<0 || head->row>=HEIGHT)
			return true;
		if(head->col<0 || head->col>=WIDTH)
			return true;
		Node* p = head;
		while(p!=NULL){
			if(p!=head && p->row==head->row && p->col==head->col)
				return true;
			p = p->next;
		}
		return false;
	}

	int getFlushRate() const{
		return flushRate;
	}

	bool isGameOver(){
		return gameOver;
	}

	void printEnd(){
		ifstream fin(RANKFILE);
		if(!fin){
			fin.close();
			ofstream fout(RANKFILE);
			fout << score;fout.close();
		}else{
			set<int> r;int tmp;
			r.insert(score);
			while(!fin.eof()){
				fin >> tmp;
				r.insert(tmp);
			}
			fin.close();
			ofstream fout(RANKFILE);
			for(auto i=r.rbegin();i!=r.rend();++i)
				++i==r.rend()?fout << *(--i):fout << *(--i) << '\n';
			fout.close();
		}
		cout << "Game Over!!\n";
		cout << "Your score is " << score << ".\n";
	}

	~Board() {
		delete s;
		delete food;
	}
};

Orient Snake::tmp_1 = Orient::RIGHT, Snake::tmp_2 = Orient::RIGHT;

int main(int argc, char* argv[]) {
	if(argc!=2 || (strcmp(argv[1], "start")!=0 && strcmp(argv[1], "rank")!=0)){
		cerr << "Usage: \"snake start\" to begin game.\n";
		cerr << "       \"snake rank\" to look rank(1-5).";
		return 1;
	}
	if(strcmp(argv[1], "rank")==0){printRank();return 0;}
	srand((unsigned)time(NULL));
	Board* gameboard = new Board();
	while (!gameboard->isGameOver()) {
		gameboard->printBoard();
		Sleep(gameboard->getFlushRate());
		system("cls");
		gameboard->keyCheck();
		gameboard->refresh();
	}
	gameboard->printEnd();
	delete gameboard;
	return 0;
}