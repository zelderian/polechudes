#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <windows.h>

using namespace std;

struct Question {
    string text;
    string word;
};

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void setColor(int color) {
    SetConsoleTextAttribute(hConsole, color);
}

void resetColor() {
    SetConsoleTextAttribute(hConsole, 7);
}

void pause_ms(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

void typePrint(const string& text, int delayMs, int color) {
    setColor(color);
    for (size_t i = 0; i < text.size(); ) {
        int len = 1;
        unsigned char c = text[i];
        if ((c & 0x80) == 0x00) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;
        cout << text.substr(i, len);
        i += len;
        pause_ms(delayMs);
    }
    cout << endl;
    resetColor();
}

vector<string> splitUtf8(const string& s) {
    vector<string> result;
    size_t i = 0;
    while (i < s.size()) {
        int len = 1;
        unsigned char c = s[i];
        if ((c & 0x80) == 0x00) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;
        result.push_back(s.substr(i, len));
        i += len;
    }
    return result;
}

void printLine(char symbol, int count, int color) {
    setColor(color);
    for (int i = 0; i < count; i++) cout << symbol;
    cout << endl;
    resetColor();
}

void printMasked(const vector<string>& letters, const vector<bool>& opened) {
    cout << "   ";
    for (size_t i = 0; i < letters.size(); i++) {
        if (opened[i]) {
            setColor(10);
            cout << letters[i];
        } else {
            setColor(14);
            cout << "_";
        }
        resetColor();
        cout << " ";
    }
    cout << endl;
}

int main() {
    system("chcp 65001 > nul");
    setlocale(LC_ALL, "");
    srand((unsigned int)time(nullptr));

    vector<Question> bank = {
        {"Набір інструкцій для виконання комп'ютером", "АЛГОРИТМ"},
        {"Іменована область пам'яті для зберігання даних", "ЗМІННА"},
        {"Впорядкований набір елементів одного типу", "МАСИВ"},
        {"Програма, яка перетворює код у виконуваний файл", "КОМПІЛЯТОР"},
        {"Повторення дій у програмі", "ЦИКЛ"},
        {"Іменований блок коду, що виконує певну дію", "ФУНКЦІЯ"},
        {"Пристрій для обробки інформації", "КОМП'ЮТЕР"},
        {"Набір готових функцій для повторного використання", "БІБЛІОТЕКА"},
        {"Складений тип даних з кількома полями", "СТРУКТУРА"},
        {"Процес пошуку та виправлення помилок у коді", "НАЛАГОДЖЕННЯ"}
    };

    Question q = bank[rand() % bank.size()];
    vector<string> letters = splitUtf8(q.word);
    vector<bool> opened(letters.size(), false);

    int attempts = 6;
    int hintsUsed = 0;
    int opened_count = 0;

    setColor(13);
    cout << "============================================" << endl;
    typePrint("            ПОЛЕ ЧУДЕС", 20, 13);
    cout << "============================================" << endl;
    resetColor();
    pause_ms(400);

    auto start = chrono::steady_clock::now();

    while (attempts > 0 && opened_count < (int)letters.size()) {
        cout << endl;
        setColor(11);
        cout << "Питання: " << q.text << endl;
        resetColor();
        printMasked(letters, opened);
        cout << endl;
        setColor(6);
        cout << "Спроб залишилось: " << attempts << "   Підказок використано: " << hintsUsed << endl;
        resetColor();
        cout << "1 - назвати літеру" << endl;
        cout << "2 - назвати ціле слово" << endl;
        cout << "3 - підказка (+2 сек)" << endl;
        cout << "0 - вийти" << endl;
        cout << "Ваш вибір: ";

        int choice;
        cin >> choice;

        if (choice == 0) {
            setColor(12);
            cout << "Гру завершено достроково." << endl;
            resetColor();
            return 0;
        } else if (choice == 1) {
            cout << "Введіть літеру (великою): ";
            string input;
            cin >> input;
            bool found = false;
            for (size_t i = 0; i < letters.size(); i++) {
                if (!opened[i] && letters[i] == input) {
                    opened[i] = true;
                    opened_count++;
                    found = true;
                }
            }
            if (found) {
                setColor(10);
                cout << "Правильно! Літеру відкрито." << endl;
                resetColor();
            } else {
                attempts--;
                setColor(12);
                cout << "Немає такої літери. Спроба втрачена." << endl;
                resetColor();
            }
        } else if (choice == 2) {
            cout << "Введіть слово повністю: ";
            string guess;
            cin >> guess;
            if (guess == q.word) {
                for (size_t i = 0; i < opened.size(); i++) opened[i] = true;
                opened_count = (int)letters.size();
                setColor(10);
                cout << "Вірно! Ви вгадали слово!" << endl;
                resetColor();
            } else {
                attempts--;
                setColor(12);
                cout << "Невірне слово. Спроба втрачена." << endl;
                resetColor();
            }
        } else if (choice == 3) {
            vector<int> closedIdx;
            for (size_t i = 0; i < opened.size(); i++) if (!opened[i]) closedIdx.push_back((int)i);
            if (!closedIdx.empty()) {
                int idx = closedIdx[rand() % closedIdx.size()];
                opened[idx] = true;
                opened_count++;
                hintsUsed++;
                setColor(14);
                cout << "Підказка відкрила одну літеру." << endl;
                resetColor();
            }
        } else {
            setColor(12);
            cout << "Невірний вибір, спробуйте ще раз." << endl;
            resetColor();
        }
        pause_ms(300);
    }

    auto end = chrono::steady_clock::now();
    long long elapsedSec = chrono::duration_cast<chrono::seconds>(end - start).count();
    elapsedSec += hintsUsed * 2;

    cout << endl;
    printLine('=', 44, 13);
    if (opened_count == (int)letters.size()) {
        typePrint("ВІТАЄМО! Ви розгадали слово: " + q.word, 15, 10);
    } else {
        setColor(12);
        cout << "На жаль, спроби закінчились." << endl;
        cout << "Загадане слово було: " << q.word << endl;
        resetColor();
    }
    printLine('=', 44, 13);

    setColor(6);
    cout << "Загальний час розгадування: " << elapsedSec << " сек." << endl;
    cout << "(з урахуванням " << hintsUsed << " підказок по 2 сек)" << endl;
    resetColor();

    cout << endl << "Натисніть Enter для виходу...";
    cin.ignore();
    cin.get();
    return 0;
}
