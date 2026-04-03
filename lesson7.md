
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
<img width="973" height="231" alt="image" src="https://github.com/user-attachments/assets/e941920e-70f2-4903-a6b0-1ea39f698fb5" />


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

**Удаление элемента**

<img width="1024" height="767" alt="image" src="https://github.com/user-attachments/assets/b30089e4-5b2e-4705-a5be-587a41ecd47c" />

**Добавление элемента в середину**

<img width="1024" height="767" alt="image" src="https://github.com/user-attachments/assets/04aefcf1-8bd5-4e6c-8123-cecebecf585a" />

**Добавление элемента в начало**
<img width="1024" height="767" alt="image" src="https://github.com/user-attachments/assets/44f6465a-ba3a-48c8-b8bf-4c76660651f9" />

## Двунаправленные списки 

В двусвязном списке каждый узел содержит указатели на следующий и предыдущий элементы. Это позволяет эффективно перемещаться в обе стороны и удалять произвольный узел, \
если известен указатель на него.

```
template<typename T>
struct DNode {
    T data;
    DNode* prev;
    DNode* next;
    DNode(const T& val) : data(val), prev(nullptr), next(nullptr) {}
};

template<typename T>
class DoublyLinkedList {
private:
    DNode<T>* head;
    DNode<T>* tail;
public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}

    ~DoublyLinkedList() {
        while (head) {
            DNode<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void pushFront(const T& value) {
        DNode<T>* newNode = new DNode<T>(value);
        if (!head) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
    }

    void pushBack(const T& value) {
        DNode<T>* newNode = new DNode<T>(value);
        if (!tail) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    void popFront() {
        if (head) {
            DNode<T>* temp = head;
            head = head->next;
            if (head) head->prev = nullptr;
            else tail = nullptr;
            delete temp;
        }
    }

    void popBack() {
        if (tail) {
            DNode<T>* temp = tail;
            tail = tail->prev;
            if (tail) tail->next = nullptr;
            else head = nullptr;
            delete temp;
        }
    }

    void printForward() const {
        DNode<T>* current = head;
        while (current) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }

    void printBackward() const {
        DNode<T>* current = tail;
        while (current) {
            std::cout << current->data << " ";
            current = current->prev;
        }
        std::cout << std::endl;
    }
};
```
Преимущества перед односвязным списком:

- Удаление узла по указателю O(1) (если известен сам узел).
- Возможность обхода в обратном направлении.
- Легче реализовать операции в конце списка.

## Стек (Stack)

Стек – это структура данных, работающая по принципу LIFO (Last In, First Out). Последний добавленный элемент извлекается первым. \

Основные операции:

- push – добавить элемент на вершину.
- pop – удалить элемент с вершины.
- top – получить значение вершины без удаления.

Возможно реализовать:
- на основе массива (динамического/статического)
- на основе односвязного списка
  
Сравнение реализаций:
- На массиве – быстрый доступ, но может требовать перераспределения, если размер стека не ограничен.
- На списке – нет ограничения на размер, но каждый элемент требует дополнительной памяти на указатель.
  
## Очередь (Queue)

Очередь – структура данных FIFO (First In, First Out). Первый добавленный элемент извлекается первым. \

Основные операции:
- enqueue – добавить элемент в конец.
- dequeue – удалить элемент из начала.
- front – получить первый элемент.

Рализовать так же можно используя массив и список.


## Бинарные деревья

Бинарное дерево — это иерархическая структура данных, в которой каждый узел имеет не более двух потомков: левый и правый. \
Бинарное дерево может быть:
- Пустым (nullptr).
- Состоять из корня (root) и двух непересекающихся бинарных деревьев — левого и правого поддеревьев.

Термины:

- Корень — вершина, не имеющая родителя.
- Лист — узел, у которого оба потомка отсутствуют.
- Внутренний узел — узел, имеющий хотя бы одного потомка.
- Высота дерева — максимальное количество рёбер от корня до листа.
- Глубина узла — количество рёбер от корня до данного узла.

Узел
```
template<typename T>
struct TreeNode {
    T data;
    TreeNode* left;
    TreeNode* right;
    TreeNode(const T& val) : data(val), left(nullptr), right(nullptr) {}
};
```

Бинарное дерево поиска (Binary Search Tree) — это бинарное дерево, удовлетворяющее свойству упорядоченности:

- все ключи в левом поддереве меньше x.data,
- все ключи в правом поддереве больше x.data.

Это свойство позволяет эффективно искать, вставлять и удалять элементы за O(log n) при сбалансированности.

Операции поиска и добавления простые и мы их описывать не будем. Разберем случаи, возникющие при удалении. \
Три ситуации:
- Удаляемый узел — лист (нет детей). Просто удаляем его, родителю ставим nullptr.
  <img width="365" height="232" alt="image" src="https://github.com/user-attachments/assets/471bff0b-63bf-4d79-9dd3-874976a25e65" />

- Узел имеет одного ребёнка. «Переподвешиваем» ребёнка на место удаляемого узла.
  <img width="363" height="370" alt="image" src="https://github.com/user-attachments/assets/18ce5461-02b7-4ddd-8d2e-7db2e52ca7c2" />

- Узел имеет двух детей. Находим преемника (самый левый узел в правом поддереве) или предшественника (самый правый в левом поддереве).
  Копируем его значение в удаляемый узел, затем рекурсивно удаляем преемника (у него не может быть левого ребёнка, т.е. он попадает под случай 1 или 2).
  <img width="620" height="216" alt="image" src="https://github.com/user-attachments/assets/b1c04484-1856-4013-af75-2672dad211a9" />
---

  <img width="623" height="290" alt="image" src="https://github.com/user-attachments/assets/d2fcfce8-c410-41e2-bd22-ac02faec26c4" />

---
  <img width="635" height="380" alt="image" src="https://github.com/user-attachments/assets/2ca14c1a-f077-4100-8e60-aee2253d6c45" />

---
  


### Обходы бинарного дерева:
- в глубину
- в ширину

### Сбалансированные бинарные деревья

Сбалансированное дерево — дерево, у которого высота левого и правого поддеревьев для любого узла отличается не более чем на некоторую константу (или строго логарифмически ограничена). \
Высота такого дерева всегда O(logn). \

**AVL-дерево (Адельсон-Вельский и Ландис, 1962)**

Для каждого узла модуль разницы высот левого и правого поддеревьев ≤ 1. \
Поддержание баланса после вставки и удаления выполняется с помощью поворотов:
- Правый поворот (Right rotation) — для лево-левого случая.
- Левый поворот (Left rotation) — для право-правого случая.
- Левый-правый поворот (Left-Right rotation) — сначала левый поворот левого ребёнка, затем правый поворот.
- Правый-левый поворот (Right-Left rotation) — симметрично.

Удаление в AVL-дереве выполняется аналогично BST, но после каждого удаления вызывается balance для всех затронутых узлов.

**Красно-чёрное дерево**

Свойства:
- Каждый узел красный или чёрный.
- Корень чёрный.
- Листья (NIL) чёрные.
- У красного узла оба ребёнка чёрные.

Для любого узла все пути от него до листьев содержат одинаковое количество чёрных узлов (чёрная высота). \
Балансировка менее строгая, чем в AVL: высота не более  2log (n+1). \
Это даёт более быстрые повороты при вставке/удалении (меньше перестроений). Используется в std::map, std::set.




