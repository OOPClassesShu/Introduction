```
#include <iostream>
#include <memory>
#include <string>

class Large {
    int id;
    std::string name;
public:
    Large(int i, const std::string& n) : id(i), name(n) {
        std::cout << "Large(" << id << ", " << name << ") created\n";
    }
    ~Large() {
        std::cout << "Large(" << id << ") destroyed\n";
    }
    void print() const {
        std::cout << "Large: id=" << id << ", name=" << name << '\n';
    }
};

// Функция, возвращающая unique_ptr (передаёт владение вызывающему)
std::unique_ptr<Large> create_large_object(int id, const std::string& name) {
    // make_unique предпочтительнее new
    return std::make_unique<Large>(id, name);
}

// Функция, которая забирает владение (принимает unique_ptr по значению)
void take_ownership(std::unique_ptr<Large> ptr) {
    if (ptr) {
        std::cout << "Inside take_ownership: ";
        ptr->print();
    } // здесь ptr разрушается, объект удаляется
}

// Функция, которая только наблюдает (сырой указатель)
void observe(Large* raw_ptr) {
    if (raw_ptr) {
        std::cout << "Inside observe: ";
        raw_ptr->print();
    } else {
        std::cout << "observe: null pointer\n";
    }
}

// Альтернатива – наблюдение через ссылку
void observe_ref(const Large& ref) {
    std::cout << "Inside observe_ref: ";
    ref.print();
}

int main() {
    // Создаём unique_ptr
    auto ptr = create_large_object(1, "Example");

    // Наблюдение без передачи владения
    observe(ptr.get());          // через сырой указатель
    observe_ref(*ptr);           // через ссылку

    // Передаём владение функции take_ownership
    take_ownership(std::move(ptr));   // ptr становится пустым

    // После передачи ptr больше не владеет объектом
    if (!ptr) {
        std::cout << "ptr is now empty\n";
    }

    // Попытка наблюдать после передачи – безопасно, но объект уже удалён
    observe(ptr.get());   // выведет "observe: null pointer"

    return 0;
}
```
