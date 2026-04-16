
[В оглавление](README.md)

# Занятие 10

# Работа с файлами

В C++ для работы с файлами используются классы из заголовочного файла <fstream>, которые наследуются от классов <iostream>:


| Наследование	| Класс для файлов	| Назначение |
|---------------|------------------|------------|
|istream	      |  ifstream	       |чтение из файла |
|ostream	      |ofstream	         |запись в файл |
|iostream	      |fstream	         |чтение и запись |

## Открытие файла

Файл можно открыть двумя способами:

- Через конструктор — сразу при создании объекта.
- Через метод open() — для повторного использования объекта.

```
ifstream fin("input.txt");        // открыть на чтение
ofstream fout("output.txt");      // открыть на запись (создаст/перезапишет)
fstream fio("data.txt", ios::in | ios::out); // открыть на чтение и запись
```
После открытия проверяем успешность:

```
if (!fin.is_open()) {
    cerr << "Ошибка открытия файла!" << endl;
    return 1;
}
```

## Режимы открытия

Флаги задаются вторым аргументом в конструктор или open(). Комбинируются через битовое ИЛИ |.
| Флаг |	Значение |
|------|-----------|
|ios::in|	открыть для чтения|
|ios::out|	открыть для записи|
|ios::binary|	бинарный режим (по умолчанию текстовый)|
|ios::app|	запись в конец файла|
|ios::ate	|переместиться в конец при открытии|
|ios::trunc|	очистить файл при открытии (по умолчанию для ofstream)|

```
ofstream fout("log.txt", ios::app);          // дописывать в конец
fstream fs("file.txt", ios::in | ios::out);  // чтение и запись без очистки
ifstream fin("data.bin", ios::binary);       // бинарное чтение
```
## Закрытие файла

Файл автоматически закрывается при уничтожении объекта (деструктор). Но можно закрыть явно методом close() \
Явное закрытие полезно, если нужно открыть другой файл тем же объектом или обработать ошибки.
```
fin.close();
```
## Запись в файл

Классы ofstream и fstream поддерживают оператор << (для форматированного вывода) и метод write() (для бинарной записи).

```
ofstream fout("test.txt");
fout << "Hello, world!" << 42 << endl;
fout.write((char*)&data, sizeof(data)); // бинарная запись
```

## Чтение из файла

Классы ifstream и fstream поддерживают оператор >> (форматированный ввод) и методы get(), getline(), read().

```
ifstream fin("test.txt");
string word;
fin >> word;              // читает слово до пробела
char ch;
fin.get(ch);              // читает один символ
char buffer[100];
fin.getline(buffer, 100); // читает строку до '\n'
string line;
getline(fin, line);       // более удобный вариант (из <string>)

// бинарное чтение
fin.read((char*)&data, sizeof(data));
```
## Проверка состояния потока

Методы good(), eof(), fail(), bad() позволяют определить состояние.
```
if (fin.eof()) cout << "Достигнут конец файла" << endl;
if (fin.fail()) cout << "Ошибка формата или открытия" << endl;
if (fin.bad())  cout << "Неисправимая ошибка потока" << endl;
```
Чтения до конца файла:
```
int x;
while (fin >> x) {   // пока чтение успешно
    // обработать x
}
```
## Позиционирование в файле

|Метод	|Назначение|
|-------|----------|
|tellg()|	текущая позиция для чтения (get pointer)
|tellp()	|текущая позиция для записи (put pointer)
|seekg(off, dir)|	переместить указатель чтения
|seekp(off, dir)|	переместить указатель записи

Параметр dir (откуда смещаться):
- ios::beg — от начала файла
- ios::cur — от текущей позиции
- ios::end — от конца файла
```
fstream fs("data.txt", ios::in | ios::out);
fs.seekg(0, ios::end);       // перейти в конец для чтения
streampos size = fs.tellg(); // узнать размер
fs.seekg(0, ios::beg);       // вернуться в начало
```

Запись и чтение текстового файла
```
#include <fstream>
#include <iostream>
#include <string>

int main() {
    // Запись
    std::ofstream out("example.txt");
    if (!out) {
        std::cerr << "Не удалось создать файл\n";
        return 1;
    }
    out << "Строка 1\n";
    out << "Строка 2 с числом " << 100 << "\n";
    out.close();

    // Чтение
    std::ifstream in("example.txt");
    if (!in) {
        std::cerr << "Не удалось открыть файл\n";
        return 1;
    }
    std::string line;
    while (std::getline(in, line)) {
        std::cout << "Прочитано: " << line << '\n';
    }
    in.close();

    return 0;
}
```

## Копирование файла побайтово
```
#include <fstream>
#include <iostream>

int main() {
    std::ifstream src("source.bin", std::ios::binary);
    std::ofstream dst("dest.bin", std::ios::binary);
    if (!src || !dst) {
        std::cerr << "Ошибка открытия файлов\n";
        return 1;
    }
    dst << src.rdbuf(); // самый простой способ скопировать
    // или посимвольно:
    // char ch;
    // while (src.get(ch)) dst.put(ch);
    std::cout << "Копирование завершено\n";
    return 0;
}
```

## Чтение чисел из файла и запись суммы

```
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("numbers.txt");
    if (!in) {
        std::cerr << "Ошибка открытия numbers.txt\n";
        return 1;
    }
    int sum = 0, value;
    while (in >> value) {
        sum += value;
    }
    in.close();

    std::ofstream out("sum.txt");
    if (!out) {
        std::cerr << "Ошибка создания sum.txt\n";
        return 1;
    }
    out << "Сумма = " << sum << '\n';
    return 0;
}
```
## Работа с бинарными файлами (массив структур)

```
#include <fstream>
#include <iostream>
#include <cstring>

struct Student {
    char name[50];
    int age;
    double grade;
};

int main() {
    Student students[] = {{"Alice", 20, 4.8}, {"Bob", 22, 4.2}};
    // Запись
    std::ofstream fout("students.bin", std::ios::binary);
    fout.write(reinterpret_cast<char*>(students), sizeof(students));
    fout.close();

    // Чтение
    Student read_students[2];
    std::ifstream fin("students.bin", std::ios::binary);
    fin.read(reinterpret_cast<char*>(read_students), sizeof(read_students));
    fin.close();

    for (const auto& s : read_students) {
        std::cout << s.name << ", " << s.age << ", " << s.grade << '\n';
    }
    return 0;
}
```
## Произвольный доступ (запись в конец и чтение с начала)

```
#include <fstream>
#include <iostream>
#include <string>

int main() {
    // Открываем для чтения и записи, создаём если нет
    std::fstream fs("log.txt", std::ios::in | std::ios::out | std::ios::app);
    if (!fs) {
        std::cerr << "Ошибка открытия\n";
        return 1;
    }
    // Пишем в конец (благодаря ios::app)
    fs << "Новая запись\n";

    // Чтобы прочитать всё с начала, нужно сбросить флаг EOF и переместиться
    fs.clear();                 // сбрасываем eofbit, failbit
    fs.seekg(0, std::ios::beg);

    std::string line;
    while (std::getline(fs, line)) {
        std::cout << "Содержимое: " << line << '\n';
    }
    return 0;
}
```

## std::istringstream

std::istringstream (сокращение от input string stream) — это класс из заголовочного файла <sstream>, \
который позволяет работать со строкой (std::string) как с источником данных для форматированного ввода. \
Проще говоря, он превращает строку в поток, из которого можно читать данные с помощью оператора >>, точно так же, как из std::cin или std::ifstream.\

Иерархия наследования
```
std::ios_base
    ↑
std::ios
    ↑
std::istream
    ↑
std::istringstream
```
Таким образом, std::istringstream наследует все методы std::istream: >>, get(), getline(), read(), ignore(), eof(), fail()

### Основные операции
```
#include <sstream>
#include <iostream>
#include <string>

std::string data = "42 3.14 Hello";
std::istringstream iss(data);   // iss теперь читает из строки data

std::istringstream iss;
iss.str("123 456");   // устанавливаем строку
```

### Чтение данных

```
int a;
double b;
std::string c;
iss >> a >> b >> c;
std::cout << a << ", " << b << ", " << c << std::endl; // 42, 3.14, Hello
```

### Проверка успешности

```
int x;
if (iss >> x) {
    // успешно прочитали число
} else {
    // ошибка: в строке не было числа
}
```

### Чтение до конца строки 
```
std::string rest;
std::getline(iss, rest); // читает всё, что осталось в потоке

# Например, после iss >> x указатель в потоке продвигается. Чтобы сбросить позицию в начало, ye;yj использjdfnm iss.seekg(0).
```

### Пример 1: Разбор строки с разными типами

```
#include <sstream>
#include <iostream>

int main() {
    std::string line = "Ivanov 25 180.5";
    std::istringstream iss(line);
    
    std::string name;
    int age;
    double height;
    
    iss >> name >> age >> height;
    std::cout << name << " " << age << " " << height << std::endl;
    return 0;
}
```

### Преобразование строки в число

```
#include <sstream>
#include <iostream>
#include <string>

bool to_int(const std::string& s, int& value) {
    std::istringstream iss(s);
    iss >> value;
    // проверяем, что чтение прошло успешно и достигнут конец строки
    return !iss.fail() && iss.eof();
}

int main() {
    int v;
    if (to_int("123abc", v))
        std::cout << "OK: " << v << std::endl;
    else
        std::cout << "Не число" << std::endl; // сработает из-за "abc"
    
    if (to_int("  456  ", v))
        std::cout << "OK: " << v << std::endl; // пробелы игнорируются, OK
    return 0;
}
```
### Комбинация с файловым вводом

```
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

int main() {
    std::ifstream file("data.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name;
        int score;
        iss >> name >> score;
        if (iss) {
            std::cout << name << ": " << score << std::endl;
        } else {
            std::cerr << "Некорректная строка: " << line << std::endl;
        }
    }
    return 0;
}
```
### Другие классы из <sstream>

**std::ostringstream** – строковый поток вывода \
Позволяет формировать строку с помощью << (как в std::cout или std::ofstream). 
```
std::ostringstream oss;
oss << "a = " << 42 << ", pi = " << 3.14;
std::string result = oss.str();   // "a = 42, pi = 3.14"
```
**std::stringstream** – двунаправленный поток \ 
Позволяет и читать, и писать в одну строку.
```
std::stringstream ss;
ss << 100 << " " << 200;
int a, b;
ss >> a >> b;   // a=100, b=200
```

## Еще немного

- После чтения до конца потока eofbit установится. Если необходимо читать заново, нужно сбросить состояние ss.clear() и при необходимости ss.seekg(0).
- Пробелы игнорируются при использовании >> по умолчанию. Чтобы читать пробелы, необходимо использовать std::noskipws или get().
- Разделители – >> останавливается на любом пробельном символе. Для других разделителей используйте std::getline(iss, token, delimiter).
- Производительность – создание istringstream для каждой строки обычно приемлемо. Для очень больших объёмов можно переиспользовать один поток, меняя его строку через str() и сбрасывая состояние.
