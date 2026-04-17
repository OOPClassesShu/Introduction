```
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>

// Допустим, класс Texture (упрощённо)
class Texture {
    std::string path;
public:
    explicit Texture(const std::string& p) : path(p) {
        std::cout << "Loading texture from " << path << '\n';
        // здесь могла бы быть реальная загрузка изображения
    }
    ~Texture() {
        std::cout << "Unloading texture " << path << '\n';
    }
    void use() const {
        std::cout << "Using texture " << path << '\n';
    }
};

class TextureCache {
    std::unordered_map<std::string, std::weak_ptr<Texture>> cache;
public:
    // Возвращает shared_ptr на текстуру. Если текстура уже есть в кэше и жива – возвращает её,
    // иначе загружает новую.
    std::shared_ptr<Texture> get(const std::string& path) {
        auto it = cache.find(path);
        if (it != cache.end()) {
            // Пытаемся получить shared_ptr из weak_ptr
            auto tex = it->second.lock();
            if (tex) {
                std::cout << "Cache hit for " << path << '\n';
                return tex;
            } else {
                // weak_ptr сброшен – объект уже удалён, удаляем запись из кэша
                std::cout << "Cache stale for " << path << ", reloading\n";
                cache.erase(it);
            }
        }

        // Загружаем новую текстуру
        auto tex = std::make_shared<Texture>(path);
        cache[path] = tex;   // сохраняем weak_ptr
        return tex;
    }

    // Опционально: метод для очистки "мёртвых" weak_ptr (можно вызывать периодически)
    void clean_stale() {
        for (auto it = cache.begin(); it != cache.end(); ) {
            if (it->second.expired()) {
                it = cache.erase(it);
            } else {
                ++it;
            }
        }
    }

    // Вывод размера кэша (для отладки)
    size_t size() const { return cache.size(); }
};

// Демонстрация
int main() {
    TextureCache cache;

    // Первый запрос – загрузка
    auto tex1 = cache.get("grass.png");
    tex1->use();

    // Второй запрос того же пути – попадание в кэш
    auto tex2 = cache.get("grass.png");
    tex2->use();

    // tex1 и tex2 указывают на один и тот же объект
    std::cout << "Use count: " << tex1.use_count() << '\n'; // должно быть 2

    // Сброс одного из указателей
    tex1.reset();
    std::cout << "After reset tex1, use_count = " << tex2.use_count() << '\n'; // 1

    // Запрашиваем ещё раз – кэш всё ещё действителен (tex2 жив)
    auto tex3 = cache.get("grass.png");
    std::cout << "Use count: " << tex3.use_count() << '\n'; // 2 (tex2 и tex3)

    // Сбрасываем все указатели на grass.png
    tex2.reset();
    tex3.reset();

    // Теперь объект Texture удалён. Запрос снова загрузит текстуру (кэш обновится)
    auto tex4 = cache.get("grass.png");
    tex4->use();

    // Запрос другой текстуры
    auto tex5 = cache.get("water.jpg");

    std::cout << "Cache size: " << cache.size() << '\n'; // 2

    return 0;
}
```
