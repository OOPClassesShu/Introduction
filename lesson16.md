[В оглавление](README.md)

# Занятие 15

# std::chrono

Библиотека для работы с временем

## Основные компоненты

Библиотека строится вокруг трёх ключевых понятий:

- Продолжительность (duration) – величина временного интервала. Например: 5 секунд, 100 миллисекунд.
- Момент времени (time_point) – привязка продолжительности к конкретной временной шкале (часам/таймеру).
  Например: «2025-05-06 12:00:00» или «момент через 10 секунд после запуска программы».
- Часы (clock) – источник текущего времени. Определяет начало эпохи и стабильность.


Все типы лежат в пространстве имён std::chrono.

## Продолжительность — std::chrono::duration 

duration – это шаблонный класс:

```
template <class Rep, class Period = std::ratio<1>>
class duration;
```

- Rep – арифметический тип для счёта тиков (чаще всего int64_t, double).
- Period – отношение «один тик / одна секунда» в виде std::ratio<Numer, Denom>.
  Например:
  - std::ratio<1> – 1 тик = 1 секунда
  - std::ratio<1,1000> – 1 тик = 1/1000 секунды = 1 миллисекунда
  - std::ratio<1,1000000000> – наносекунда


Для удобства предопределены популярные типы:

|тип	|значение|
|-|-|
|std::chrono::nanoseconds|	наносекунды |
|std::chrono::microseconds|	микросекунды|
|std::chrono::milliseconds|	миллисекунды|
|std::chrono::seconds|	секунды|
|std::chrono::minutes|	минуты|
|std::chrono::hours|	часы|

Создание и использование
```
#include <chrono>

using namespace std::chrono;

// явное создание
milliseconds ms(1500);        // 1500 миллисекунд
seconds sec(10);              // 10 секунд

// литералы (C++14)
using namespace std::chrono_literals;
auto ms2 = 1500ms;   // milliseconds
auto sec2 = 10s;     // seconds
auto min2 = 3min;    // minutes
auto us = 123us;     // microseconds
auto ns = 5ns;       // nanoseconds
```

Доступ к значению
```
milliseconds ms(1234);
cout << ms.count();        // 1234 (целое число тиков)
```

## Момент времени — std::chrono::time_point

std::chrono::time_point — это шаблонный тип, представляющий момент времени на временной шкале определённых часов.

Шаблонные параметры
```
template<class Clock, class Duration = typename Clock::duration>
class time_point;
```
- Clock — тип часов, которые определяют эпоху (начало отсчёта) и «тиканье» (например, system_clock, steady_clock).
- Duration — тип продолжительности, используемый для хранения смещения от эпохи часов (обычно Clock::duration, но можно задать свой).
- Внутри time_point хранит только одно значение — объект типа Duration, который показывает, сколько времени прошло с момента начала эпохи данных часов до этого момента.

Конструирование
```
#include <chrono>
using namespace std::chrono;

// 1) Конструктор по умолчанию — представляет эпоху часов
time_point<steady_clock> tp_default;   // время = эпоха часов (обычно 0 тиков)

// 2) Из продолжительности (смещения от эпохи)
time_point<steady_clock> tp_from_dur(seconds(10));   // момент через 10 секунд после эпохи

// 3) Копирование / преобразование
time_point<steady_clock> tp1 = steady_clock::now(); // текущий момент
time_point<steady_clock, milliseconds> tp2(tp1);    // сменили единицу хранения (допустимо, если точность не теряется)
```

Преобразование между time_point разных типов

Как и для duration, нельзя просто так преобразовать time_point<system_clock, nanoseconds>  \
в time_point<system_clock, seconds> — компилятор запретит потерю точности. \
Используйте time_point_cast:

```
auto tp_ns = system_clock::now();          // обычно nanoseconds
auto tp_sec = time_point_cast<seconds>(tp_ns);  // отбрасываем доли секунды
```
Можно преобразовывать только те time_point, которые относятся к одним и тем же часам. Между разными часами преобразование не имеет смысла.

Сложение/вычитание с duration

Это самая частая операция:

- time_point + duration → time_point
- time_point - duration → time_point
- time_point - time_point → duration

```
auto start = steady_clock::now();
auto middle = start + 250ms;          // через 250 мс
auto end = middle + 250ms;            // ещё через 250 мс

auto elapsed = end - start;           // duration (500ms)

// Проверка, наступил ли уже момент
auto deadline = start + 5s;
if (steady_clock::now() >= deadline) {
    std::cout << "5 секунд прошло\n";
}
```

Сравнение time_point

Моменты времени, порождённые одними и теми же часами, можно сравнивать: ==, !=, <, >, <=, >=. \
Сравнение моментов от разных часов — запрещено (и логически бессмысленно).

```
auto t1 = steady_clock::now();
std::this_thread::sleep_for(100ms);
auto t2 = steady_clock::now();

if (t2 > t1) { /* всегда истинно, так как часы монотонные */ }
```

Эпоха часов

Каждый тип часов определяет свою точку отсчёта (эпоху).

- system_clock на всех популярных платформах (Linux, Windows, macOS) считает эпохой 1970-01-01 00:00:00 UTC (Unix time). Именно поэтому to_time_t() работает.
- steady_clock обычно считает эпохой момент запуска программы или время загрузки системы. Гарантировано, что эпоха у steady_clock никогда не повторяется и не сдвигается.

Обратите внимание: сам по себе time_point не знает, что такое «год» или «месяц» — это просто смещение от эпохи. Для перевода в календарную дату нужно использовать функции-помощники (например, system_clock::to_time_t()).

Пример: измерение с привязкой к абсолютному времени
```
#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace std::chrono_literals;

int main() {
    // Абсолютная временная метка (UNIX-время)
    time_point<system_clock> epoch;  // эпоха system_clock (1970-01-01)
    auto now = system_clock::now();

    // Продолжительность жизни Unix-времени
    auto unix_age = now - epoch;
    std::cout << "Секунд с начала эпохи: " 
              << duration_cast<seconds>(unix_age).count() << '\n';

    // Добавляем 1 минуту к текущему моменту
    auto one_minute_later = now + minutes(1);
    
    // Сравниваем
    if (one_minute_later > now) {
        std::cout << "Будущее уже близко\n";
    }

    // Ждём до определённого момента (C++11)
    std::this_thread::sleep_until(one_minute_later);
    std::cout << "Прошла минута!\n";
}
```

Преобразование time_point в строку
```
auto now = system_clock::now();
std::time_t t = system_clock::to_time_t(now);
std::cout << std::ctime(&t);  // "Tue May  6 12:34:56 2025\n"
```


|Часы|	Характеристика|
|-|-|
|system_clock|	соответствует системному времени (стенные часы). Может прыгать (пользователь изменил время, NTP). Полезна для вывода дат/времени.|
|steady_clock|	монотонные часы (никогда не идут назад). Идеальны для измерения интервалов.|
|high_resolution_clock|	обычно алиас для самого мелкозернистого таймера (часто steady_clock).|


Статические методы часов

- now() – возвращает time_point с текущим временем у данных часов.
- time_since_epoch() – продолжительность от начала эпохи часов.
- to_time_t() / from_time_t() (только system_clock) – преобразование в time_t (секунды от эпохи Unix).

------------
- steady_clock::now() – надёжный секундомер, который никогда не сломается из-за изменения системного времени.
- system_clock::now() – календарные часы, показывающие реальное время (которое может скакать), полезны для вывода дат и взаимодействия с внешним миром.

```
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

int main() {
    auto t1 = std::chrono::system_clock::now();
    std::this_thread::sleep_for(500ms);
    // Допустим, в этот момент пользователь перевел часы на 1 час назад
    auto t2 = std::chrono::system_clock::now();

    auto diff = t2 - t1;   // Может быть отрицательным или сильно меньше 500 мс!
    std::cout << "Измерено: " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << " ms\n";
    // Вывод, например: "-3599500 ms" (если час назад)
}
```

Измерение времени выполнения
```
#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace std::chrono_literals;

int main() {
    auto start = steady_clock::now();

    // симуляция работы
    std::this_thread::sleep_for(200ms);

    auto end = steady_clock::now();
    auto duration_ms = duration_cast<milliseconds>(end - start);

    std::cout << "Время выполнения: " << duration_ms.count() << " ms\n";
    // вывод: Время выполнения: 200 ms (приблизительно)
}
```

вывод текущего времени
```
#include <ctime>
#include <iostream>
#include <chrono>

int main() {
    using namespace std::chrono;
    auto now = system_clock::now();
    time_t t = system_clock::to_time_t(now);
    std::cout << "current time: " << std::ctime(&t);
}
```


Задержки и тайм-ауты

```
std::this_thread::sleep_for(500ms);               // задержка 0.5 сек
std::this_thread::sleep_until(steady_clock::now() + 1s); // до определённого момента
```

