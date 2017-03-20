/*
          "                  ""#
  m mm  mmm     mmmm   mmm     #
  #"  "   #    #" "#  #"  #    #
  #       #    #   #  #""""    #
  #     mm#mm  "#m"#  "#mm"    "mm
                m  #
                 ""
*/
//Yohan MR, 2015
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <string>

using namespace std;

//float to string converter
template <typename T>
string to_string(T value)
{
    ostringstream oss;
    oss << value;
    return oss.str();
}

const int numberOfLevels = 9;
int powerupCount;
int blockCount;
float gravity = 400.f;

bool crashed = false;
bool levelComplete = false;
int distanceCovered;
int speedValue = 25;

int levelNum;
string level = "data/level";
string levelNumber;
string backgroundFile;

class objectAttirbutes //class defined to be inherited by other classes
{
	protected:
		struct rectangleData
		{
			int height;
			int width;
			int posY;
			int posX;
		};
		struct rectangleColumn
		{
			rectangleData top;
			rectangleData bottom;
		};
		struct powerupData
		{
			int radius;
			int posY;
			int posX;
			int type;
		};
		struct shape
		{
			sf::RectangleShape shapeTop;
			sf::RectangleShape shapeBottom;
		};
};

class blocks:private objectAttirbutes
{
	private:
	public:
		rectangleColumn value;
		static int offset;
		blocks()
		{
		}
		~blocks()
		{
		}
		shape drawThis;
		static void getBlockCount()
		{
			string file = level + levelNumber + ".data";
			ifstream input(file.c_str(), ios::binary);
			input >> blockCount;
			input.close();
		}
		static void fileRead(blocks *blockList)
		{
			int i;
			string file = level + levelNumber + ".map";
			ifstream input(file.c_str(), ios::binary);
			for(i = 0; i < blockCount; i++)
			{
				input.read((char*)&blockList[i].value, sizeof(blockList[i].value));
			}
			input.close();
		}
		void setValues(int x)
		{
			if(levelNumber == "0" || levelNumber == "1" || levelNumber == "3" || levelNumber == "4" || levelNumber == "9") //set for block colors
			{
				drawThis.shapeTop.setFillColor(sf::Color(128, 0, 255));
				drawThis.shapeBottom.setFillColor(sf::Color(128, 0, 255));
				drawThis.shapeTop.setOutlineColor(sf::Color::Black);
				drawThis.shapeBottom.setOutlineColor(sf::Color::Black);
			}
			else if(levelNumber == "2")
			{
				drawThis.shapeTop.setFillColor(sf::Color(255, 0, 30));
				drawThis.shapeBottom.setFillColor(sf::Color(255, 0, 30));
				drawThis.shapeTop.setOutlineColor(sf::Color::Black);
				drawThis.shapeBottom.setOutlineColor(sf::Color::Black);
			}
			else
			{
				drawThis.shapeTop.setFillColor(sf::Color(30, 150, 30));
				drawThis.shapeBottom.setFillColor(sf::Color(30, 150, 30));
				drawThis.shapeTop.setOutlineColor(sf::Color(30, 30, 30));
				drawThis.shapeBottom.setOutlineColor(sf::Color(30, 30, 30));
			}

			drawThis.shapeTop.setOutlineThickness(-7.5);
			drawThis.shapeBottom.setOutlineThickness(-7.5);
			value.top.posX = x;
			value.bottom.posX = x;
			drawThis.shapeTop.setSize(sf::Vector2f(value.top.width, value.top.height));
			drawThis.shapeBottom.setSize(sf::Vector2f(value.bottom.width, value.bottom.height));
			drawThis.shapeTop.setPosition(value.top.posX, value.top.posY);
			drawThis.shapeBottom.setPosition(value.bottom.posX, value.bottom.posY);
		}
		static void moveBlocks(blocks *blockList)
		{
			offset -= speedValue;
			for(int i = 0; i < blockCount; i++)
			{
				blockList[i].setValues(i * blockList[i].value.top.width + offset);
			}
			distanceCovered += speedValue;
		}
		static void drawBlocks(blocks *blockList, sf::RenderWindow &window)
		{
			int i;
			for(i = 0; i < blockCount; i++)
			{
				window.draw(blockList[i].drawThis.shapeTop);
				window.draw(blockList[i].drawThis.shapeBottom);
			}
		}
};

int blocks::offset = 0; //initialises position for where blocks will start moving

class powerups:private objectAttirbutes
{
	private:
	public:
		int type;
		sf::Texture design;
		powerupData value;
		sf::CircleShape body;
		powerups()
		{
		}
		~powerups()
		{
		}
		static void fileReadPowerups(powerups *powerupsList)
		{
			string file = level + levelNumber + ".pow";
			ifstream input(file.c_str(), ios::binary);
			int i;
			for(i = 0; i < powerupCount; i++)
			{
                input.read((char*)&powerupsList[i].value, sizeof(powerupsList[i].value));
			}
			input.close();
		}
		void setValues()
		{
			body.setRadius(value.radius);
			body.setPosition(value.posX, value.posY);
			body.setFillColor(sf::Color::Blue);
		}
		void drawPowerup(sf::RenderWindow &window)
		{
			window.draw(body);
		}
		static void getPowerupCount()
		{
			string file = level + levelNumber + ".data";
			ifstream input(file.c_str(), ios::binary);
            input >> powerupCount;
            input >> powerupCount;
            input.close();
        }
        void movePowerup()
		{
			value.posX -= speedValue;
			body.setPosition(value.posX, value.posY);
		}
};

class fly
{
	private:
		struct posVector
		{
			int x;
			int y;
		};
		struct velocityVector
		{
			float y;
		};
		velocityVector velocity;
		posVector position;
		sf::Texture design;
		int type;
	public:
		sf::Sprite body;
		fly()
		{
			position.x = 0;
			position.y = 0;
		}
		fly(int a, int b, const char *picture, blocks *blockList, int type)
		{
			position.x = a;
			position.y = (blockList[1].value.top.posY + blockList[1].value.bottom.posY) / 2;
			if(!design.loadFromFile(picture));
			if(type == 0) //for speedVal of Airplane
			{
				speedValue = 25;
			}
			else if(type == 1) //for speedVal of Twitter Bird
			{
				speedValue = 15;
			}
			else if(type == 2) //for setting values of Helium Balloon
			{
				speedValue = 10;
				gravity = -200.0f; //controls are inverted!
			}
			body.setTexture(design);
			body.setColor(sf::Color(255, 255, 255, 255));
		}
		~fly()
		{
		}
		void setValues(int a, int b, const char *picture, blocks *blockList, int value)
		{
			position.x = a;
			position.y = (blockList[1].value.top.posY + blockList[1].value.bottom.posY) / 2;
			if(!design.loadFromFile(picture))
			{
				cout << "\nI/O Error";
			}
			body.setTexture(design);
			type = value;
			if(type == 0)
			{
				speedValue = 25;
			}
			else if(type == 1)
			{
				speedValue = 15;
			}
			else if(type == 2)
			{
				speedValue = 10;
				gravity = -200.0f;
			}
			body.setColor(sf::Color(255, 255, 255, 255));
		}
		void movement(float dt, int flag)
		{
			if(type == 0 || type == 1)
			{
				if(flag == 1)
				{
					velocity.y -= 600.f;
				}
				if(velocity.y < gravity)
				{
					velocity.y += 10.f;
				}
				else if(velocity.y > gravity)
				{
					velocity.y = gravity;
				}
			}
			if(type == 2) //for helium balloon
			{
				if(flag == 1)
				{
					velocity.y += 500.f;
				}
				if(velocity.y < gravity)
				{
					velocity.y = gravity;
				}
				else if(velocity.y > gravity)
				{
					velocity.y -= 10.f;
				}
			}
			position.y += velocity.y * dt;
			body.setPosition(position.x, position.y);
		}
		void jump(float dt)
		{
			movement(dt, 1);
		}
		void collisionCheck(blocks *blockList)
		{
			for(int i = 0; i < blockCount; i++)
			{
				if(body.getGlobalBounds().intersects(blockList[i].drawThis.shapeTop.getGlobalBounds()) || body.getGlobalBounds().intersects(blockList[i].drawThis.shapeBottom.getGlobalBounds())) //condition checks whether flying object collides with bottom or top block
				{
					if(!design.loadFromFile("sprites/explosion.png"));
					body.setTexture(design);
					crashed = true;
				}
			}
		}
		void collisionCheck(powerups *powerupsList, bool &doubleScore)
		{
			for(int i = 0; i < powerupCount; i++)
			{
				if(body.getGlobalBounds().intersects(powerupsList[i].body.getGlobalBounds()))
				{
					if(powerupsList[i].type == 0)
					{
						doubleScore = true;
					}
				}
			}
		}
		static int typeNumber(string imageFile)
		{
			if(imageFile == "sprites/airplane.png")
			{
				return 0;
			}
			if(imageFile == "sprites/bird.png")
			{
				return 1;
			}
			if(imageFile == "sprites/balloon.png")
			{
				return 2;
			}
			return -1;
		}
		int flyType()
		{
			return type;
		}
};

struct node //for linked list
{
	int value;
	node *next;
}*top, *temp, *save;

class scoresList
{
	private:
	public:
		scoresList()
		{
		}
		~scoresList()
		{
		}
		void writeScores(int score)
		{
			ofstream output("data/scores.txt", ios::binary|ios::app);
			output.seekp(0, ios::end);
			output << score << "\n";
			output.close();
		}
		void getNodes()
		{
			ifstream input("data/scores.txt", ios::binary);
			int x;
			while(input.eof() == 0)
			{
				input >> x;
				insertNode(createNode(x));
			}
			input.close();
		}
		node *createNode(int x)
		{
			temp = new node;
			temp -> value = x;
			temp -> next = NULL;
			return temp;
		}
		void insertNode(node *newNode)
		{
			if(top == NULL)
			{
				top = newNode;
			}
			else
			{
				save = top;
				top = newNode;
				newNode -> next = save;
			}
		}
		void displayAllNodes(node *newNode)
		{
			int i = 0;
			int max = 0;
			while(newNode != NULL && i <= 10)
			{
				cout << newNode -> value << "\n";
				if(newNode -> value > max)
				{
					max = newNode -> value;
				}
				newNode = newNode -> next;
				i++;
			}
			cout << "Highest score over last 10 plays: " << max << "\n";
		}
};

int checkLevelComplete(int distanceCovered, bool &gamePause)
{
	if(distanceCovered >= blockCount * 100)
	{
		levelComplete = true;
		gamePause = true;
		levelNumber = to_string(++levelNum);
		return 1;
	}
	return 0;
}

void gameTitle()
{
	cout << "	       *                  **#" << "\n";
	cout << "	m mm  mmm     mmmm   mmm     #" << "\n";
	cout <<	"	#*  *   #    #* *#  #*  #    #" << "\n";
	cout << "	#       #    #   #  #****    #" << "\n";
	cout << "	#     mm#mm  *#m*#  *#mm*    *mm" << "\n";
	cout << "		      m  #               " << "\n";
	cout << "		       **        			" << "\n";
	cout << "\nRIGEL! A cross platform videogame";
        cout << "\nProgrammed by Yohan";
	cout << "\n--------------------------------------------\n";
}

void deleteScores()
{
	ofstream output("data/scores.txt", ios::trunc); //ios::trunc will overwrite file
	output.close();
}

int menu(bool &menuCheck, string &imageFile, scoresList listOfScores)
{
	int choice;
	int levelChoice;
	char option;
	cout << "\nSelect option:\n\n";
	cout << "1 - PLAY!\n";
	cout << "2 - Choose aircraft\n";
	cout << "3 - Scores\n";
	cout << "4 - Clear score history\n";
	cout << "5 - Controls\n";
	cout << "6 - Select Level\n";
	cout << "7 - About\n";
	cout << "8 - Exit\n";
	cin >> choice;
	switch(choice)
	{
		case 1:
			menuCheck = true;
			break;
		case 2:
			cout << "a - Airplane (default): The Airplane is faster than the other two and provides a decent multiplier.\n";
			cout << "b - Twitter Bird: For new players; the Twitter Bird is slower than an Airplane.\n";
			cout << "c - Helium Balloon: For experienced players; the Helium Balloon inverts controls!\n";
			cin >> option;
			switch(option)
			{
				case 'a':
					imageFile = "sprites/airplane";
					break;
				case 'b':
					imageFile = "sprites/bird";
					break;
				case 'c':
					imageFile = "sprites/balloon";
					break;
			}
			break;
		case 3:
			cout << "Previous 10 scores:\n";
			listOfScores.getNodes();
			listOfScores.displayAllNodes(top);
			top = NULL;
			break;
		case 4:
			deleteScores();
			cout << "Previous scores deleted.\n";
			break;
		case 5:
			cout << "Press <SPACE> to jump, P to pause.\n";
			break;
		case 6:
			cout << "Select level (0 -> 9)\n";
			cin >> levelChoice;
			if(levelChoice >= 0 && levelChoice < 10)
			{
				levelNum = levelChoice;
				levelNumber = to_string(levelChoice);
				cout << "\nLevel " << levelChoice << " selected\n";
			}
			else
			{
				cout << "No such level! Please try again!\n";
			}
			break;
		case 7:
			gameTitle();
			break;
		case 8:
			return 1;
	}
	return 0;
}

bool checkFiles() //checks if all needed files are present in the data folder
{
	string file;
	string levelString = "data/level";
	string backgroundString = "images/background";
	int i;
	ifstream input;
	ofstream output;
	for(i = 0; i < 10; i++)
	{
		file = levelString + to_string(i) + ".data";
		input.open(file.c_str());
		if(input.good() == false)
		{
			input.close();
			return false;
		}
		input.close();
		file = levelString + to_string(i) + ".map";
		input.open(file.c_str());
		if(input.good() == false)
		{
			input.close();
			return false;
		}
		input.close();
		file = levelString + to_string(i) + ".pow";
		input.open(file.c_str());
		if(input.good() == false)
		{
			input.close();
			return false;
		}
		input.close();
	}
	file = "data/font.ttf";
	input.open(file.c_str());
	if(input.good() == false)
	{
		input.close();
		return false;
	}
	file = "data/scores.txt";
	input.open(file.c_str());
	if(input.good() == false)
	{
		input.close();
		output.open(file.c_str(), ios::binary|ios::trunc);
		output.close();
	}
	return true;
}
		
int main(int argc, char **argv)
{
	int score;
	sf::Clock frametime;
	sf::Texture img;
	sf::Sprite background;

	string scoreString;
	string image;

	scoresList listOfScores;

	bool gamePause = true;
	bool doubleScore = false;
	bool inPlay = true;
	bool menuCheck = false;
	bool writtenToFile = false;

	int doubleScoreCount = 0;
	int i;
	score = 0;
	gamePause = true;
	doubleScore = false;
	doubleScoreCount = 0;

	//SFML requires special objects for fonts, windows, sprites, colours, etc.
	sf::Event event;
	sf::Font guiFont;
	sf::Text scoreText, scoreValue;
	sf::Text levelCompleteText;
	sf::Text crashedText;
	sf::Text doubleScoreText;
	sf::Text multiplier;
	sf::Text multiplierValue;
	sf::Text gamePausedText;
	sf::Color fontColor = sf::Color::White;
	fly flyBody;
	string imageFile = "sprites/airplane";
	gameTitle();
	levelNumber = "0";
	levelNum = 0;
	
	if(checkFiles() == false) //terminates program with error code if files are missing
	{
		cout << "\nI/O Error. It looks like there are some missing files. Check whether all the folders are present. Running the levelRNG program again might fix this.";
		return 1;
	}
	
	while(menuCheck == false)
	{
		if(menu(menuCheck, imageFile, listOfScores) == 1)
		{
			return 0;
		}
	}

	imageFile += ".png";

	blocks *blockList;
	powerups *powerupsList;
	//no more variable declarations after this line

	sf::RenderWindow window(sf::VideoMode(1280, 720), "rigel"); //creates window for SFML to draw in
	window.setFramerateLimit(60);

	while(inPlay == true) //main thread for all program logic
	{
		if(levelNumber == "1" || levelNumber == "3")
		{
			fontColor = sf::Color::Black; //for better contrast
		}
		else
		{
			fontColor = sf::Color::White;
		}

		backgroundFile = "images/background" + levelNumber + ".jpg";
		if(!img.loadFromFile(backgroundFile));

		background.setTexture(img);

		blocks::getBlockCount();
		blockList = new blocks[blockCount]; //dynamically allocated to avoid stack overflows
		blocks::fileRead(blockList);
		blocks::moveBlocks(blockList);

		flyBody.setValues(190, 300, imageFile.c_str(), blockList, fly::typeNumber(imageFile));
		guiFont.loadFromFile("data/font.ttf");

		scoreText.setFont(guiFont);
		scoreText.setCharacterSize(40);
		scoreText.setColor(fontColor);
		scoreText.setString("SCORE: ");
		scoreText.setPosition(0, 600);
		scoreValue.setFont(guiFont);
		scoreValue.setCharacterSize(40);
		scoreValue.setColor(fontColor);
		scoreValue.setPosition(150, 600);

		levelCompleteText.setFont(guiFont);
		levelCompleteText.setCharacterSize(70);
		levelCompleteText.setColor(sf::Color::White);
		levelCompleteText.setString("LEVEL COMPLETE!");
		levelCompleteText.setPosition(300, 200);

		crashedText.setFont(guiFont);
		crashedText.setCharacterSize(70);
		crashedText.setColor(sf::Color::Red);
		crashedText.setString("CRASHED!");
		crashedText.setPosition(300, 200);

		doubleScoreText.setFont(guiFont);
		doubleScoreText.setCharacterSize(40);
		doubleScoreText.setColor(sf::Color::Red);
		doubleScoreText.setString("DOUBLE SCORE!");
		doubleScoreText.setPosition(450, 600);

		multiplier.setFont(guiFont);
		multiplier.setCharacterSize(40);
		multiplier.setColor(sf::Color::White);
		multiplier.setString("MULTIPLIER: ");
		multiplier.setPosition(0, 550);

		multiplierValue.setFont(guiFont);
		multiplierValue.setCharacterSize(40);
		multiplierValue.setColor(sf::Color::White);
		multiplierValue.setString(to_string(speedValue / 10.0));
		multiplierValue.setPosition(270, 550);

		gamePausedText.setFont(guiFont);
		gamePausedText.setCharacterSize(30);
		gamePausedText.setColor(sf::Color::White);
		gamePausedText.setString("GAME PAUSED");
		gamePausedText.setPosition(270, 400);

		powerups::getPowerupCount();
		powerupsList = new powerups[powerupCount]; //dynamically allocated to avoid stack overflows
		powerups::fileReadPowerups(powerupsList);

		for(i = 0; i < powerupCount; i++)
		{
			powerupsList[i].setValues();
		}

		while (window.isOpen())
		{
			float dt = frametime.restart().asSeconds();
			while(window.pollEvent(event))
			{
				if(event.type == sf::Event::Closed)
				{
					window.close();
				}
				if(event.type == sf::Event::KeyPressed)
				{
					if(event.key.code == sf::Keyboard::Space && crashed == false)
					{
						flyBody.jump(dt);
					}
					if(event.key.code == sf::Keyboard::P && gamePause == false && crashed == false)
					{
						window.draw(gamePausedText);
						gamePause = true;
					}
					else
					{
						gamePause = false;
					}
				}
				if(event.type == sf::Event::KeyPressed && crashed == true)
				{
					return 0;
				}
			}
			if(crashed == true)
			{
				scoreString = to_string(score);
				scoreValue.setString(scoreString);
				window.draw(crashedText);
				window.draw(scoreText);
				window.draw(scoreValue);
				window.display();
				gamePause = true;
				if(writtenToFile == false)
				{
					cout << "Score: " << score << "\n";
					listOfScores.writeScores(score);
					writtenToFile = true;
				}
			}
			if(checkLevelComplete(distanceCovered, gamePause) == 1)
			{
				if(flyBody.flyType() == 2)
				{
					score -= 5;
					score *= 4;
				}
				score *= speedValue / 10;
				scoreString = to_string(score);
				scoreValue.setString(scoreString);
				window.clear(sf::Color::Black);
				window.draw(levelCompleteText);
				window.draw(multiplier);
				window.draw(multiplierValue);
				window.draw(scoreText);
				window.draw(scoreValue);
				window.display();
				break;
			}
			if(gamePause == false)
			{
				flyBody.movement(dt, 0);
				window.clear(sf::Color::Black);
				flyBody.collisionCheck(blockList);
				flyBody.collisionCheck(powerupsList, doubleScore);
				window.draw(background);
				for(i = 0; i < powerupCount; i++)
				{
					powerupsList[i].movePowerup();
					powerupsList[i].drawPowerup(window);
				}
				if(crashed == false)
				{
					blocks::moveBlocks(blockList);
				}
				blocks::drawBlocks(blockList, window);
				window.draw(flyBody.body);
				if(doubleScore == true && doubleScoreCount < 30)
				{
					score += speedValue * 2;
					doubleScoreCount++;
					window.draw(doubleScoreText);
				}
				else if(doubleScoreCount == 30)
				{
					score += speedValue;
					doubleScore = false;
					doubleScoreCount = 0;
				}
				else
				{
					doubleScore = false;
					score += speedValue;
				}
				scoreString = to_string(score); //SFML string requires string arguement
				scoreValue.setString(scoreString);
				window.draw(scoreText);
				window.draw(scoreValue);
				window.display();
			}
		}
		if(levelNum > numberOfLevels) //when all levels are finished
		{
			listOfScores.writeScores(score);
			cout << "Score: " << score << "\n";
			cout << "\nCongratulations on finishing! :)";
			return 0;
		}
		//reinitialise variables for next level
		distanceCovered = 0;
		blocks::offset = 0;
	}
	delete []blockList;
	delete []powerupsList;
	return 0; //returns 0
}
