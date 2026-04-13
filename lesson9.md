# Исключения

Исключения (exceptions) — это встроенный механизм языка C++ для разделения нормального потока выполнения и обработки ошибок. \
При возникновении ошибки программа генерирует исключение, которое перехватывается в специальном блоке.

## try, throw, catch

```
#include <iostream>
#include <stdexcept>

double divide(double a, double b) {
    if (b == 0.0)
        throw std::runtime_error("Деление на ноль!");
    return a / b;
}

int main() {
    try {
        double result = divide(10, 0);
        std::cout << "Результат: " << result << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }
    return 0;
}
```

## Иерархия стандартных исключений

```
std::exception
    ├── std::logic_error
    │      ├── std::invalid_argument
    │      ├── std::out_of_range
    │      ├── std::length_error
    │      └── ...
    └── std::runtime_error
           ├── std::range_error
           ├── std::overflow_error
           └── std::underflow_error
```
Все они имеют виртуальный метод what(), возвращающий строку с пояснением.
Можно ловить по базовому типу std::exception&, обрабатывая все стандартные исключения сразу.
```
try {
    // код
}
catch (const std::out_of_range& e) {
    // специальная обработка выхода за границы
}
catch (const std::exception& e) {
    // все остальные исключения от std::exception
}
```
## Создание собственных классов исключений

Часто удобно определить свои типы исключений, унаследовав от std::exception (или std::runtime_error). Это позволяет передавать дополнительную информацию об ошибке.

```
#include <iostream>
#include <exception>
#include <string>

class MyException : public std::exception {
    std::string msg;
public:
    explicit MyException(const std::string& message) : msg(message) {}
    const char* what() const noexcept override {
        return msg.c_str();
    }
};

void riskyFunction(int value) {
    if (value < 0)
        throw MyException("Отрицательное значение недопустимо!");
}

int main() {
    try {
        riskyFunction(-5);
    }
    catch (const MyException& e) {
        std::cout << "Поймано своё исключение: " << e.what() << std::endl;
    }
    return 0;
}
```

## Исключения в конструкторах

Конструкторы не возвращают значения, поэтому единственный надёжный способ сообщить об ошибке создания объекта — сгенерировать исключение. \
Если в конструкторе возникло исключение, объект не считается созданным, его деструктор вызван не будет (но уже созданные подобъекты будут уничтожены автоматически). \
Не допускайте утечек ресурсов в конструкторе. Лучше использовать RAII (умные указатели, контейнеры), чтобы при исключении ресурсы освобождались автоматически.
```
class Array {
    int* data;
    size_t size;
public:
    Array(size_t n) : size(n) {
        if (n == 0)
            throw std::invalid_argument("Размер массива должен быть > 0");
        data = new int[n];  // может бросить std::bad_alloc
    }
    ~Array() { delete[] data; }
};
```
## Исключения в деструкторах

Деструкторы не должны генерировать исключения. Если деструктор вызывается в процессе раскрутки стека из-за другого исключения, \
а деструктор сгенерирует своё исключение, программа аварийно завершится. Если в деструкторе может возникнуть ошибка, обработайте её внутри, не выкидывая наружу 
```
class FileCloser {
    FILE* f;
public:
    ~FileCloser() {
        try {
            if (f && fclose(f) != 0) {
                // логируем ошибку, но не бросаем исключение
                std::cerr << "Ошибка закрытия файла\n";
            }
        } catch (...) {
            // ничего не делаем, подавляем
        }
    }
};
```
## Спецификаторы noexcept

noexcept говорит компилятору и программисту, что функция не генерирует исключений. \
Это позволяет компилятору генерировать более эффективный код и даёт возможность использовать move-семантику в контейнерах.
```
void safeFunction() noexcept {
    // не должно быть throw и вызовов функций, которые могут бросить
}

// если всё же исключение вылетит из noexcept-функции -> std::terminate
```

Используйте noexcept для:

- деструкторов
- операций обмена (swap)
- move-конструкторов/операторов присваивания

Пример
```
#include <iostream>
#include <stdexcept>
#include <vector>

class BankAccount {
    double balance;
public:
    explicit BankAccount(double initial) : balance(initial) {
        if (initial < 0)
            throw std::invalid_argument("Начальный баланс не может быть отрицательным");
    }
    void withdraw(double amount) {
        if (amount > balance)
            throw std::runtime_error("Недостаточно средств");
        if (amount < 0)
            throw std::invalid_argument("Сумма снятия не может быть отрицательной");
        balance -= amount;
    }
    double getBalance() const { return balance; }
};

int main() {
    try {
        BankAccount acc(100);
        acc.withdraw(200);
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Ошибка аргумента: " << e.what() << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Ошибка выполнения: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Общая ошибка: " << e.what() << std::endl;
    }
    return 0;
}
```
Пример (только для меня )
```
int main8(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");
    /*try {
        std::string path = "test_ofstream.txt";
        std::ifstream ifs;
        ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        std::cout << "Попытка открыть файл" << std::endl;
        ifs.open(path);
        std::cout << "Файл открыт" << std::endl;
    }
    catch (std::fstream::failure ex) {
        std::cout << ex.what() << std::endl;
        std::cout << ex.code() << std::endl;
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }*/


    //foo(11);

    /*try {        
        foo(-12);
    }
    catch (int i) {
        std::cout << "int error " << i << std::endl;
    }
    catch (const char* er) {
        std::cout << "const char* error " << er << std::endl;
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    catch(...){
        std::cout << "Что-то пошло не так!" << std::endl;
    }

    MyMass mass;
    std::cout << mass[12] << std::endl;
    // попробовать поймать базовый класс!!!!

    try {        
        std::cout << mass[12] << std::endl;
    }catch(MyException& ex) {
        std::cout << "wrong index " << ex.getIndex() << std::endl;
    }


    return 0;
}
```
