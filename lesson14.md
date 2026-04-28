[В оглавление](README.md)

# Занятие 14

# Поговорим об указателях на функцию )

## Указатель на функцию

Указатель на функцию — это способ сохранить адрес функции и вызвать её позже. Он даёт возможность выбирать, \
какая функция будет выполняться во время работы программы.\
Указатель на функцию хранит адрес кода функции в памяти. Тип указателя определяется возвращаемым типом и типами аргументов функции.

```
// Указатель на функцию, возвращающую int и принимающую два int
int (*funcPtr)(int, int) = nullptr;

// Альтернативный синтаксис (более читаемый с помощью using)
using Operation = int(*)(int, int);
Operation op = nullptr;
```

Присваивание и вызов

```
#include <iostream>

int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }

int main() {
    // Объявляем указатель
    int (*operation)(int, int) = nullptr;
    
    // Присваиваем адрес функции add
    operation = add;
    std::cout << operation(5, 3) << std::endl; // 8
    
    // Меняем функцию
    operation = subtract;
    std::cout << operation(5, 3) << std::endl; // 2
    
    // Можно сразу при объявлении
    int (*op2)(int, int) = add;
    
    return 0;
}
```

Функции с одинаковой сигнатурой, но разными именами

```
void transform(int* arr, size_t size, int (*func)(int)) {
    for (size_t i = 0; i < size; ++i) {
        arr[i] = func(arr[i]);
    }
}

int square(int x) { return x * x; }
int negate(int x) { return -x; }

int main() {
    int data[] = {1,2,3,4};
    transform(data, 4, square);   // {1,4,9,16}
    transform(data, 4, negate);   // {-1,-4,-9,-16}
}
```

Указатели на методы класса

```
class Calculator {
public:
    int add(int a, int b) { return a + b; }
};

int main() {
    // Указатель на метод класса Calculator
    int (Calculator::*methodPtr)(int, int) = &Calculator::add;
    
    Calculator calc;
    int result = (calc.*methodPtr)(5, 3); // 8
}
```
Но в современном C++ для таких целей чаще используют std::function или лямбды, которые проще.

Ограничения указателей на функции:
- Не могут захватывать состояние.
- Не могут быть лямбдами, которые захватывают переменные.
- Тип указателя строго фиксирован: нельзя присвоить функцию с другой сигнатурой.
- Синтаксис громоздкий, особенно для указателей на методы.

## std::function – универсальный функциональный объект

std::function (#include <functional>) — это обёртка, которая может хранить любой вызываемый объект: указатель на функцию, лямбду, функтор (класс с operator()), \
результат std::bind и даже указатель на метод. Она стирает конкретный тип вызываемого объекта, предоставляя единый интерфейс.

Синтаксис и базовое использование
```
#include <functional>
#include <iostream>

int add(int a, int b) { return a + b; }

int main() {
    // std::function<возвращаемый_тип(аргументы)>
    std::function<int(int, int)> func;
    
    func = add;          // можно присвоить указатель на функцию
    std::cout << func(5, 3) << std::endl; // 8
    
    func = [](int a, int b) { return a * b; }; // лямбда
    std::cout << func(5, 3) << std::endl; // 15
}
```
Функторы (классы с operator())

```
struct Square {
    int operator()(int x) const { return x * x; }
};

std::function<int(int)> f = Square();
std::cout << f(4); // 16
```
Указатели на методы класса
```
class Greeter {
public:
    void say(const std::string& name) const {
        std::cout << "Hello, " << name << std::endl;
    }
};

int main() {
    Greeter g;
    // std::function может обернуть метод, но нужно передавать объект
    std::function<void(const Greeter&, const std::string&)> method = &Greeter::say;
    method(g, "Alice");
}
```

Преимущества std::function

- Единый тип для всех вызываемых объектов с одинаковой сигнатурой.
- Можно хранить в контейнерах (std::vector<std::function<void(int)>>).
- Поддерживает лямбды с захватом.
- Удобен для обратных вызовов (callbacks), стратегий, событий.


```
void doWork(vector<int>& vc, function<void(int)> fun) {

    for (auto v : vc) {
        fun(v);
    }
}

void doWork(vector<int>& vc, vector <function<void(int)>> funcs) {

    for (auto v : vc) {
        for (auto fun : funcs)
            fun(v);
    }
}

int main() {
    setlocale(LC_ALL, "rus");

    vector<int> vc = { 1,2,3,45,67,33,221,88,99,22 };

    doWork(vc);
    doWork(vc, fun);
    vector<function<void(int)>> funcs = { fun,fun2 };
    doWork(vc, funcs);

    return 0;
}
```

## Лямбда-выражения

Лямбда — это анонимная функция, которую можно определить прямо в месте использования. \
Она может захватывать переменные из окружающей области видимости.

Синтаксис лямбды

```
```
