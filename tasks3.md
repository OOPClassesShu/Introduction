
Напишите программу, которая копирует содержимое файла source.txt в файл dest.txt, заменяя все пробелы на символ подчёркивания _. \
Оба файла открывайте в текстовом режиме.

```
\#include <fstream>
#include <iostream>
#include <cctype>   // для isspace, но мы заменим только пробел ' '

int main() {
    // Открываем исходный файл для чтения
    std::ifstream source("source.txt");
    if (!source.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл source.txt\n";
        return 1;
    }

    // Открываем файл назначения для записи
    std::ofstream dest("dest.txt");
    if (!dest.is_open()) {
        std::cerr << "Ошибка: не удалось создать файл dest.txt\n";
        return 1;
    }

    char ch;
    // Читаем посимвольно до конца файла
    while (source.get(ch)) {
        // Если символ - пробел, заменяем на '_'
        if (ch == ' ') {
            dest.put('_');
        } else {
            dest.put(ch);
        }
    }

    // Файлы закроются автоматически при разрушении объектов
    std::cout << "Копирование завершено. Пробелы заменены на '_'.\n";
    return 0;
}
```

```
#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ifstream source("source.txt");
    std::ofstream dest("dest.txt");
    if (!source || !dest) {
        std::cerr << "Ошибка открытия файлов\n";
        return 1;
    }

    std::string line;
    while (std::getline(source, line)) {
        for (char& c : line) {
            if (c == ' ') c = '_';
        }
        dest << line << '\n';
    }
    return 0;
}
```

## Задание «Ведомость успеваемости»
Дан текстовый файл students.txt, содержащий список студентов и их оценки. Формат файла:
```
Ivanov 5 4 3
Petrov 4 4 5
Sidorov 2 3 4
```
Требуется написать программу, которая:

 - Открывает файл students.txt для чтения с помощью std::ifstream.
 - Читает данные только оператором >> (без getline, get и т.д.).
 - Для каждого студента вычисляет средний балл как (оценка1 + оценка2 + оценка3) / 3.0.
 - Записывает в файл report.txt с помощью std::ofstream строки вида:
```
Иванов: 4.00
Петров: 4.33
Сидоров: 3.00

```
После обработки всех записей программа выводит на экран сообщение:
```
Обработано студентов: 3
Отчёт сохранён в файл report.txt
```

```
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

int main() {
    std::ifstream in("students.txt");
    if (!in) {
        std::cerr << "Не удалось открыть students.txt\n";
        return 1;
    }

    std::ofstream out("report.txt");
    if (!out) {
        std::cerr << "Не удалось создать report.txt\n";
        return 1;
    }

    std::string surname;
    int g1, g2, g3;
    int count = 0;

    // Чтение с помощью оператора >>
    while (in >> surname >> g1 >> g2 >> g3) {
        double avg = (g1 + g2 + g3) / 3.0;
        out << std::fixed << std::setprecision(2);
        out << surname << ": " << avg << "\n";
        ++count;
    }

    std::cout << "Обработано студентов: " << count << "\n";
    std::cout << "Отчёт сохранён в файл report.txt\n";
    return 0;
}
```

```
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>   // для std::istringstream
#include <vector>

int main() {
    std::ifstream in("students.txt");
    if (!in) {
        std::cerr << "Ошибка: не удалось открыть students.txt\n";
        return 1;
    }

    std::ofstream out("report.txt");
    if (!out) {
        std::cerr << "Ошибка: не удалось создать report.txt\n";
        return 1;
    }

    out << std::fixed << std::setprecision(2);
    std::string line;
    int studentCount = 0;

    while (std::getline(in, line)) {
        // Пропускаем пустые строки
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string surname;
        iss >> surname;   // читаем фамилию

        if (surname.empty()) continue; // защита от строк без фамилии

        // Читаем все оценки из этой строки
        std::vector<int> grades;
        int grade;
        while (iss >> grade) {
            grades.push_back(grade);
        }

        // Если оценок нет – пропускаем студента
        if (grades.empty()) {
            std::cerr << "Предупреждение: у студента " << surname 
                      << " нет оценок, пропущен\n";
            continue;
        }

        // Вычисляем среднее
        double sum = 0;
        for (int g : grades) {
            sum += g;
        }
        double average = sum / grades.size();

        // Записываем в отчёт
        out << surname << ": " << average << "\n";
        ++studentCount;
    }

    std::cout << "Обработано студентов: " << studentCount << "\n";
    std::cout << "Отчёт сохранён в report.txt\n";

    return 0;
}
```

##

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
