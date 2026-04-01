
[В оглавление](README.md)
# Занятие 7

# Динамические структуры данных

В программировании структуры данных играют ключевую роль в организации и управлении информацией. \
В зависимости от задач используются различные способы хранения и доступа к данным. 

## Статические массивы
Статический массив – это набор элементов фиксированного размера, память под который выделяется на этапе компиляции (в стеке или в статической области).
```
int arr[10];          // размер известен во время компиляции
arr[0] = 5;
```
- Размер задаётся константой и не может быть изменён во время выполнения.
- Выделение и освобождение памяти автоматическое.
- Быстрый доступ по индексу O(1).
- Нельзя изменить размер после создания.

## Динамические массивы

Динамический массив создаётся в куче (heap) с помощью операторов new[] / delete[] или через контейнер std::vector. Размер может определяться во время выполнения.

```
int size;
std::cin >> size;
int* dynArr = new int[size];   // выделение памяти
dynArr[0] = 10;
delete[] dynArr;               // обязательное освобождение
```
**Преимущества:**

- Гибкость размера (можно создать массив нужного размера в runtime).
- Возможность перераспределения (например, как в std::vector).

**Недостатки:**

- Доступ по индексу всё ещё O(1), но добавление/удаление элементов может требовать копирования всего массива (O(n)).

## Однонаправленные списки

Односвязный список состоит из узлов, каждый из которых содержит данные и указатель на следующий узел. Доступ к элементам последовательный.

```
template<typename T>
struct Node {
    T data;
    Node* next;
    Node(const T& val) : data(val), next(nullptr) {}
};

template<typename T>
class SinglyLinkedList {
private:
    Node<T>* head;
public:
    SinglyLinkedList() : head(nullptr) {}

    ~SinglyLinkedList() {
        while (head) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Добавление в начало
    void pushFront(const T& value) {
        Node<T>* newNode = new Node<T>(value);
        newNode->next = head;
        head = newNode;
    }

    // Добавление в конец
    void pushBack(const T& value) {
        Node<T>* newNode = new Node<T>(value);
        if (!head) {
            head = newNode;
            return;
        }
        Node<T>* current = head;
        while (current->next)
            current = current->next;
        current->next = newNode;
    }

    // Удаление первого элемента
    void popFront() {
        if (head) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Поиск элемента
    bool find(const T& value) const {
        Node<T>* current = head;
        while (current) {
            if (current->data == value) return true;
            current = current->next;
        }
        return false;
    }

    // Вывод списка
    void print() const {
        Node<T>* current = head;
        while (current) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};
```
**Сложности операций:**

- Вставка/удаление в начале: O(1)
- Вставка/удаление в конце: O(n) без указателя на хвост
- Поиск: O(n)

