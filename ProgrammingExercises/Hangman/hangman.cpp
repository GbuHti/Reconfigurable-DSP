/*这是一个猜字游戏
 * 来源：C++ Primer Plus(第6版)P663
 * Yu Zhenbo  
 * 2020-02-25 14:30:13 
 */
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cctype>

using std::string;

const int NUM = 26;

const string wordlist[NUM] = {"apiary","beetle","cereal",
		"danger","ensign","florid","garage","health","insult",
		"jackal","keeper","loaner","initiate","manage","onset",
		"playground","summer","remote","understand","train","usefle",
		"valid", "because", "propse","suprise","keyboard"};

int main()
{
	using std::cout;
	using std::cin;
	using std::tolower;
	using std::endl;
	std::srand(std::time(0));
	char play;
	cout << "你愿意玩个游戏吗？<y/n> ";
	cin >> play;
	play = tolower(play);
	while (play == 'y')
	{
		string target = wordlist[std::rand()%NUM];
		int length = target.length();
		string attempt(length,'-');
		string badchars;
		int guesses = 6;
		cout << "Guess my secret word. It has " << length
			<< " letters, and you guess\n"
			<< "one letter at a time. You get " << guesses
			<< "wrong guesses.\n";
		cout << "Your word: " << attempt << endl;
		while(guesses > 0 && attempt != target)
		{
			char letter;
			cout << "Guess a letter: ";
			cin >> letter;
			if (badchars.find(letter) != string::npos || attempt.find(letter) != string::npos)
			{
				cout << "You already guessed that. Try again.\n";
				continue;	
			}	
			int loc = target.find(letter);
			if(loc == string::npos)
			{
				cout << "Oh, bad guess!\n";
				--guesses;
				badchars += letter; //add to string	
			}
			else
			{
				cout << "Good guess!\n";	
				attempt[loc] = letter;
				//check if letter appears again
				loc = target.find(letter, loc+1);
				while(loc != string::npos)
				{
					attempt[loc] = letter;
					loc = target.find(letter,loc+1);	
				}
			}
			cout << "Your word: " << attempt << endl;
			if (attempt != target )
			{
				if(badchars.length() > 0)
					cout << "Bad choices: " << badchars << endl;
				cout << guesses << " bad guesses left\n";	
			}
			cout << endl;
		}
		if(guesses > 0)
			cout << "That's right\n";
		else
			cout << "Sorry, the word is " << target << ".\n";
		cout << "再玩一次？<y/n> ";
		cin >> play;
		play = tolower(play);
	}
	cout << "再见\n";
	return 0;
}

