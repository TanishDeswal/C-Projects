#include <iostream>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <ctime>
#include <vector>
#include <sstream>

using namespace std;

class HangmanGame {
private:
    int tries;
    int score;
    int score_needed;
    const int points_per_correct_guess;
    const int word_timer_seconds;
    string message;
    string hintMessage;

    void Hang_Man(char state, int score, int time_left) {
        string head_string = "|";
        string stage = "=";
        string lever = "| ";
        if (state == 'f') {
            head_string = " ";
        }
        else if (state == 'h') {
            stage = " ";
            lever = " /";
        }

        cout << "\t\t\t\t___________________" << "                          Score: " << score << endl;
        cout << "\t\t\t\t       " << head_string << "         |                           Time Left: " << time_left << " seconds" << endl;
        cout << "\t\t\t\t       O         |" << endl;
        cout << "\t\t\t\t      /|\\        |" << "\t   " << message << endl;
        cout << "\t\t\t\t       |         |" << "\t   /" << endl;
        cout << "\t\t\t\t      / \\        |        O " << endl;
        cout << "\t\t\t\t  ==" << stage << stage << stage << stage << stage << stage << stage << "==    |       /|\\" << endl;
        cout << "\t\t\t\t  |         |    |   " << lever << "   | " << endl;
        cout << "\t\t\t\t  |         |    |   O   / \\" << endl;
        cout << "\t\t\t\t====================|=|================" << endl;
    }

    int CheckGuess(char guess, const string& real_word, string& Hidden_word) {
        int matches = 0;
        int len = real_word.length();
        for (int i = 0; i < len; i++) {
            if (guess == Hidden_word[i]) {
                return 0;
            }
            if (guess == real_word[i]) {
                Hidden_word[i] = guess;
                matches++;
            }
        }
        return matches;
    }

public:
    HangmanGame()
        : tries(5), score(0), points_per_correct_guess(100),
        word_timer_seconds(120), message("Score needed to be free!") {}

    void color(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }

    void gotoxy(int x, int y) {
        COORD c;
        c.X = x;
        c.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    }

    void select_difficulty() {
        int Set[] = { 7, 7, 7 }; 
        int counter = 1; 
        char key; 

        while (true) {
            system("cls");

            
            gotoxy(50, 10);
            color(Set[0]);
            cout << "Easy (Score 1000)";

            gotoxy(50, 11);
            color(Set[1]);
            cout << "Medium (Score 3000)";

            gotoxy(50, 12);
            color(Set[2]);
            cout << "Hard (Score 5000)";

            
            key = _getch();

            
            if (key == 72 && (counter > 1)) {
                counter--;
            }
            if (key == 80 && (counter < 3)) {
                counter++;
            }
            
            if (key == '\r') {
                system("cls");
                if (counter == 1) {
                    score_needed = 1000;
                }
                else if (counter == 2) {
                    score_needed = 3000;
                }
                else if (counter == 3) {
                    score_needed = 5000;
                }
                break;
            }

            
            Set[0] = Set[1] = Set[2] = 7;
            Set[counter - 1] = 12;
        }
    }


    void Game_Start() {
        select_difficulty();

        char letter;
        string input;
        string word;
        vector<string> words;
        vector<string> hints;

        const int time_penalty_seconds = 10;
        string difficulty_file;

   
        if (score_needed == 1000) {
            difficulty_file = "easy_words_with_hints.txt";
        }
        else if (score_needed == 3000) {
            difficulty_file = "medium_words_with_hints.txt";
        }
        else {
            difficulty_file = "hard_words_with_hints.txt";
        }

        ifstream file(difficulty_file);
        if (!file.is_open()) {
            cout << "Error opening file. Please make sure the file exists and the path is correct." << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string word, hint;
            if (getline(ss, word, '-') && getline(ss, hint)) {
                words.push_back(word);
                hints.push_back(hint);
            }
        }
        file.close();

        if (words.empty()) {
            cout << "No words available in the file!" << endl;
            return;
        }

        while (tries > 0 && score < score_needed) {
            system("cls");
            srand(time(NULL));
            int word_number = rand() % words.size();
            word = words[word_number];
            hintMessage = hints[word_number];

            string Hide_Word(word.length(), '_');
            time_t start_time = time(NULL);
            bool first_guess = true;

            while (tries > 0 && score < score_needed) {
                time_t current_time = time(NULL);
                int elapsed_time = static_cast<int>(current_time - start_time);
                int time_left = max(0, word_timer_seconds - elapsed_time);

                Hang_Man('n', score, time_left);

                cout << "\t\t\t\tLife: " << tries << endl;
                cout << "\t\t\t\t" << Hide_Word << endl << endl;
                cout << "\t\t\t\tGuess a letter (or type 'help' for a hint): ";
                cin >> input;

                system("cls");

                if (input == "help") {
                    message = "Hint: " + hintMessage;
                    continue;
                }

                if (input.length() == 1) {
                    if (first_guess) {
                        start_time = time(NULL);
                        first_guess = false;
                    }
                    letter = input[0];

                    if (CheckGuess(letter, word, Hide_Word) > 0) {
                        message = "Correct Guess";
                    }
                    else {
                        message = "Incorrect Letter!";
                        tries--;
                        elapsed_time += time_penalty_seconds;
                        start_time -= time_penalty_seconds;
                    }

                    if (Hide_Word == word) {
                        score += points_per_correct_guess;
                        message = "New Word!";
                        break;
                    }
                }
                else {
                    cout << "Please enter a single letter or type 'help' for a hint." << endl;
                }

                if (time_left <= 0) {
                    message = "Time's Up!";
                    Hang_Man('h', score, time_left);
                    tries--;
                }
            }

            if (score >= score_needed) {
                message = "Congratulations! You're free.";
                Hang_Man('f', score, word_timer_seconds);
            }
            else if (tries == 0) {
                message = "You're Hanged!";
                Hang_Man('h', score, word_timer_seconds);
            }
        }
    }
};

int main() {
    HangmanGame game;
    int Set[] = { 7, 7, 7 };
    int counter = 1;
    char key;
    bool play_again = true;

    while (play_again) {
        while (true) {
            system("cls");

            cout << "\t\t\t\t _ " << endl;
            cout << "\t\t\t\t| | " << endl;
            cout << "\t\t\t\t| |__   __ _ _ __   __ _ _ __ ___   __ _ _ __  " << endl;
            cout << "\t\t\t\t| '_ \\ / _` | '_ \\ / _` | '_ ` _ \\ / _` | '_ \\ " << endl;
            cout << "\t\t\t\t| | | | (_| | | | | (_| | | | | | | (_| | | | |" << endl;
            cout << "\t\t\t\t|_| |_|\\__,_|_| |_|\\__, |_| |_| |_|\\__,_|_| |_|" << endl;
            cout << "\t\t\t\t                    __/ |                      " << endl;
            cout << "\t\t\t\t                   |___/                       " << endl;

            game.gotoxy(50, 10);
            game.color(Set[0]);
            cout << "Play";

            game.gotoxy(50, 11);
            game.color(Set[1]);
            cout << "Credits";

            game.gotoxy(50, 12);
            game.color(Set[2]);
            cout << "Exit";

            key = _getch();

            if (key == 72 && (counter > 1)) {
                counter--;
            }
            if (key == 80 && (counter < 3)) {
                counter++;
            }
            if (key == '\r') {
                system("cls");
                if (counter == 1) {
                    game.Game_Start();

                    char play_again_choice;
                    cout << "\n\t\t\t\tDo you want to play again? (y/n): ";
                    cin >> play_again_choice;

                    if (play_again_choice == 'y' || play_again_choice == 'Y') {
                        play_again = true;
                    }
                    else {
                        play_again = false;
                    }

                    break;
                }
                if (counter == 2) {
                    game.gotoxy(50, 11);
                    cout << "   Hangman Game" << endl;
                    game.gotoxy(50, 12);
                    cout << "___________________" << endl;
                    game.gotoxy(50, 13);
                    cout << "Developed by IICONIC" << endl;
                    game.gotoxy(50, 14);
                    cout << "Made with C++" << endl;

                    system("pause");
                    system("cls");
                }
                if (counter == 3) {
                    return 0;
                }
            }

            Set[0] = Set[1] = Set[2] = 7;
            Set[counter - 1] = 12;
        }
    }

    return 0;
}
