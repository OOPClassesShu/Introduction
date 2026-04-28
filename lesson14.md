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
[ capture ] ( parameters ) -> return_type { body }
```
- capture — список захвата переменных.
- parameters — как у обычной функции (можно опустить, если нет аргументов).
- return_type — возвращаемый тип (может быть опущен, компилятор выведет).
- body — тело функции.

Простейшая лямбда:
```
auto sayHello = []() { std::cout << "Hello\n"; };
sayHello();
```

С параметрами и выводом типа:
```
auto add = [](int a, int b) { return a + b; };
std::cout << add(2,3); // 5 (тип int выведен автоматически)
```

С явным указанием возвращаемого типа
```
auto divide = [](double a, double b) -> double { return a / b; };
```

### Захват переменных

Лямбда может захватывать переменные из окружающей области видимости по значению или по ссылке.

|Способ захвата|	Синтаксис|	Пояснение|
|-|-|-|
|По значению|	[x, y]|	Копирует переменные x и y.|
|По ссылке|	[&x, &y]|	Захватывает по ссылке.|
|Все по значению|	[=]|	Все используемые переменные копируются.|
|Все по ссылке|	[&]|	Все переменные захватываются по ссылке.|
|По значению, кроме ...|	[=, &y]|	y по ссылке, остальные по значению.|
|По ссылке, кроме ...|	[&, x]|	x по значению, остальные по ссылке.|
|Захват текущего объекта|	[this]|	Захватывает this (чтобы использовать поля класса).|


```
int x = 10, y = 20;

// Захват x по значению, y по ссылке
auto l1 = [x, &y]() { 
    // x++ нельзя (const), y++ можно
    y += x;
};
l1();

// Захват всех по ссылке
auto l2 = [&]() {
    x++;
    y++;
};

// Захват всех по значению (константные копии)
auto l3 = [=]() {
    // x++; // ошибка, x константен
    return x + y;
};
```
mutable – позволяет изменять копии захваченных по значению переменных внутри лямбды (но не влияет на оригинал):

```
int count = 0;
auto increment = [count]() mutable {
    count++; // изменяем копию
    return count;
};
increment(); // 1
increment(); // 2
// оригинальный count остался 0
```

### Лямбды в алгоритмах STL

Главное применение лямбд – передача критериев в алгоритмы.

```
#include <vector>
#include <algorithm>

std::vector<int> v = {1,2,3,4,5};
// Удалить чётные
v.erase(std::remove_if(v.begin(), v.end(),
                       [](int n) { return n % 2 == 0; }),
        v.end());

// Сортировка по убыванию
std::sort(v.begin(), v.end(),
          [](int a, int b) { return a > b; });
```

Лямбда, возвращающая лямбду (замыкания)

```
auto make_multiplier = [](int factor) {
    return [factor](int x) { return x * factor; };
};

auto times3 = make_multiplier(3);
std::cout << times3(10); // 30
```

Обобщённые лямбды 
```
auto add_generic = [](auto a, auto b) { return a + b; };
std::cout << add_generic(2, 3) << std::endl;   // 5
std::cout << add_generic(2.5, 1.5) << std::endl; // 4.0
```

## Обобщенные примеры

Калькулятор с выбором операции (указатель на функцию)

```
#include <iostream>

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

int main() {
    int (*op)(int, int) = nullptr;
    char choice;
    std::cin >> choice;
    
    switch (choice) {
        case '+': op = add; break;
        case '-': op = sub; break;
        case '*': op = mul; break;
    }
    
    if (op) std::cout << op(10, 5) << std::endl;
}
```

Обратный вызов с std::function

```
#include <iostream>
#include <functional>
#include <vector>

class Button {
public:
    void setOnClick(std::function<void()> callback) {
        clickHandler = callback;
    }
    void click() {
        if (clickHandler) clickHandler();
    }
private:
    std::function<void()> clickHandler;
};

int main() {
    Button btn;
    int counter = 0;
    
    btn.setOnClick([&counter]() {
        counter++;
        std::cout << "Clicked " << counter << " times\n";
    });
    
    btn.click(); // Clicked 1 times
    btn.click(); // Clicked 2 times
}
```

Лямбда в алгоритме сортировки сложных объектов
```
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

struct Person {
    std::string name;
    int age;
};

int main() {
    std::vector<Person> people = {{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};
    
    // Сортировка по возрасту
    std::sort(people.begin(), people.end(),
              [](const Person& a, const Person& b) {
                  return a.age < b.age;
              });
    
    for (const auto& p : people) {
        std::cout << p.name << " (" << p.age << ")\n";
    }
}
```

std::function в векторе

```
#include <functional>
#include <vector>
#include <iostream>

int main() {
    std::vector<std::function<void()>> tasks;
    
    int x = 10;
    tasks.push_back([]() { std::cout << "Task 1\n"; });
    tasks.push_back([&x]() { std::cout << "Task 2, x=" << x << "\n"; });
    tasks.push_back(std::function<void()>([](){ std::cout << "Task 3\n"; }));
    
    for (auto& task : tasks) {
        task();
    }
}
```
## Задачки

```
Задача
std::function как фильтр

Напишите функцию std::vector<int> filter(const std::vector<int>& data, std::function<bool(int)> predicate),
которая возвращает новый вектор, содержащий только элементы, удовлетворяющие предикату. Проверьте с лямбдой, отбирающей чётные числа.
```
```
Задача
Хранилище обработчиков событий
Создайте класс EventManager, который хранит std::vector<std::function<void(const std::string&)>>.
Реализуйте методы subscribe (добавить обработчик) и trigger (вызвать все обработчики, передав им строку).
В main подпишите две разные лямбды (одна выводит сообщение в консоль, другая, например, подсчитывает количество вызовов) и вызовите trigger("Hello").




#include <iostream>
#include <vector>
#include <functional>
#include <string>

class EventManager {
private:
    std::vector<std::function<void(const std::string&)>> handlers;
public:
    // Подписать новый обработчик
    void subscribe(std::function<void(const std::string&)> handler) {
        handlers.push_back(handler);
    }
    
    // Вызвать все обработчики, передав им сообщение
    void trigger(const std::string& message) {
        for (const auto& handler : handlers) {
            handler(message);
        }
    }
};

int main() {
    EventManager em;
    
    // Счётчик вызовов (захват по ссылке, чтобы изменять оригинал)
    int callCount = 0;
    
    // Подписываем первый обработчик: просто выводит сообщение
    em.subscribe([](const std::string& msg) {
        std::cout << "Handler 1 received: " << msg << std::endl;
    });
    
    // Второй обработчик: увеличивает счётчик и выводит его значение
    em.subscribe([&callCount](const std::string& msg) {
        callCount++;
        std::cout << "Handler 2 (call #" << callCount << ") got: " << msg << std::endl;
    });
    
    // Третий обработчик: выводит длину сообщения
    em.subscribe([](const std::string& msg) {
        std::cout << "Handler 3: message length = " << msg.size() << std::endl;
    });
    
    // Триггерим события
    em.trigger("Hello, world!");
    em.trigger("Event #2");
    
    return 0;
}
```

```
Задача
Лямбда как компаратор для std::set
Создайте std::set<int, std::function<bool(int,int)>>, передав в конструктор лямбду, которая сортирует числа по убыванию.
Добавьте несколько чисел и выведите их (проверьте порядок).

```


```
Задача
 «Счётчик вызовов» через std::function
Создайте функцию-обёртку counted, которая принимает любую функцию std::function<void()> и возвращает новую std::function<void()>,
которая при каждом вызове увеличивает счётчик и выводит количество вызовов. Используйте лямбду с захватом счётчика.

auto counted = [](std::function<void()> f) {
    int count = 0;
    return [f, count]() mutable { count++; f(); std::cout << "Вызов #" << count << "\n"; };
};

```
