
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
