
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const double PI = 3.14159265358979323846;
const double MIN_SIDE = 0.001, MAX_SIDE = 100000.0;
const string FILE_NAME = "result.txt";

// ===== Безпечне введення: повторюємо запит, доки не введуть коректне число =====
void checkEof(istream& in) {                  // кінець потоку (Ctrl+Z / Ctrl+D) - виходимо
    if (in.eof()) { cout << "\nКінець введення.\n"; exit(0); }
}
int readInt(istream& in, const string& prompt, int lo, int hi) {
    int x;
    while (true) {
        cout << prompt;
        if (in >> x && in.peek() == '\n' && x >= lo && x <= hi) { in.ignore(1000, '\n'); return x; }
        checkEof(in);
        cout << "  Помилка: потрібне ціле число від " << lo << " до " << hi << ".\n";
        in.clear();                           // скидаємо стан помилки потоку
        in.ignore(10000, '\n');               // викидаємо сміття з рядка
    }
}
double readDouble(istream& in, const string& prompt, double lo, double hi) {
    double x;
    while (true) {
        cout << prompt;
        if (in >> x && in.peek() == '\n' && x >= lo && x <= hi) { in.ignore(1000, '\n'); return x; }
        checkEof(in);
        cout << "  Помилка: потрібне число від " << lo << " до " << hi << " (через крапку).\n";
        in.clear();
        in.ignore(10000, '\n');
    }
}

// Друга лінія захисту: конструктори теж не приймають некоректні розміри
double checkSize(double v, const string& name) {
    if (!(v >= MIN_SIDE && v <= MAX_SIDE))    // така форма ловить і NaN
        throw invalid_argument(name + " має бути від 0.001 до 100000.");
    return v;
}

// ===== Перший базовий клас: КУБ =====
class Cube {
protected:
    double a;                                 // сторона куба
public:
    Cube(double side) : a(checkSize(side, "Сторона куба")) { cout << "  [конструктор Cube]\n"; }
    ~Cube() { cout << "  [деструктор Cube]\n"; }
    double volume() const { return a * a * a; }
    double surface() const { return 6 * a * a; }
    void print(ostream& out) const {
        out << "Куб: сторона = " << a << ", об'єм = " << volume() << ", площа поверхні = " << surface() << "\n";
    }
};

// ===== Другий базовий клас: СФЕРА =====
class Sphere {
protected:
    double r;                                 // радіус сфери
public:
    Sphere(double radius) : r(checkSize(radius, "Радіус сфери")) { cout << "  [конструктор Sphere]\n"; }
    ~Sphere() { cout << "  [деструктор Sphere]\n"; }
    double volume() const { return 4.0 / 3.0 * PI * r * r * r; }
    double surface() const { return 4 * PI * r * r; }
    void print(ostream& out) const {
        out << "Сфера: радіус = " << r << ", об'єм = " << volume() << ", площа поверхні = " << surface() << "\n";
    }
};

// ===== Похідний клас: СФЕРА, ВПИСАНА У КУБ (множинне успадкування) =====
class InscribedSphere : public Sphere, public Cube {
public:
    // Вписана сфера має радіус r = a / 2
    InscribedSphere(double side) : Sphere(checkSize(side, "Сторона куба") / 2), Cube(side) {
        cout << "  [конструктор InscribedSphere]\n";
    }
    ~InscribedSphere() { cout << "  [деструктор InscribedSphere]\n"; }

    // volume() є і в Sphere, і в Cube, тому просто volume() було б неоднозначним.
    // Невизначеність усуваємо явним зазначенням класу: Cube::volume(), Sphere::volume().
    double freeVolume() const { return Cube::volume() - Sphere::volume(); }
    double ratio() const { return Sphere::volume() / Cube::volume(); }   // завжди pi/6

    void print(ostream& out) const {          // працює і для cout, і для файлового потоку
        out << fixed << setprecision(4);
        Cube::print(out);
        Sphere::print(out);
        out << "Вільний об'єм між кубом і сферою = " << freeVolume() << "\n"
            << "Відношення об'ємів сфера/куб = " << ratio() << " (pi/6 = 0.5236)\n";
    }
};

// ===== Робота з файлом =====
void writeToFile(const InscribedSphere& s) {
    ofstream f(FILE_NAME, ios::out | ios::app);        // app - дописуємо в кінець файлу
    if (!f.is_open()) throw runtime_error("Не вдалося відкрити файл для запису.");
    s.print(f);
    f << "----------------------------------------\n";
    if (!f) throw runtime_error("Помилка під час запису у файл.");
    f.close();
    cout << "Записано у файл " << FILE_NAME << "\n";
}
void showFile() {
    ifstream f(FILE_NAME);                             // за замовчуванням ios::in
    if (!f.is_open()) { cout << "Файл ще не створено.\n"; return; }
    string line;
    while (getline(f, line)) cout << line << "\n";
}
void clearFile() {
    ofstream f(FILE_NAME, ios::out | ios::trunc);      // trunc - стираємо вміст
    if (!f.is_open()) throw runtime_error("Не вдалося відкрити файл.");
    cout << "Файл очищено.\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);                         // українські літери в консолі Windows
    SetConsoleCP(65001);
#endif
    while (true) {
        cout << "\n1-Розрахувати  2-Розрахувати і записати у файл  3-Показати файл  4-Очистити файл  0-Вихід\n";
        int choice = readInt(cin, "Ваш вибір: ", 0, 4);
        if (choice == 0) break;
        try {
            if (choice == 1 || choice == 2) {
                double side = readDouble(cin, "Сторона куба: ", MIN_SIDE, MAX_SIDE);
                InscribedSphere s(side);               // об'єкт існує лише в цьому блоці
                s.print(cout);
                if (choice == 2) writeToFile(s);
            }                                          // тут викликаються деструктори
            else if (choice == 3) showFile();
            else clearFile();
        }
        catch (const invalid_argument& e) { cout << "Некоректне значення: " << e.what() << "\n"; }
        catch (const runtime_error& e) { cout << "Помилка файлу: " << e.what() << "\n"; }
        catch (const exception& e) { cout << "Інша помилка: " << e.what() << "\n"; }
    }
    cout << "Кінець програми.\n";
}