
# Задача

Дан вектор структур Book с полями 

- title (строка) 
- year (целое).

Отсортируйте книги по году издания (по возрастанию). Если годы совпадают – по названию (лексикографически). Выведите отсортированный список.

```
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

struct Book {
    std::string title;
    int year;
};

int main() {
    std::vector<Book> books = {
        {"1984", 1949},
        {"Brave New World", 1932},
        {"Fahrenheit 451", 1953},
        {"Animal Farm", 1945},
        {"1984", 2017}  // другая книга с тем же названием
    };

    std::sort(books.begin(), books.end(),
              [](const Book& a, const Book& b) {
                  if (a.year != b.year) return a.year < b.year;
                  return a.title < b.title;
              });

    for (const auto& b : books) {
        std::cout << b.title << " (" << b.year << ")\n";
    }
    return 0;
}
```

# Задача

Дан вектор строк. Скопируйте только те строки, длина которых не превышает 3 символа, в новый вектор short_words. \
Затем отсортируйте полученный вектор по алфавиту и удалите дубликаты.

```
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

int main() {
    std::vector<std::string> words = {"cat", "dog", "elephant", "bee", "cat", "ant", "a"};

    std::vector<std::string> short_words;
    std::copy_if(words.begin(), words.end(), std::back_inserter(short_words),
                 [](const std::string& s) { return s.size() <= 3; });

    std::sort(short_words.begin(), short_words.end());
    auto last = std::unique(short_words.begin(), short_words.end());
    short_words.erase(last, short_words.end());

    for (const auto& w : short_words) {
        std::cout << w << " ";
    }
    // Возможный вывод (зависит от исходных данных): a ant bee cat dog
    return 0;
}
```
# Задача

Дан вектор структур Point с полями x и y. Создайте новый вектор distances, содержащий расстояния от каждой точки до начала координат (√(x²+y²)). 
Для вычисления корня используйте std::sqrt  #include <cmath>

```
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

struct Point {
    double x, y;
};

int main() {
    std::vector<Point> points = {{1.0, 0.0}, {3.0, 4.0}, {0.0, 5.0}};
    std::vector<double> distances(points.size());

    std::transform(points.begin(), points.end(), distances.begin(),
                   [](const Point& p) {
                       return std::sqrt(p.x * p.x + p.y * p.y);
                   });

    for (size_t i = 0; i < points.size(); ++i) {
        std::cout << "Point (" << points[i].x << "," << points[i].y
                  << ") distance = " << distances[i] << std::endl;
    }
    return 0;
}
```

# Задача
Даны два вектора целых чисел, каждый из которых уже отсортирован и не содержит дубликатов внутри себя.  \
Получите вектор, содержащий объединение двух множеств (уникальные элементы, присутствующие хотя бы в одном из векторов)

```
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

int main() {
    std::vector<int> a = {1, 3, 5, 7};
    std::vector<int> b = {2, 3, 6, 7, 8};
    std::vector<int> result;

    std::set_union(a.begin(), a.end(),
                   b.begin(), b.end(),
                   std::back_inserter(result));

    std::cout << "Union: ";
    for (int x : result) std::cout << x << " ";
    // Вывод: 1 2 3 5 6 7 8
    return 0;
}
```

