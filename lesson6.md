
[В оглавление](README.md)
# Занятие 6

# Шаблоны классов

В объектно-ориентированном программировании мы часто сталкиваемся с ситуацией, когда нужно создать класс, который работает с разными типами данных, \
но логика его работы остаётся одинаковой. Классический пример — контейнерные классы: массив, список, стек. \
Без шаблонов пришлось бы создавать отдельные классы для каждого типа: IntArray, DoubleArray, StringArray… \
Это приводит к дублированию кода и усложняет поддержку. \
Шаблоны классов позволяют параметризовать класс типом данных. Это механизм параметрического полиморфизма в C++. \
Код пишется один раз, а компилятор автоматически генерирует конкретные версии класса для нужных типов.

## Синтаксис шаблона класса: базовый пример

Простой класс Pair, хранящий два значения, возможно разных типов.
```
#include <iostream>
#include <string>

// Объявление шаблона класса с двумя параметрами типов T и U
template <typename T, typename U>
class Pair {
private:
    T first;
    U second;

public:
    Pair(const T& f, const U& s) : first(f), second(s) {}

    T getFirst() const { return first; }
    U getSecond() const { return second; }

    void print() const {
        std::cout << "(" << first << ", " << second << ")" << std::endl;
    }
};

int main() {
    Pair<int, double> p1(10, 3.14);
    p1.print(); // (10, 3.14)

    Pair<std::string, std::string> p2("Hello", "World");
    p2.print(); // (Hello, World)

    return 0;
}
```
Обычно шаблоны классов полностью определяются в заголовочном файле (.h), потому что компилятор должен видеть полное определение шаблона в момент инстанцирования \
(генерации кода для конкретных типов). Размещение реализации в .cpp файле приводит к ошибкам линковки. \
Для каждого метода, определяемого вне класса, необходимо:
- Повторить объявление шаблона с теми же параметрами.
- Указать квалификатор ИмяКласса<Параметры>::.

```
// Файл: Pair.h
template <typename T, typename U>
class Pair {
private:
    T first;
    U second;
public:
    Pair(const T& f, const U& s);      // объявление конструктора
    T getFirst() const;                // объявление метода
    void print() const;
};

// Определения методов вне класса
template <typename T, typename U>
Pair<T, U>::Pair(const T& f, const U& s) : first(f), second(s) {}

template <typename T, typename U>
T Pair<T, U>::getFirst() const {
    return first;
}

template <typename T, typename U>
void Pair<T, U>::print() const {
    std::cout << "(" << first << ", " << second << ")" << std::endl;
}
```

## Полная специализация шаблона класса

Бывают ситуации, когда для конкретного набора типов нужно изменить поведение класса. \
Например, для Pair<bool, bool> мы можем захотеть хранить пару битов в одном байте для экономии памяти. \
Полная специализация означает, что мы задаём конкретные типы для всех параметров шаблона. 
```
// Полная специализация для Pair<bool, bool>
template <>
class Pair<bool, bool> {
private:
    unsigned char bits; // используем один байт: бит 0 - first, бит 1 - second

public:
    Pair(bool f, bool s) {
        bits = (f ? 1 : 0) | ((s ? 1 : 0) << 1);
    }

    bool getFirst() const { return bits & 1; }
    bool getSecond() const { return (bits >> 1) & 1; }

    void print() const {
        std::cout << "(" << getFirst() << ", " << getSecond() << ")" << std::endl;
    }
};
```
Теперь при использовании Pair<bool, bool> компилятор выберет специализированную версию, а для остальных — основную. \
Для полностью специализированного класса определение методов вне тела выглядит почти как для обычного класса: \
шаблонные параметры отсутствуют, вместо них используются конкретные типы.
```
// Полная специализация Pair<bool, bool>
template <>
class Pair<bool, bool> {
    // ... объявления
public:
    Pair(bool f, bool s);
    bool getFirst() const;
    void print() const;
};

// Определения методов для полной специализации
// Шаблонный заголовок template <> указывает, что это специализация
template <>
Pair<bool, bool>::Pair(bool f, bool s) {
    bits = (f ? 1 : 0) | ((s ? 1 : 0) << 1);
}

template <>
bool Pair<bool, bool>::getFirst() const {
    return bits & 1;
}

template <>
void Pair<bool, bool>::print() const {
    std::cout << "(" << getFirst() << ", " << getSecond() << ")" << std::endl;
}
```

## Частичная специализация шаблона класса

Если шаблон имеет несколько параметров, мы можем специализировать его не для всех, а только для некоторых. Это называется частичной специализацией.

### Частичная специализация для одинаковых типов

Допустим, мы хотим, чтобы Pair<T, T> (оба типа одинаковы) имел дополнительный метод sum() (если типы поддерживают операцию сложения). \
При этом общий шаблон Pair<T, U> такого метода не предоставляет.
```
// Частичная специализация: оба типа одинаковы
template <typename T>
class Pair<T, T> {
private:
    T first;
    T second;

public:
    Pair(const T& f, const T& s) : first(f), second(s) {}

    T getFirst() const { return first; }
    T getSecond() const { return second; }

    void print() const {
        std::cout << "(" << first << ", " << second << ")" << std::endl;
    }

    // Дополнительный метод, доступный только когда типы одинаковы
    T sum() const { return first + second; }
};

int main() {
    Pair<int, double> p1(5, 3.14); // использует общий шаблон
    // p1.sum(); // ошибка компиляции: метод sum не существует

    Pair<int, int> p2(5, 10);      // использует частичную специализацию
    std::cout << p2.sum() << std::endl; // 15

    return 0;
}
```
Для частичной специализации синтаксис определение методов вне тела аналогичен полной, но шаблонные параметры — это те, которые не были специализированы.
```
// Частичная специализация: оба типа одинаковы
template <typename T>
class Pair<T, T> {
private:
    T first;
    T second;
public:
    Pair(const T& f, const T& s);
    T sum() const;
    void print() const;
};

// Определения методов для частичной специализации
template <typename T>
Pair<T, T>::Pair(const T& f, const T& s) : first(f), second(s) {}

template <typename T>
T Pair<T, T>::sum() const {
    return first + second;
}

template <typename T>
void Pair<T, T>::print() const {
    std::cout << "(" << first << ", " << second << ")" << std::endl;
}
```



Рассмотрим частичную специализацию Pair<T, T> (оба типа одинаковы):

## Особенности использования шаблонов классов
### Размещение кода шаблонов
Как уже упоминалось, реализация шаблонов обычно помещается в заголовочные файлы. \
Это связано с тем, что компилятор должен видеть полное определение шаблона в точке его инстанцирования. \
Альтернативой является явное инстанцирование для нужных типов в .cpp файле, но это ограничивает гибкость.

### Момент инстанцирования
Код для конкретного набора типов генерируется только тогда, когда этот шаблон используется. \
Например, если в программе нигде не встречается Pair<std::string, int>, то и соответствующая реализация не будет скомпилирована. \
Это может привести к тому, что ошибки в методах, которые не используются, не будут обнаружены до момента инстанцирования.

### Специализация методов
Можно специализировать не весь класс, а отдельные его методы. Это бывает удобно, если нужно изменить поведение только одной функции для определённого типа.
```
template <typename T>
class Printer {
public:
    void print(const T& value) {
        std::cout << value << std::endl;
    }
};

// Специализация метода print для типа bool
template <>
void Printer<bool>::print(const bool& value) {
    std::cout << (value ? "true" : "false") << std::endl;
}

int main() {
    setlocale(LC_ALL, "Ru");
    srand(time(NULL));
    Printer<int> p;
    p.print (10);
    Printer<bool> p1;
    p1.print (10);
    return 0;
}
```

# Наследование 

```
#include <iostream>
#include <typeinfo>

template <typename T>
class Box {
public:
    void set(T v) {
        value = v;
    }

    T get() {
        return value;
    }

private:
    T value;
};

template <typename T>
class Base {
public:
    T get() { return value; }
protected:
    T value;
};

template <typename T, typename U>
class Derived : public Base<T> {
    U  v1;


public:
    void print() {
        //T x = get(); // error
        T x = this->get();
        T x1 = Base<T>::get();
    }

};

template <typename T>
class Device {
protected:
    T data;  // данные, с которыми работает устройство
public:
    explicit Device(T d) : data {d} {
        std::cout << "Device<T>::configure (T= " << typeid(T).name() << ")" << std::endl;
    }

    virtual void start() const {
        std::cout << "Device<T>::start (T=" << typeid(T).name() << ")" << std::endl;
    }

    virtual void shutdown() const {
        std::cout << "Device<T>::shutdown (T=" << typeid(T).name() << ")" << std::endl;
    }

    virtual void send(const T& msg) const {
        std::cout << "Device<T>::send (T=" << typeid(T).name() << ")" << std::endl;
    }

    virtual void configure() const {
        std::cout << "Device<T>::configure (T=" << typeid(T).name() << ")" << std::endl;
    }

    virtual ~Device<T>() {}
};


template <typename T>
class NetworkDevice : public Device<T> {
protected:
    bool isConnected;  // состояние сети
public:

    explicit NetworkDevice(T d, bool connected = false) : Device<T>(d), isConnected(connected) {
        std::cout << "NetworkDevice<T> ctor (T=" << typeid(T).name() << ")" << std::endl;
    }

    // переопределяем некоторые виртуальные методы
    void start() const override {
        std::cout << "NetworkDevice<T>::start (T=" << typeid(T).name() << ") — checking network..." << std::endl;
        if (isConnected) {
            std::cout << "  network OK" << std::endl;
        } else {
            std::cout << "  network not connected" << std::endl;
        }
    }
    void send(const T& msg) const override {
        std::cout << "NetworkDevice<T>::send (T=" << typeid(T).name() << ") — sending over network: " << msg << std::endl;
    }
    void configure() const override {
        std::cout << "NetworkDevice<T>::configure (T=" << typeid(T).name() << ") — network settings" << std::endl;
    }

    virtual ~NetworkDevice() {
        std::cout << "NetworkDevice<T> dtor (T=" << typeid(T).name() << ")" << std::endl;
    }
};


template <typename T, typename U>
class Router : public NetworkDevice<T> {
protected:
    U address;  // адрес маршрутизатора (например, IP-адрес, строка, число и т.д.)
public:
    Router(T d, U addr, bool connected = true) : NetworkDevice<T>(d, connected), address(addr) {
        std::cout << "Router<T,U> ctor (T=" << typeid(T).name() << ", U=" << typeid(U).name() << ")" << std::endl;
    }

    // переопределяем виртуальные методы
    void start() const override {
        std::cout << "Router<T,U>::start (T=" << typeid(T).name() << ", U=" << typeid(U).name() << ") — starting routing engine..." << std::endl;
        // можем вызвать версию из NetworkDevice<T> (через this-> или NetworkDevice<T>::)
        NetworkDevice<T>::start();  // базовая проверка сети
    }
    void send(const T& msg) const override {
        std::cout << "Router<T,U>::send (T=" << typeid(T).name() << ", U=" << typeid(U).name() << ") — routing message: " << msg << " to address " << address << std::endl;
    }
    void configure() const override {
        std::cout << "Router<T,U>::configure (T=" << typeid(T).name() << ", U=" << typeid(U).name() << ") — router config, address=" << address << std::endl;
    }

    virtual ~Router() {
        std::cout << "Router<T,U> dtor (T=" << typeid(T).name() << ", U=" << typeid(U).name() << ")" << std::endl;
    }
};

template <typename T, typename U, typename V>
class SecureRouter : public Router<T, U> {
protected:
    V key;  // ключ шифрования
public:
    SecureRouter(T d, U addr, V k, bool connected = true) : Router<T, U>(d, addr, connected), key(k) {
        std::cout << "SecureRouter<T,U,V> ctor (T=" << typeid(T).name() << ", U=" << typeid(U).name() << ", V=" << typeid(V).name() << ")" << std::endl;
    }

    // переопределяем виртуальные методы
    void start() const override {
        std::cout << "SecureRouter<T,U,V>::start — initializing security layer..." << std::endl;
        // вызываем Router<T,U>::start() — это базовая логика маршрутизации/сети
        Router<T, U>::start();
    }

    void send(const T& msg) const override {
        std::cout << "SecureRouter<T,U,V>::send — encrypting message with key " << key << std::endl;
        // после шифрования вызываем базовую отправку (маршрутизацию)
        Router<T, U>::send(msg);
    }

    void configure() const override {
        std::cout << "SecureRouter<T,U,V>::configure — secure config, key=" << key << std::endl;
        // можно дополнить базовую конфигурацию
        Router<T, U>::configure();
    }

    virtual ~SecureRouter() {
        std::cout << "SecureRouter<T,U,V> dtor (T=" << typeid(T).name() << ", U=" << typeid(U).name() << ", V=" << typeid(V).name() << ")" << std::endl;
    }
};


int main() {
    std::cout << "\n=== Создаем SecureRouter<int, const char*, int> ===\n";

    // T = int, U = const char* (адрес), V = int (ключ)
    Device<int>* dev = new SecureRouter<int, const char*, int>(42, "192.168.1.1", 1234, true);

    std::cout << "\n--- dev->start() ---\n";
    dev->start();  // виртуальный вызов: SecureRouter -> Router -> NetworkDevice -> Device (цепочка)

    std::cout << "\n--- dev->configure() ---\n";

    dev->configure();  // цепочка переопn--- dev->send(100";
    dev->send(100);  // цепочка: шифрование ->shutdown() ---\n";
    dev->shutdown();  // вызовется Device<int>::shutdown (если "\n--- удаление через базовый указатель ---\n";
    delete dev;  // вызов цепдаем Router<double, int> (через указатель на NetworkDevice) ===\n";

    return 0;
}



int main_11 () {

    Box<int> b;
    Derived<int,double> f;
    return 0;

}
```
