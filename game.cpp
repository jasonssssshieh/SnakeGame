// snake game
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <process.h>
#include <windows.h>
#include <ctype.h>
#include <conio.h>
#include <vector>
#include <cstdlib>
using namespace std;

int Left = 0;
int Right = 1;
int Up = 2;
int Down = 3;
int EXIT = -1;
int NOTHING = 0;
int APPLE = 1;
int SNAKE_LENGTH = 1;
int WIDTH = 100;
int HEIGHT = 50;
int WALL = -2;
int SNAKE = -1;
vector<vector<int>> dir = {{0,-1},{0,1},{-1,0},{0,1}};
int input = RIGHT;
int item = NOTHING;


struct Coordinate{
	int x;
	int y;
};

void gotoxy(int column, int row)
{
	HANDLE hStdOut;
	COORD coord;
 
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;
 
	coord.X = column;
	coord.Y = row;
	SetConsoleCursorPosition(hStdOut, coord);
}
 
void clearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };
 
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;
 
	/* Get the number of cells and cell attributes in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;
 
	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,		//handle to console screen buffer
		(TCHAR) ' ',	//character to write to the buffer
		cellCount,		//number of cells to write to
		homeCoords,		//coordinates of first cell
		&count			//receives the number of characters written
		)) return;
 
	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,			//handle to console screen buffer
		csbi.wAttributes,	//Character attributes to use
		cellCount,			//Number of cells to set attribute
		homeCoords,			//Coordinate of first cell
		&count				//receives the number of characters written
		)) return;
 
	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

//check whether two directions are opposite
int oppositeDirection(int input1, int input2){
	if((input1 == LEFT && input2 == RIGHT) || (input2 == LEFT && input1 == RIGHT)){
		return 1;
	}
	if((input1 == DOWN && input2 == UP)||(input2 == DOWN && input1 == UP)){
		return 1;
	}
	return 0;
}

class snake{
public:
	snake(){}
	int initGround(){
		for(int i = 0; i < MAX; ++i){
			for(int j = 0; j < MAX; ++j){
				ground[i][j] = 0;
			}
		}
		for(int i = 0; i <= WIDTH + 1; i++){
			ground[0][i] = WALL;
			ground[HEIGHT+1][i] = WALL;
		}
		for(int j = 0; j <= HEIGHT + 1; ++j){
			ground[j][0] = WALL;
			ground[i][WIDTH + 1] = WALL;
		}
	}

	void initSnake(){
		len = SNAKE_LENGTH;
		//at the beginning of the game, our snake is set in the middle of the ground.
		body[0].x = WIDTH/2;
		body[0].y = HEIGHT/2;
		direction = input;// direction
		foodCounter = 0;
		//here we initialize our snake locations, as we have the direction and length of snake, so from the snake head
		//we could get the position of all remaining parts of the snake
		for(int i = 1; i < len; ++i){
			body[i].x = body[i-1].x - dir[direction][0];
			body[i].y = body[i-1].y - dir[direction][1];
		}
		for(int i = 0; i < len; ++i){
			ground[body[i].x][body[i].y] = SNAKE;//current these positions are occpuied by the snake
		}
	}

	void updateSnake(int delay){
		Coordinate prev[WIDTH*HEIGHT];//previous locations of the snake
		for(int i = 0; i < len; ++i){
			prev[i].x = body[i].x;
			prev[i].y = body[i].y;
		}
		if(input != EXIT && !oppositeDirection(direction, input)){
			direction = input;
		}
		body[0].x = prev[0].x + dir[direction][0];
		body[0].y = prev[0].y + dir[direction][1];

		if(ground[body[0].x][body[0].y] < NOTHING){
			//we hit the wall or we hit the snake body
			item = -1;
			return;
		}else if(ground[body[0].x][body[0].y] == APPLE){
			len++;
			item = APPLE;
		}else{
			//its just empty (NOTHING) place.
			//so we can just simple remove last node.
			ground[body[len-1].y][body[len-1].x] = NOTHING;
			item = NOTHING;
			gotoxy(body[len-1].x, body[len-1].y);
			cout<<" ";
		}
		for(int i = 1; i < len; ++i){
			body[i].x = prev[i-1].x;
			body[i].y = prev[i-1].y;
		}

		gotoxy(body[1].x, body[1].y);
		cout<<"+";
		goto(body[0].x, body[0].y);
		cout<<"<";
		for(int i = 0; i < len; ++i){
			ground[body[i].x][body[i].y] = SNAKE;
		}
		Sleep(delay);
		return;
	}

	void updateApple(){
		//randomly put the apple in the ground;
		int x;
		int y;
		do{
			x = rand() % WIDTH + 1;
			y = rand() % HEIGHT + 1;
		}while(ground[x][y] != NOTHING);
		ground[x][y] = FOOD;
		foodCounter++;
		gotoxy(x, y);
		cout<<"\u2022";
	}

	void firstDraw(){
		clearScreen();
		for(int i = 0; i <= HEIGHT+1; ++i){
			for(int j = 0; j <= WIDTH + 1; ++j){
				switch(ground[i][j]){
					case NOTHING:
						cout<<" "; break;
					case WALL:
						if((i == 0 && j == 0) 
							|| (i == 0 && j == WIDTH + 1) 
							|| (i == HEIGHT + 1 && j == 0)
							|| (i == HEIGHT + 1 && j == WIDTH + 1)){
							cout<<"+";
						}else{
							if(j == 0 || j == WIDTH + 1){
								cout<<"|";
							}else{
								cout<<"-";
							}
						}
						break;
					case SNAKE:
						if(i == body[0].y && j == body[0].x)
							cout<<"<";
						else
							cout<<"+";
						break;
					default:
						cout<< "\u2022";
				}
			}
			cout<<endl;
		}
	}

	int getFoodCounter(){
		return foodCounter;
	}

	void userInput(void* id){
		do{
			int c = _getch();
			switch(c){
				case 'W': case 'w': input = Up; break;
				case 'S': case 's': input = Down; break;
				case 'D': case 'd': input = Right; break;
				case 'A': case 'a': input = Left; break;
				case 27: input = EXIT; break;
			}
		}while(input != EXIT && item >= 0);

		_endthread();
		return;
	}
private:
	int len;
	Coordinate body[WIDTH*HEIGHT];
	int direction;
	int ground[MAX][MAX];
	int foodCounter;//count how many foods are currently been eaten in the game
};

int main(){
	int delay = 50;
	srand(time(NULL));
	snake shieh;
	shieh.initGround();
	shieh.initSnake();
	shieh.updateApple();
	shieh.firstDraw();
	_beginthread(userInput, 0, (void*) 0);

	do{
		shieh.updateSnake(delay);
		if(item == APPLE){
			shieh.updateApple();
		}
	}while(item >= 0 && input != EXIT);

	gotoxy(WIDTH / 2 - 5, HEIGHT / 2 - 2);
	cout << "GAME OVER";
	gotoxy(WIDTH / 2 - 8, HEIGHT / 2 + 2);
	cout << "Your final score is " << nagini.getFoodCounter() - 1 << "!" << endl;
	gotoxy(WIDTH / 2, HEIGHT / 2);
 
	_getch();
	return 0;
}
