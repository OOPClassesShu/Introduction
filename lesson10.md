
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

