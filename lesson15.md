[В оглавление](README.md)

# Занятие 15

# Алгоритмы в stl

STL предоставляет более сотни готовых алгоритмов, которые выполняют наиболее востребованные действия над последовательностями элементов.

Проблемы, которые решают алгоритмы STL:

- Повторение кода: Сортировка пузырьком или наивный поиск пишутся начинающими снова и снова, хотя есть эффективные, отлаженные реализации.
- Смешение логики и итерации: Алгоритмы позволяют отделить "что сделать" (логику) от "как перебрать" (цикл).
- Совместимость: Один и тот же алгоритм (например, std::find) умеет работать с std::vector, std::list, std::deque, и даже с обычным массивом C++.

STL-алгоритмы являются обобщенными функциями, которые оперируют не конкретными контейнерами, а диапазонами, заданными парой итераторов. \
Именно эта концепция обеспечивает их невероятную гибкость и переиспользуемость.


Концепция полуоткрытых диапазонов: [first, last). Почти все алгоритмы ожидают два итератора — начало диапазона (first) и конец диапазона (last),\
причем last указывает за последний элемент последовательности (элемент, следующий за обрабатываемым). \
Это стандартное соглашение и удобно для циклов, так как last — это естественное условие остановки (while(first != last)).

## Основные категории алгоритмов STL

Все алгоритмы STL можно разделить на несколько крупных категорий, в зависимости от того, какого рода операции они выполняют.

|Категория|	Суть|	Примеры алгоритмов|
|-|-|-|
|Не модифицирующие|	Читают элементы, не изменяя их и не меняя их порядок.|	for_each, find, count, equal|
|Модифицирующие|	Изменяют содержимое элементов или копируют их в новое место.|	copy, transform, fill, replace|
|Операции с сортировкой и множествами|	Упорядочивают или выполняют операции над отсортированными диапазонами.|	sort, merge, set_union, lower_bound|
|Числовые операции|	Выполняют математические и статистические расчеты.|	accumulate, partial_sum, iota|

Эта классификация помогает быстро ориентироваться в огромном количестве функций и выбирать нужную под задачу. \
Многие алгоритмы имеют вариации с суффиксами в названиях, которые уточняют их поведение.

## Понимание суффиксов _if и _copy

Проектировщики STL ввели два специальных суффикса для облегчения понимания названий алгоритмов:

- Суффикс _if: Означает, что алгоритм получает функцию-предикат (или функциональный объект), а не конкретное значение.
  Простой find ищет точное значение, а find_if ищет по условию.
- Суффикс _copy: Означает, что результат работы алгоритма копируется в другой диапазон, а исходная последовательность\
  остается без изменений. reverse переворачивает элементы на месте, а reverse_copy создает перевернутую копию в другом месте.


## Немодифицирующие алгоритмы

Немодифицирующие алгоритмы — это алгоритмы, которые только просматривают или ищут данные, не изменяя их и не меняя порядка следования.\
Эти алгоритмы безопасны: они не повредят ваши данные, даже если вы ошибетесь.

### std::for_each для обхода и не только

По сути, это "цикл в виде функции".
```
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    
    // Просто вывод с помощью лямбды
    std::for_each(v.begin(), v.end(), [](int x) {
        std::cout << x << " ";
    });
    std::cout << std::endl;
    
    // Модифицировать элементы v можно через ссылку
    std::for_each(v.begin(), v.end(), [](int &x) {
        x *= 2;
    });
    
    // Вывод после модификации
    std::for_each(v.begin(), v.end(), [](int x) {
        std::cout << x << " ";
    });
}
```
### Алгоритмы поиска find, find_if и count

Одни из наиболее часто используемых алгоритмов.

- find(begin, end, value) возвращает итератор на первый элемент, равный value. Если такой элемент не найден, возвращает end.
- find_if(begin, end, predicate) ищет первый элемент, для которого predicate (условие) возвращает true.
- count(begin, end, value) возвращает количество элементов в диапазоне, равных value.
- count_if(begin, end, predicate) возвращает количество элементов, удовлетворяющих предикату.


```
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> nums = {5, 2, 7, 2, 9, 2};
    
    // find: ищем первую двойку
    auto it = std::find(nums.begin(), nums.end(), 2);
    if (it != nums.end()) {
        std::cout << "Первая двойка найдена на позиции: " 
                  << std::distance(nums.begin(), it) << std::endl;
    }
    
    // find_if: ищем первое четное число
    auto it_even = std::find_if(nums.begin(), nums.end(), [](int x) { 
        return x % 2 == 0; 
    });
    if (it_even != nums.end()) {
        std::cout << "Первое четное число: " << *it_even << std::endl;
    }
    
    // count: сколько раз встречается 2
    int cnt = std::count(nums.begin(), nums.end(), 2);
    std::cout << "Количество двоек: " << cnt << std::endl;
    
    // count_if: сколько четных
    int cnt_even = std::count_if(nums.begin(), nums.end(), [](int x) {
        return x % 2 == 0;
    });
    std::cout << "Количество четных чисел: " << cnt_even << std::endl;
}
```

## Модифицирующие алгоритмы

Эти алгоритмы изменяют последовательности: копируют, перемещают, удаляют или заменяют элементы.


### std::copy и родственные

Алгоритмы копирования — работа с перемещением данных.

```
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> src = {1, 2, 3, 4, 5};
    std::vector<int> dst(5);
    
    // Простое копирование
    std::copy(src.begin(), src.end(), dst.begin());
    
    std::cout << "Копия: ";
    for (int x : dst) std::cout << x << " ";
    std::cout << std::endl;
    
    // Копирование только четных чисел (copy_if)
    std::vector<int> even_numbers;
    std::copy_if(src.begin(), src.end(), std::back_inserter(even_numbers), 
                 [](int x) { return x % 2 == 0; });
    
    std::cout << "Четные числа: ";
    for (int x : even_numbers) std::cout << x << " "; // 2 4
    std::cout << std::endl;
}
```

### std::transform

std::transform применяет функцию к каждому элементу диапазона и записывает результат преобразования в выходной диапазон. \
Когда нужно преобразовать данные, сохранив исходные, или когда нужно применить операцию между элементами двух последовательностей.
```
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> src = {1, 2, 3, 4};
    std::vector<int> dst(4);
    
    // Унарный режим: умножаем каждый элемент на 2
    std::transform(src.begin(), src.end(), dst.begin(), 
                   [](int x) { return x * 2; });
    
    std::cout << "Умножение на 2: ";
    for (int x : dst) std::cout << x << " "; // 2 4 6 8
    std::cout << std::endl;
    
    // Бинарный режим: поэлементное сложение двух векторов
    std::vector<int> vec1 = {1, 2, 3};
    std::vector<int> vec2 = {4, 5, 6};
    std::vector<int> sum(3);
    
    std::transform(vec1.begin(), vec1.end(), 
                   vec2.begin(), 
                   sum.begin(), 
                   [](int a, int b) { return a + b; });
    
    std::cout << "Поэлементная сумма: ";
    for (int x : sum) std::cout << x << " "; // 5 7 9
    std::cout << std::endl;
}
```

### std::fill, generate для заполнения данных

Эти алгоритмы хороши для начальной инициализации или перезаполнения элементов.

- fill(begin, end, value) присваивает каждому элементу диапазона заданное значение.
- fill_n(begin, count, value) присваивает значение указанному количеству элементов, начиная с begin.
- generate(begin, end, generator) присваивает элементам значения, вычисленные функцией-генератором.
- generate_n(begin, count, generator) аналогично для заданного количества.

```
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

int main() {
    std::vector<int> v(10);
    
    // Заполнить все элементы значением 42
    std::fill(v.begin(), v.end(), 42);
    
    // Заполнить первые 5 элементов нулями
    std::fill_n(v.begin(), 5, 0);
    
    /* 
        заполнить вектор последовательными 
        числами, начиная с 1 
    */
    int value = 1;
    std::generate(v.begin(), v.end(), [&value]() { 
        return value++; 
    });
    // v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
    
    // Заполнить первые 5 элементов квадратами чисел
    int num = 1;
    std::generate_n(v.begin(), 5, [&num]() {
        int res = num * num;
        num++;
        return res;
    });
}
```

## Алгоритмы сортировки и работы с множествами

Эти алгоритмы — "тяжелая артиллерия" для упорядочивания данных и выполнения теоретико-множественных операций, но они требуют, чтобы диапазон был отсортирован.

### std::sort

```
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> v = {5, 2, 8, 1, 9, 3};
    
    // Сортировка по возрастанию (по умолчанию)
    std::sort(v.begin(), v.end());
    // v = {1, 2, 3, 5, 8, 9}
    
    // Сортировка по убыванию (с помощью лямбды)
    std::sort(v.begin(), v.end(), [](int a, int b) {
        return a > b;
    });
    // v = {9, 8, 5, 3, 2, 1}
    
    // Сортировка строк по длине
    std::vector<std::string> words = {"cat", "elephant", "dog", "bee"};
    std::sort(words.begin(), words.end(), [](const std::string& a, const std::string& b) {
        if (a.size() != b.size()) 
            return a.size() < b.size();
        return a < b;
    });
    // Результат: "bee", "cat", "dog", "elephant"
}
```
### Поиск в отсортированных диапазонах

- lower_bound(begin, end, value) возвращает итератор на первый элемент, не меньший value.
- upper_bound(begin, end, value) возвращает итератор на первый элемент, больший value.
- binary_search(begin, end, value) просто проверяет, существует ли такой элемент (возвращает true/false).

```
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> data = {10, 20, 20, 20, 30, 40};
    
    // binary_search: есть ли 20?
    if (std::binary_search(data.begin(), data.end(), 20)) {
        std::cout << "20 найдено!" << std::endl;
    }
    
    // lower_bound: позиция первой 20 (индекс 1)
    auto low = std::lower_bound(data.begin(), data.end(), 20);
    int idx_low = std::distance(data.begin(), low);
    std::cout << "lower_bound для 20 -> индекс: " << idx_low << std::endl;
    
    // upper_bound: позиция после последней 20 (индекс 4)
    auto up = std::upper_bound(data.begin(), data.end(), 20);
    int idx_up = std::distance(data.begin(), up);
    std::cout << "upper_bound для 20 -> индекс: " << idx_up << std::endl;
    
    // Количество 20 = расстояние между lower и upper
    int count20 = std::distance(low, up);
    std::cout << "Количество 20: " << count20 << std::endl;
}
```

### Множественные операции: merge, set_union, set_intersection

Для работы с отсортированными диапазонами. Названия говорят сами за себя: merge (слияние), set_union (объединение), set_intersection (пересечение) и другие
```
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> A = {1, 3, 5, 7};
    std::vector<int> B = {2, 4, 5, 6, 8};
    std::vector<int> result;
    
    // Объединение: все элементы A ∪ B
    std::set_union(A.begin(), A.end(),
                   B.begin(), B.end(),
                   std::back_inserter(result));
    // result = {1,2,3,4,5,6,7,8}
    
    result.clear();
    
    // Пересечение: только общие элементы A ∩ B
    std::set_intersection(A.begin(), A.end(),
                          B.begin(), B.end(),
                          std::back_inserter(result));
    // result = {5}
}
```

## Числовые алгоритмы из <numeric>

Для числовых операций STL предоставляет отдельный заголовочный файл <numeric>. Познакомимся с основными.

- accumulate(begin, end, init) вычисляет сумму (или другую операцию) элементов диапазона.
- partial_sum(begin, end, out) считает частичные суммы (префиксы) диапазона.
- adjacent_difference(begin, end, out) считает разности между соседними элементами.
- iota(begin, end, value) заполняет диапазон последовательно возрастающими значениями, начиная с value

```
#include <iostream>
#include <vector>
#include <numeric>

int main() {
    std::vector<int> nums(10);
    
    // iota: заполнить последовательными числами 1..10
    std::iota(nums.begin(), nums.end(), 1);
    // nums = {1,2,3,4,5,6,7,8,9,10}
    
    // accumulate: найти сумму всех элементов
    int sum = std::accumulate(nums.begin(), nums.end(), 0);
    std::cout << "Сумма: " << sum << std::endl; // 55
    
    // accumulate с пользовательской операцией (произведение)
    int product = std::accumulate(nums.begin(), nums.end(), 1, 
                                   std::multiplies<int>());
    std::cout << "Произведение: " << product << std::endl; // 3628800
    
    // partial_sum: вычисление префиксов
    std::vector<int> prefix(nums.size());
    std::partial_sum(nums.begin(), nums.end(), prefix.begin());
    // prefix = {1,3,6,10,15,21,28,36,45,55}
    
    // adjacent_difference: разности соседних элементов
    std::vector<int> diff(nums.size());
    std::adjacent_difference(nums.begin(), nums.end(), diff.begin());
    // diff = {1,1,1,1,1,1,1,1,1,1}
}
```

## Итераторные адаптеры

Иногда необходимо вставить элементы в контейнер, используя алгоритм. Обычные итераторы этого не позволяют. На помощь приходят итераторные адаптеры из <iterator>:

- std::back_inserter(container) вызывает push_back.
- std::front_inserter(container) вызывает push_front.
- std::inserter(container, it) вызывает insert.

```
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

int main() {
    std::vector<int> src = {1,2,3};
    std::vector<int> dst;
    
    // Копирование с back_inserter — подходит даже для пустого контейнера
    std::copy(src.begin(), src.end(), std::back_inserter(dst));
    // dst = {1,2,3}

    // filter + copy_if
    std::vector<int> even;
    std::copy_if(src.begin(), src.end(), std::back_inserter(even), 
                 [](int x) { return x % 2 == 0; });
}
```

## Еще немного, полезные: swap, minmax_element, shuffle

- swap(a, b) обменивает местами значения двух объектов (O(1) для STL-контейнеров).
- minmax_element(begin, end) (C++11) возвращает пару итераторов на минимальный и максимальный элемент за один проход (эффективнее, чем два раздельных вызова min_element и max_element).
- shuffle(begin, end, generator) (C++11) случайным образом переставляет элементы (замена устаревшего random_shuffle).

```
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

int main() {
    std::vector<int> data = {3, 1, 4, 1, 5, 9, 2};
    
    // minmax_element: находим минимум и максимум
    auto [min_it, max_it] = std::minmax_element(data.begin(), data.end());
    std::cout << "Минимум: " << *min_it << ", Максимум: " << *max_it << std::endl;
    
    // Перемешиваем
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(data.begin(), data.end(), g);
    
    std::cout << "После перемешивания: ";
    for (int x : data) std::cout << x << " ";
    std::cout << std::endl;
    
    // swap векторов (очень быстрый)
    std::vector<int> other = {10, 20, 30};
    data.swap(other); // или std::swap(data, other)
}
```

## Практический пример
Задача: имеется список оценок студентов. Нужно найти средний балл, отфильтровать тех, у кого балл выше среднего, отсортировать их по убыванию и вывести.

```
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

int main() {
    struct Student {
        std::string name;
        int score;
    };
    
    std::vector<Student> students = {
        {"Alice", 85}, {"Bob", 62}, {"Charlie", 91},
        {"Diana", 78}, {"Eve", 95}, {"Frank", 70}
    };
    
    // 1. Подсчет средней оценки с помощью accumulate
    int total = std::accumulate(students.begin(), students.end(), 0,
        [](int sum, const Student& s) { return sum + s.score; });
    double average = static_cast<double>(total) / students.size();
    std::cout << "Средняя оценка: " << average << std::endl;
    
    // 2. Отбор студентов с оценкой выше среднего
    std::vector<Student> top;
    std::copy_if(students.begin(), students.end(), std::back_inserter(top),
        [average](const Student& s) { return s.score > average; });
    
    // 3. Сортировка отобранных по убыванию оценки
    std::sort(top.begin(), top.end(),
        [](const Student& a, const Student& b) { return a.score > b.score; });
    
    // 4. Вывод результата
    std::cout << "\nСтуденты с оценкой выше среднего:" << std::endl;
    std::for_each(top.begin(), top.end(), [](const Student& s) {
        std::cout << s.name << " - " << s.score << std::endl;
    });
    
    return 0;
}
```
```
Задача
Дан вектор целых чисел. Используя алгоритмы STL (без циклов и индексов), найдите сумму всех элементов,
произведение всех четных элементов и количество элементов, больших 10.
```

```
Задача
Дан текст, разделенный пробелами. Используя std::vector<std::string>, удалите все слова короче 4 символов,
затем отсортируйте оставшиеся по алфавиту и удалите дубликаты (последовательные).
Используйте алгоритмы std::remove_if, std::sort, std::unique.
```

```
Задача
Задача-исследование. С помощью std::shuffle и std::sample из большой коллекции случайным образом выберите подвыборку.
Сравните поведение этих алгоритмов.
```


## Только для меня


```
----------------------------------------------------------------- 1 ----------------------------------------------

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

int main() {
    std::vector<int> v = {5, 12, 3, 8, 15, 2, 20, 1};

    // 1. Сумма всех элементов (accumulate)
    int sum = std::accumulate(v.begin(), v.end(), 0);
    std::cout << "Сумма: " << sum << std::endl;

    // 2. Произведение чётных элементов
    //    Сначала отбираем чётные через copy_if, затем перемножаем
    std::vector<int> even;
    std::copy_if(v.begin(), v.end(), std::back_inserter(even),
                 [](int x) { return x % 2 == 0; });
    int product = std::accumulate(even.begin(), even.end(), 1,
                                   std::multiplies<int>());
    std::cout << "Произведение чётных: " << product << std::endl;

    // Альтернативно, один проход с accumulate и пользовательской операцией
    int product2 = std::accumulate(v.begin(), v.end(), 1,
        [](int acc, int x) {
            return (x % 2 == 0) ? acc * x : acc;
        });
    std::cout << "Произведение чётных (в один проход): " << product2 << std::endl;

    // 3. Количество элементов > 10 (count_if)
    int cnt = std::count_if(v.begin(), v.end(), [](int x) { return x > 10; });
    std::cout << "Количество элементов > 10: " << cnt << std::endl;

    return 0;
}


-------------------------------------------------------------- 2 ---------------------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

std::vector<std::string> split_words(const std::string& text, const std::string& delimiters = " \t\n") {
    std::vector<std::string> words;
    size_t start = text.find_first_not_of(delimiters);
    while (start != std::string::npos) {
        size_t end = text.find_first_of(delimiters, start);
        words.push_back(text.substr(start, end - start));
        start = text.find_first_not_of(delimiters, end);
    }
    return words;
}

int main() {
    std::string text = "cat apple dog banana cat apple elephant cat";
    
    // Разбиваем на слова вручную
    std::vector<std::string> words = split_words(text);
    
    // 1. Удаляем слова короче 4 символов (remove_if + erase)
    words.erase(std::remove_if(words.begin(), words.end(),
                               [](const std::string& s) { return s.size() < 4; }),
                words.end());
    
    // 2. Сортируем по алфавиту
    std::sort(words.begin(), words.end());
    
    // 3. Удаляем дубликаты (последовательные)
    words.erase(std::unique(words.begin(), words.end()), words.end());
    
    // Вывод результата
    std::cout << "Результат: ";
    for (const auto& w : words) std::cout << w << " ";
    std::cout << std::endl;
    
    return 0;
}


-------------------------------------------------------------------------- 2 альт -------------------------------------------------


#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>

int main() {
    std::string text = "cat apple dog banana cat apple elephant cat";
    
    // Разбиваем текст на слова
    std::istringstream iss(text);
    std::vector<std::string> words;
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter(words));
    
    // 1. Удаляем слова короче 4 символов (remove_if + erase)
    words.erase(std::remove_if(words.begin(), words.end(),
                               [](const std::string& s) { return s.size() < 4; }),
                words.end());
    
    // 2. Сортируем по алфавиту
    std::sort(words.begin(), words.end());
    
    // 3. Удаляем дубликаты (они теперь стоят рядом)
    words.erase(std::unique(words.begin(), words.end()),
                words.end());
    
    // Вывод результата
    std::cout << "Результат: ";
    for (const auto& w : words) std::cout << w << " ";
    std::cout << std::endl;
    
    return 0;
}
```

Служебное
```
// алгоритмы
// sort

bool sort_direction (int a,int b){
    return a > b;// от большего к меньшему!
}


struct Person{

    string name;
    double score;
    Person (string n, double s) :name{ n }, score{ s }{}
    bool operator() (const Person& p) {
        return p.score > 180;
    }
};

int main4(){
    setlocale(LC_ALL, "rus");

    vector <int> v {1,2,3,4,5,7,6,8,9};

    for (auto a : v)
        cout << a << " ";
    cout << endl;

    sort (v.begin(),v.end(),sort_direction);
    for (auto a : v)
        cout << a << " ";
    cout << endl;


    const int size = 9;
    int mass[size] {1,2,3,4,5,7,6,8,9};
    for (auto a : mass)
        cout << a << " ";
    cout << endl;

    sort (mass,mass+size);
    for (auto a : mass)
        cout << a << " ";
    cout << endl;

    vector<Person> people{
        Person("Иван", 190),
        Person("Петя", 90),
        Person("Федя", 100),
        Person("Анна", 55),
        Person("Катя", 181),
        Person("Таня", 188),
        Person("Нина", 170),
        Person("Жанна", 100)
    };

    for (auto a : people)
        cout << a.name << " " << a.score << endl;
    cout << endl;


    sort (people.begin(),people.end(), [](const Person& p1,const Person& p2 ){

        return p1.score > p2.score;
    });
    for (auto a : people)
        cout << a.name << " " << a.score << endl;
    cout << endl;

    return 0;
}


// алгоритмы
// find find_if find_not_if
int main5(){
    setlocale(LC_ALL, "rus");

    vector <int> v {1,2,3,4,5,7,6,8,9,9};

    for (auto a : v)
        cout << a << " ";
    cout << endl;

    auto fi = find (v.begin(),v.end(),9);

    if (fi != v.end()){
        cout << (*fi) << endl;
    }else{
        cout << "Not find" << endl;
    }

   vector<Person> people{
        Person("Иван", 190),
        Person("Петя", 90),
        Person("Федя", 100),
        Person("Анна", 55),
        Person("Катя", 181),
        Person("Таня", 188),
        Person("Нина", 170),
        Person("Жанна", 100)
    };

    for (auto a : people)
        cout << a.name << " " << a.score << endl;
    cout << endl;


    auto fi1 = find_if(people.begin(),people.end(), [](const Person& p1){
        return p1.score > 100;
    });

    if (fi1 != people.end())
        cout << (*fi1).name << " " << (*fi1).score << endl;

    cout << endl;

    fi1 = people.begin();
    while (fi1 != people.end()){
        fi1 = find_if(fi1,people.end(), [](const Person& p1){
            return p1.score > 100;
        });
        if (fi1 != people.end()){
            cout << (*fi1).name << " " << (*fi1).score << endl;
            fi1++;
        }
    }
    return 0;
}


// алгоритмы
// copy  copy_if
int main6(){
   setlocale(LC_ALL, "rus");

   vector<Person> people{
        Person("Иван", 190),
        Person("Петя", 90),
        Person("Федя", 100),
        Person("Анна", 55),
        Person("Катя", 181),
        Person("Таня", 188),
        Person("Нина", 170),
        Person("Жанна", 100)
    };

    for (auto a : people)
        cout << a.name << " " << a.score << endl;
    cout << endl;

    vector<Person> fi;

    copy_if (people.begin(), people.end(),back_inserter(fi),[](const Person& p1){
        return p1.score > 100;
    });

    for (auto a : fi)
        cout << a.name << " " << a.score << endl;
    cout << endl;

    return 0;
}

// алгоритмы
// remove, remove_if
int main7(){
   setlocale(LC_ALL, "rus");

   vector<int> vi {1,2,3,4,5,5,6,7,8,8,9,0};

    for (auto v : vi)
        cout << v << endl;
    cout << endl;

   auto result = remove (vi.begin(),vi.end(), 8);

  cout << "------------------"<<*result << endl;
  for (auto v : vi)
        cout << v << endl;
    cout << endl;

  vi.erase (result, vi.end());
  for (auto v : vi)
        cout << v << endl;
    cout << endl;


   vector<Person> people{
        Person("Иван", 190),
        Person("Петя", 90),
        Person("Федя", 100),
        Person("Анна", 55),
        Person("Катя", 181),
        Person("Таня", 188),
        Person("Нина", 170),
        Person("Жанна", 100)
    };

    for (auto a : people)
        cout << a.name << " " << a.score << endl;
    cout << endl;


    auto res = remove_if (people.begin(), people.end(),[](const Person& p1){
        return p1.score > 100;
    });


    people.erase (res, people.end());

    for (auto a : people)
        cout << a.name << " " << a.score << endl;
    cout << endl;



    string str = "Моя строка для проврки работы алгоритма";

    cout << str << endl;

    str.erase (remove (str.begin(), str.end(),' '), str.end());

    cout << str << endl;

    return 0;
}


// алгоритмы
// max_element min_element, minmax_element

int main8(){
   setlocale(LC_ALL, "rus");

   vector<int> vi {1,2,3,4,5,5,6,7,8,8,9,0};

   for (auto v : vi)
        cout << v << endl;
    cout << endl;


   auto result = max_element (vi.begin(), vi.end());

   cout << *result << endl;


   list<int> li {1,2,3,4,5,5,6,7,8,8,9,0};

   for (auto v : li)
        cout << v << endl;
    cout << endl;


   auto result1 = max_element (vi.begin(), vi.end());

   cout << *result1 << endl;

   const int size = 12;
   int mass[size] =  {1,2,3,4,5,5,6,7,8,8,9,0};

   for (auto v : mass)
        cout << v << endl;
    cout << endl;

   auto result2 = max_element (mass, mass+size);
   cout << *result2 << "  "<< result2-mass<< endl;


   return 0;
}




// алгоритмы
// #include <numeric>
// сумма, произведение

int main9(){
   setlocale(LC_ALL, "rus");

   vector<int> vi {1,2,3,4,5};

   for (auto v : vi)
        cout << v << endl;
    cout << endl;

    cout << "Sum = "<<accumulate (vi.begin(), vi.end(), 0)<< endl;

    cout << "Sum = "<<accumulate (begin(vi), end(vi), 0)<< endl;

    int mass[] =  {1,2,3,4,5};
    cout << "Sum = "<<accumulate (begin(mass), end(mass), 0)<< endl;


///////////////////////

    cout << "P = "<<accumulate (begin(vi), end(vi), 1,[](int a, int b){
            return a*b;
    })<< endl;


    cout << "P = "<<accumulate (begin(vi), end(vi), 1,[](int a, int b){
            if (b % 2 == 0)
                return a*b;
            return a;
    })<< endl;



    //"1-2-3-4-5"
    cout << "STR = "<<accumulate (begin(vi), end(vi), string(""),[](string a, int b){
            return a+"-"+to_string(b);
    })<< endl;

    cout << "STR = "<<accumulate (begin(vi), end(vi), to_string(vi[0]),[](string a, int b){
            return a+"-"+to_string(b);
    })<< endl;


    cout << "STR = "<<accumulate (next(begin(vi)), end(vi), to_string(vi[0]),[](string a, int b){
            return a+"-"+to_string(b);
    })<< endl;

    return 0;
}



// алгоритмы
//
// equal и mismatch


struct Point {

    int x,y;
    Point (int x = 0, int y=0): x{x}, y{y}{}

};


int main10(){
   setlocale(LC_ALL, "rus");

// сказать про разность длин!!!
// + сортировка

   vector<int> vi {1,2,3,4,5};

   list<int> li {11,2,3,4,5};

   for (auto v : vi)
        cout << v << endl;
    cout << endl;

   for (auto v : li)
        cout << v << endl;
    cout << endl;

    cout << equal (begin(vi), end(vi) , begin(li))<< endl;
    cout << equal (begin(vi), end(vi) , begin(li), end(li))<< endl;

    // где разность 
    auto a=  mismatch (begin(vi), end(vi) , begin(li), end(li));
    cout << *a.first << " " <<*a.second << endl;



    vector <Point> pv1 = {Point(1,3), Point(2,5), Point(6,7)};
    vector <Point> pv2 = {Point(1,3), Point(2,5), Point(6,7)};


    cout << equal (begin(pv1), end(pv1) , begin(pv2), end(pv2), [](const Point& p1, const Point& p2){
                return p1.x == p2.x && p1.y == p2.y;

        })<< endl;

    return 0;
}



// алгоритмы
//
//  for_each
int main(){
   setlocale(LC_ALL, "rus");

   vector<int> vi {1,2,3,4,5};

   for (auto v : vi)
        cout << v << endl;
    cout << endl;

   for_each(begin(vi), end (vi), [](int v){
                cout << v << endl;
    });
    cout << endl;

    return 0;
}
```
