#include <ncurses.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>

using namespace std;

using FieldType = vector<vector<uint8_t>>;



// globals
bool ShouldExit = false;
FieldType Field;

void clearField(FieldType &field)
{
	move(0,0);
	for (int i = 0; i < field.size(); i++)
	{
		for (int j = 0; j < field[i].size(); j++)
		{
			addch(' ');
		}
	}
}

void userInputHandler(){
	getch();
	ShouldExit = true;
}


struct RateLimiter{
	
	RateLimiter(double hz){
		int ms = (1 / hz) * 1000;
		auto dur = chrono::milliseconds(ms);
		stopTime = chrono::system_clock::now() + dur;
	}
	
	
	~RateLimiter(){
		this_thread::sleep_until(stopTime);
	}
	
	std::chrono::system_clock::time_point stopTime;
	
};

class Letter{
public:
	virtual void draw(FieldType& field, int atRow, int atCol) = 0;
};

class TheLetterD : public Letter{
public:
	TheLetterD(){
		if(ifstream letterD_file("/home/sholmp/repos/ncurses_diana/ascii_art/diana_D"); letterD_file){
			stringstream ss;
			ss << letterD_file.rdbuf();
			
			letter = ss.str();
		} else{
			cout << "Failed to load the 'Letter d' file" << endl;
		}
	}

	void draw(FieldType& field, int x, int y){
		move(y, x);
		
		for (auto ch : letter){
			addch(ch);
			if(ch == '\n'){
				move(++y, x);
			}	
		}

	}
	
	virtual ~TheLetterD(){}
	
private:
	string letter;
};




int main()
{
	initscr();

	int rows, cols;
	getmaxyx(stdscr, rows, cols);

	Field.resize(rows);
	for (auto &row : Field)
	{
		row.resize(cols);
	}

	TheLetterD D;
	thread userInputThread(userInputHandler);	

	int xPos = 0;
	while(!ShouldExit){
		RateLimiter rl(1);
		clearField(Field);
		
		D.draw(Field, xPos++ % cols, 0);
		// D.draw(Field, 0, 0);
		refresh();
	}
	
	
	

	endwin();

	userInputThread.join();

	return 0;
}
