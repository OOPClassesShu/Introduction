[В оглавление](README.md)

# Занятие 11

# Приведение типов

## Неявное приведение типов

Компилятор выполняет неявное преобразование автоматически, когда типы совместимы или преобразование определено стандартом.
```
int a = 10;
double b = a;          // int -> double (расширение)

double c = 3.14;
int d = c;             // double -> int (усечение, потеря данных)

char ch = 'A';
int code = ch;         // char -> int (продвижение)

bool flag = 42;        // int -> bool (ненулевое -> true)
```

Когда происходит неявное приведение

- В присваиваниях и инициализациях
- При передаче аргументов в функции
- В возвращаемых значениях
- В арифметических выражениях (обычные арифметические преобразования)

## Явное приведение типов
C-стиль
```
double x = 3.14;
int a = (int)x;        // C-style cast
int b = int(x);        // functional-style cast
```

 ### C++ операторы приведения

#### static_cast

Наиболее часто используемый. Выполняет:
- Преобразования между числовыми типами
- Преобразования указателей/ссылок в иерархии классов (вверх и вниз, но без проверки)
- Вызов явных конструкторов
- Преобразования enum в int и обратно

```
// 1. Числовые типы
double pi = 3.14159;
int intPi = static_cast<int>(pi);  // 3

// 2. Вверх по иерархии (upcast) — безопасно
class Base {};
class Derived : public Base {};
Derived d;
Base* b = static_cast<Base*>(&d);   // upcast, можно и неявно

// 3. Вниз по иерархии (downcast) — опасно, нет проверки!
Base* b2 = new Derived;
Derived* d2 = static_cast<Derived*>(b2);  // OK, но если b2 на самом деле не Derived — UB

// 4. Пользовательские преобразования
class A {
public:
    explicit A(int) {}
};
int n = 10;
A a = static_cast<A>(n);  // вызывает explicit конструктор

// 5. void* в конкретный указатель
void* p = malloc(100);
int* arr = static_cast<int*>(p);
```

#### dynamic_cast

Безопасное приведение вниз по иерархии (и вбок для множественного наследования). \
Работает только с полиморфными типами (хотя бы одна виртуальная функция). Использует RTTI (Run-Time Type Information).

```
class Base {
public:
    virtual ~Base() = default;  // нужен полиморфный тип
};
class Derived : public Base {};

Base* b = new Derived;

// Приведение указателя — при неудаче возвращает nullptr
Derived* d = dynamic_cast<Derived*>(b);
if (d) {
    // работает с Derived
}

// Приведение ссылки — при неудаче бросает std::bad_cast
try {
    Derived& dr = dynamic_cast<Derived&>(*b);
} catch (const std::bad_cast& e) {
    // обработка
}
```
Пример с множественным наследованием:
```
class Base1 { virtual ~Base1() {} };
class Base2 { virtual ~Base2() {} };
class Derived : public Base1, public Base2 {};

Base2* b2 = new Derived;
Derived* d = dynamic_cast<Derived*>(b2);  // корректирует указатель!
```
Используется, когда есть уверенность в реальном типе объекта и хотите безопасно downcast.
В плагин-системах, фабриках, контейнерах гетерогенных объектов.

#### const_cast

Единственное приведение, которое умеет добавлять или снимать cv-квалификаторы (const, volatile). Никаких других преобразований.

```
void print(char* s) {
    std::cout << s;
}

const char* msg = "Hello";
print(const_cast<char*>(msg));  // снимаем const, но осторожно!

// Пример: вызов константного метода для неконстантного объекта
class MyClass {
public:
    void set(int x) { value = x; }
    int get() const { return value; }
private:
    int value = 0;
};

const MyClass obj;
// obj.set(5);  // ошибка
const_cast<MyClass&>(obj).set(5);  // снимаем const — UB, если obj изначально const
```
Снимать const с объекта, который изначально был объявлен const — может привести к неопределённому поведению! \ 
Логично использовать если объект неконстантный, но на него смотрят через константный указатель/ссылку.

Используется для адаптация старых C-функций, которые не принимают const, но не изменяют данные \
или внутри константного метода, когда нужно изменить mutable поле.

### reinterpret_cast


