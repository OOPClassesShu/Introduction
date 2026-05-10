[В оглавление](README.md)

# Занятие 17

# Немного про многопоточное программирование

Многопоточное программирование — это способ организации вычислительного процесса, при котором в рамках одной программы одновременно выполняются \
несколько потоков управления (threads). Каждый поток имеет собственный стек и регистровый контекст, но разделяет с другими \
потоками память процесса (кучу, глобальные переменные, дескрипторы файлов). \
Такая модель позволяет:
- Эффективно использовать многоядерные процессоры — разные потоки могут выполняться на разных ядрах одновременно.
- Сохранять отзывчивость приложений — например, интерфейс пользователя остаётся активным, пока фоновый поток выполняет длительную операцию.
- Упрощать моделирование одновременных событий — сервер, обрабатывающий запросы нескольких клиентов.

Cложности:

- Состояние гонки (data race) — неопределённое поведение при одновременном доступе к общим данным без синхронизации.
- Взаимные блокировки (deadlocks) — когда два потока ждут друг друга, удерживая ресурсы.
- Сложность отладки — ошибки могут проявляться недетерминированно.

# Класс std::thread

std::thread — это обёртка над системным потоком, предоставляющая платформонезависимый интерфейс для создания, управления и синхронизации потоков.

Поток создаётся передачей в конструктор вызываемого объекта (callable object):

- Функция (глобальная или статическая)
- Лямбда-выражение
- Функтор (объект класса с перегруженным operator())
- Указатель на метод класса (требует дополнительного аргумента – экземпляра или указателя)

```
#include <iostream>
#include <thread>

void hello() {
    std::cout << "Привет из потока!" << std::endl;
}

int main() {
    std::thread t(hello);  // создаём поток, он сразу начинает выполняться
    t.join();              // ожидаем завершения
    return 0;


    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";
    
    // Например, для инициализации пула потоков
    // unsigned int thread_pool_size = std::thread::hardware_concurrency();

}
```
## Ожидание завершения: join()

join() блокирует текущий поток до тех пор, пока поток, связанный с объектом std::thread, не завершится. \
После вызова join() объект std::thread больше не представляет действительный поток (становится неприсоединяемым). \
Повторный вызов join() или detach() приведёт к std::system_error.

```
std::thread t([] { /* работа */ });
t.join(); // блокируемся, пока поток не закончит
// здесь поток гарантированно завершён
```
## Отсоединение потока: detach()

detach() отделяет объект std::thread от реального потока. \ 
Поток продолжает выполняться в фоновом режиме, а объект std::thread теряет связь с ним. \ 
Освобождение ресурсов потока происходит автоматически при его завершении.

```
std::thread t([] { std::this_thread::sleep_for(std::chrono::seconds(1)); });
t.detach();
// основная программа может завершиться, оставив фоновый поток выполняться, но
// если main() завершится, все потоки, включая отсоединённые, будут принудительно убиты.
```

## Проверка, может ли поток быть присоединён: joinable()

Метод joinable() возвращает true, если объект std::thread ассоциирован с выполняющимся потоком \
(т.е. не был join() или detach()). Полезно для проверки перед вызовом join().
```
std::thread t([] {});
if (t.joinable()) t.join();
```

## Идентификатор потока: get_id()

Каждый поток имеет уникальный объект std::thread::id, который можно получить через get_id(). 
Также есть std::this_thread::get_id() для текущего потока.

```
std::thread t([] {
    std::cout << "ID потока: " << std::this_thread::get_id() << std::endl;
});
std::cout << "ID объекта t: " << t.get_id() << std::endl;
t.join();

```

# Гонка данных (data race)

Когда несколько потоков одновременно обращаются к одной переменной, и хотя бы один поток изменяет её, возникает неопределённое \ 
поведение – программа может выдать неверный результат, зависнуть или упасть. Это называется гонкой данных.

Пример
```
#include <iostream>
#include <thread>
#include <vector>

int counter = 0;

void increment() {
    for (int i = 0; i < 100000; ++i) {
        ++counter; // НЕ атомарно!
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(increment);
    }
    for (auto& t : threads) t.join();
    std::cout << counter << std::endl; // Ожидаем 1'000'000, но получаем меньше (например, 834721)
}
```

Операция ++counter компилируется в три машинные инструкции:
- Загрузить значение counter в регистр.
- Увеличить регистр.
- Сохранить результат обратно в counter.

Потоки могут прерываться между этими шагами, и один поток прочитает значение до того, как другой успеет его сохранить. В результате часть инкрементов «теряется».


## std::mutex – базовый мьютекс

Мьютекс (mutual exclusion) – объект, который может находиться в двух состояниях: заблокирован (locked) или разблокирован (unlocked). Поток вызывает lock():

- Если мьютекс свободен, поток захватывает его и продолжает выполнение.
- Если мьютекс уже захвачен другим потоком, поток блокируется (засыпает) до тех пор, пока мьютекс не освободится.

После выполнения критической секции поток вызывает unlock(), освобождая мьютекс, и один из ожидающих потоков просыпается.

```
#include <mutex>

std::mutex mtx;
int counter = 0;

void safe_increment() {
    for (int i = 0; i < 100000; ++i) {
        mtx.lock();
        ++counter;
        mtx.unlock();
    }
}
```

Если внутри критической секции возникнет исключение, unlock() не вызовется, и мьютекс останется заблокирован \ 
навсегда – все остальные потоки, пытающиеся его захватить, зависнут. \ 
Даже если исключения нет, легко забыть написать unlock(), особенно при ранних return.

```
void dangerous() {
    mtx.lock();
    if (some_condition) {
        // забыли mtx.unlock() перед return!
        return;
    }
    mtx.unlock();
}
```
Для решения таких проблем используюут RAII-обёртки


## std::lock_guard – самая простая обёртка

```
void safe_increment() {
    for (int i = 0; i < 100000; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        ++counter;
    } // lock уничтожается, mtx.unlock() вызывается автоматически
}
```

Особенности:

- Конструктор lock_guard вызывает mtx.lock().
- Деструктор вызывает mtx.unlock().
- Копирование и перемещение запрещены.
- Нельзя досрочно освободить мьютекс – только по выходу из области видимости.

## std::unique_lock – гибкая обёртка

unique_lock имеет тот же интерфейс RAII, но предоставляет дополнительные возможности:

- Отложенная блокировка (создаётся без захвата).
- Ручное управление – lock(), unlock(), try_lock().
- Возможность перемещать владение блокировкой.
- Условные переменные требуют unique_lock.

```
std::unique_lock<std::mutex> lock(mtx, std::defer_lock); // не захватывает сразу
// ... выполняем подготовительные действия без блокировки
lock.lock(); // теперь захватили
// ... критическая секция
lock.unlock(); // можно освободить досрочно
// ... что-то ещё без блокировки
lock.lock(); // снова захватили
// lock сам разблокирует в деструкторе
```

try_lock
```
std::unique_lock<std::mutex> lock(mtx, std::try_to_lock);
if (lock.owns_lock()) {
    // мьютекс захвачен, можно работать
} else {
    // не удалось – делаем что-то другое
}
```

Перемещение блокировки
```
std::unique_lock<std::mutex> lock1(mtx);
std::unique_lock<std::mutex> lock2 = std::move(lock1); // теперь lock2 владеет, lock1 пуст
```

## std::scoped_lock – множественная блокировка

Позволяет безопасно захватить несколько мьютексов одновременно, избегая взаимных блокировок (deadlock). \
Использует алгоритм, предотвращающий дедлок (например, захват в порядке возрастания адресов или через std::lock).

```
std::mutex m1, m2;

void transfer(int& a, int& b, int amount) {
    std::scoped_lock lock(m1, m2); // захватывает оба мьютекса
    a -= amount;
    b += amount;
} // оба мьютекса освобождаются автоматически
```
В отличие от lock_guard, scoped_lock может работать с любым количеством мьютексов 
(даже с одним – тогда он аналогичен lock_guard).

Deadlock возникает, когда два (или более) потока удерживают мьютексы и ждут друг друга.

```
std::mutex mtxA, mtxB;

// Поток 1
void foo() {
    std::lock_guard<std::mutex> lockA(mtxA);
    std::this_thread::sleep_for(10ms);
    std::lock_guard<std::mutex> lockB(mtxB); // ждёт mtxB
}

// Поток 2
void bar() {
    std::lock_guard<std::mutex> lockB(mtxB);
    std::this_thread::sleep_for(10ms);
    std::lock_guard<std::mutex> lockA(mtxA); // ждёт mtxA
}
```

```
struct Account {
    std::mutex mtx;
    int balance = 0;
};

void transfer(Account& from, Account& to, int amount) {
    // Сначала блокируем счёт from, потом to
    std::lock_guard<std::mutex> lock_from(from.mtx);
    std::lock_guard<std::mutex> lock_to(to.mtx);
    
    from.balance -= amount;
    to.balance += amount;
}
```

Кажется, всё хорошо. Но что, если два потока одновременно вызывают transfer(a, b, 100) и transfer(b, a, 100)?

- Поток 1: захватывает a.mtx → затем пытается захватить b.mtx.
- Поток 2: захватывает b.mtx → затем пытается захватить a.mtx.

В этом случае каждый поток удерживает нужный другому мьютекс. Оба потока засыпают навечно – взаимная блокировка (deadlock).

```
/*
Теперь даже при одновременных встречных переводах deadlock не возникает.
scoped_lock гарантирует, что оба мьютекса будут захвачены без состояния гонки.
*/
void transfer(Account& from, Account& to, int amount) {
    // scoped_lock захватывает оба мьютекса безопасно
    std::scoped_lock lock(from.mtx, to.mtx);
    
    from.balance -= amount;
    to.balance += amount;
} // оба мьютекса автоматически освобождаются
```

## std::recursive_mutex

```
std::recursive_mutex rec_mtx;
void recurse(int depth) {
    std::lock_guard<std::recursive_mutex> lock(rec_mtx);
    if (depth > 0) recurse(depth - 1);
}
```

Обычно при рекурсивных вызовах функций, каждая из которых должна быть защищена мьютексом. Но во многих случаях можно реструктурировать код.

## std::timed_mutex

Позволяют попытаться захватить мьютекс с таймаутом.
```
std::timed_mutex tm;

if (tm.try_lock_for(std::chrono::milliseconds(100))) {
    // захватили
    tm.unlock();
} else {
    // не удалось за 100 мс
}
```

- try_lock_for(duration) – ожидает не более duration времени, затем возвращает false, если мьютекс не был захвачен.
- try_lock_until(time_point) – ожидает до абсолютного момента времени.
- После таймаута поток не блокируется и может выполнить полезную работу вместо ожидания.

timed_mutex может быть немного медленнее обычного mutex, поэтому используйте его только когда нужна функциональность таймаута.


## std::shared_mutex – для чтения/записи

Разделяет блокировки на два типа: разделяемые (shared) для множества читающих потоков и исключительные (unique) для одного пишущего. \
Специализированные обёртки: std::shared_lock и std::unique_lock (или lock_guard).

- std::shared_lock – RAII для разделяемой блокировки (аналог unique_lock, но с lock_shared() / unlock_shared()).
- std::shared_lock также поддерживает отложенную блокировку, try_lock, перемещение и т.д.




```
std::shared_mutex sm;

void reader() {
    std::shared_lock lock(sm); // разделяемая блокировка
    // чтение данных
}
void writer() {
    std::unique_lock lock(sm); // исключительная
    // запись
}
```

```
#include <iostream>
#include <shared_mutex>
#include <thread>
#include <vector>

class SharedCounter {
    int value = 0;
    mutable std::shared_mutex mtx;
public:
    void increment() {
        std::unique_lock lock(mtx);
        ++value;
    }
    int get() const {
        std::shared_lock lock(mtx);
        return value;
    }
};

int main() {
    SharedCounter c;
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&c] { for (int i = 0; i < 10000; ++i) c.increment(); });
    }
    for (auto& t : threads) t.join();
    std::cout << c.get() << std::endl; // 100000
}
```

## Условные переменные — std::condition_variable и std::condition_variable_any

Условные переменные — это примитив синхронизации, который позволяет одному или нескольким потокам приостановить выполнение \
до тех пор, пока не будет выполнено некоторое условие. 

Предположим, у нас есть очередь, в которую один поток (продюсер) помещает данные, а другой поток (консьюмер) забирает их. \
Консьюмеру нужно ждать, пока очередь не станет непустой. Наивный подход — активное ожидание (busy waiting) с периодической проверкой условия.

```
// Плохо: активное ожидание
while (queue.empty()) {
    // пустой цикл – бесполезная нагрузка на процессор
}
```

Этот подход потребляет 100% одного ядра процессора, не давая возможности другим потокам работать. \
Очевидное решение — блокирующее ожидание, при котором поток засыпает и просыпается только тогда, когда условие может стать истинным.

Условная переменная предоставляет именно такой механизм:

- поток засыпает (с отдачей процессора) в ожидании события;
- другой поток сигналит (notify) об изменении состояния;
- разбуженный поток проверяет условие и продолжает работу.

Для работы условной переменной необходимо три элемента:

- Мьютекс (std::mutex) – защищает разделяемые данные (например, очередь), которые проверяются и изменяются.
- Условная переменная (std::condition_variable или std::condition_variable_any) – предоставляет методы wait(), notify_one(), notify_all().
- Предикат – условие, которое проверяется (например, !queue.empty()). Обычно передается в wait().

Условная переменная всегда используется вместе с мьютексом, чтобы избежать гонок между проверкой условия и переходом в режим ожидания.

**Методы ожидания**

wait(unique_lock<mutex>& lock)
Атомарно:

- Освобождает мьютекс lock.
- Засыпает до тех пор, пока не будет разбужен вызовом notify_one() или notify_all().
- Проснувшись, снова захватывает мьютекс и возвращает управление.
  
После возврата нужно повторно проверить условие, так как возможны ложные пробуждения.


**Методы уведомления**

- notify_one() – будит один ожидающий поток (если есть).
- notify_all() – будит все ожидающие потоки.

Уведомления не сохраняются: если ни один поток не ждёт в момент вызова notify, сигнал теряется. \
Поэтому важно правильно организовывать синхронизацию — обычно сначала изменяют разделяемые данные, затем уведомляют.

**Требования к мьютексу**

std::condition_variable работает только с std::unique_lock<std::mutex>. \
Более гибкая версия — std::condition_variable_any (работает с любым BasicLockable), но она может быть медленнее.

Пример «Очередь производитель-потребитель» (один продюсер, один консьюмер)
```
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

std::queue<int> q;
std::mutex mtx;
std::condition_variable cv;

void producer() {
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        {
            std::lock_guard<std::mutex> lock(mtx);
            q.push(i);
            std::cout << "Produced: " << i << std::endl;
        }
        cv.notify_one(); // уведомляем потребителя
    }
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        // Ждём, пока очередь не станет непустой
        cv.wait(lock, []{ return !q.empty(); });
        int val = q.front();
        q.pop();
        lock.unlock(); // можно освободить раньше, чтобы не держать мьютекс при обработке
        std::cout << "Consumed: " << val << std::endl;
        if (val == 9) break; // условие остановки
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    return 0;
}
```

- Защита очереди: q защищена мьютексом mtx. Все операции с очередью (чтение, запись) выполняются под блокировкой.
- Почему unique_lock, а не lock_guard? wait() требует, чтобы мьютекс был захвачен через unique_lock, так как внутри wait() он освобождается и снова захватывается.
  lock_guard не поддерживает ручное управление.
- Предикат в wait() гарантирует, что после пробуждения условие !q.empty() истинно, и мы не выйдем из wait() по ложному пробуждению.
- Освобождение мьютекса перед обработкой (lock.unlock()) улучшает параллелизм: пока потребитель обрабатывает данные, продюсер может добавлять новые.

**notify_one() vs notify_all()**

notify_one() будит один случайный поток из ожидающих. Эффективно, когда каждый элемент может быть обработан одним потоком (например, очередь задач). Лишние пробуждения отсутствуют.

notify_all() будит все ожидающие потоки. Полезно, когда изменилось условие, важное для многих потоков (например, shutdown_flag = true).
notify_all() может вызвать «толчею» (thundering herd) — много потоков одновременно проснутся и будут конкурировать за мьютекс, что снижает производительность.



Реализация блокирующей очереди с двумя условными переменными (пусто/полно)
Если очередь ограничена по размеру, нужны две условные переменные: одна для «не пусто», другая для «не полно».

```
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>

// Шаблонный класс ограниченной (bounded) блокирующей очереди
template<typename T>
class BoundedBlockingQueue {
private:
    std::queue<T> queue_;
    size_t capacity_;
    mutable std::mutex mtx_;
    std::condition_variable not_empty_; // сигналит, когда очередь перестала быть пустой
    std::condition_variable not_full_;  // сигналит, когда очередь перестала быть полной

public:
    explicit BoundedBlockingQueue(size_t capacity) : capacity_(capacity) {}

    // Добавляет элемент в очередь. Если очередь заполнена – блокируется.
    void push(const T& value) {
        std::unique_lock<std::mutex> lock(mtx_);
        // Ждём, пока в очереди появится свободное место
        not_full_.wait(lock, [this]() {
            return queue_.size() < capacity_;
        });
        queue_.push(value);
        // После добавления очередь точно не пуста – уведомляем одного потребителя
        not_empty_.notify_one();
    }

    // Извлекает элемент из очереди. Если очередь пуста – блокируется.
    T pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        // Ждём, пока очередь не станет непустой
        not_empty_.wait(lock, [this]() {
            return !queue_.empty();
        });
        T value = queue_.front();
        queue_.pop();
        // После извлечения освободилось место – уведомляем одного производителя
        not_full_.notify_one();
        return value;
    }

    // Неблокирующая попытка извлечь элемент (опционально)
    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty()) return false;
        value = queue_.front();
        queue_.pop();
        not_full_.notify_one();
        return true;
    }

    // Текущий размер очереди (полезен для отладки, не для принятия решений)
    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.size();
    }
};

// ---------- Пример использования ----------

void producer(BoundedBlockingQueue<int>& q, int id, int items) {
    for (int i = 0; i < items; ++i) {
        int value = id * 100 + i;  // уникальное значение
        q.push(value);
        std::cout << "[Producer " << id << "] added " << value
                  << " | queue size = " << q.size() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // имитация работы
    }
    std::cout << "[Producer " << id << "] finished." << std::endl;
}

void consumer(BoundedBlockingQueue<int>& q, int id, int items) {
    for (int i = 0; i < items; ++i) {
        int value = q.pop();
        std::cout << "[Consumer " << id << "] got " << value
                  << " | queue size = " << q.size() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(80)); // обработка
    }
    std::cout << "[Consumer " << id << "] finished." << std::endl;
}

int main() {
    const size_t capacity = 3;
    BoundedBlockingQueue<int> queue(capacity);

    const int prod_count = 2;
    const int cons_count = 2;
    const int items_per_prod = 5;  // всего 2*5 = 10 элементов
    const int items_per_cons = 5;  // суммарно потребители заберут 10

    std::vector<std::thread> producers, consumers;

    for (int i = 0; i < prod_count; ++i) {
        producers.emplace_back(producer, std::ref(queue), i, items_per_prod);
    }
    for (int i = 0; i < cons_count; ++i) {
        consumers.emplace_back(consumer, std::ref(queue), i, items_per_cons);
    }

    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    std::cout << "All threads finished." << std::endl;
    return 0;
}
```

**Объяснение работы**

Зачем две условные переменные?

- not_empty_ — используется потребителями. Они ждут, когда очередь из пустой станет непустой.
- not_full_ — используется производителями. Они ждут, когда очередь из полной станет не полностью заполненной (появится свободное место).

Разделение сигналов позволяет:

- избегать ненужных пробуждений (например, после вставки будить только потребителей, а не производителей);
- сделать код логически ясным.

**Защита мьютексом**

Все обращения к queue_ (чтение, запись, проверка размера) выполняются под одним std::mutex. Это стандартный способ обеспечения потокобезопасности.

**Почему std::unique_lock, а не lock_guard?**

Методы wait() условной переменной требуют std::unique_lock, потому что внутри они освобождают мьютекс перед засыпанием и снова захватывают его перед возвратом. \
lock_guard не предоставляет операций lock()/unlock().

**Роль предиката в wait()**

```
not_full_.wait(lock, [this]() { return queue_.size() < capacity_; });
```

Это защита от ложных пробуждений (spurious wakeups) и от ситуаций, когда другой поток успел изменить условие после пробуждения. 

Предикат проверяется:
- перед засыпанием (если условие уже истинно, поток не засыпает);
- после каждого пробуждения (пока условие не станет истинным, цикл ожидания продолжается).

**notify_one() вместо notify_all()**

Мы используем notify_one(), потому что достаточно разбудить одного потребителя (или одного производителя). \
Это эффективнее: не создаётся «толчея» из множества потоков, конкурирующих за мьютекс.



## Сводно об примитивах синхронизации

### Мьютексы (взаимные исключения)

|Примитив|	Назначение|	Появился в|
|-|-|-|
|std::mutex|	Базовый мьютекс, нерекурсивный, не поддерживает таймаут|	C++11|
|std::recursive_mutex|	Рекурсивный – один поток может захватить многократно|	C++11|
|std::timed_mutex|	Поддерживает try_lock_for и try_lock_until|	C++11|
|std::recursive_timed_mutex|	Рекурсивный + таймаут|	C++11|
|std::shared_mutex|	Разделяемый (читатели/писатели)|	C++17|
|std::shared_timed_mutex|	Разделяемый + таймаут|	C++14|

**RAII-обёртки для управления блокировками**

|Обёртка|	Особенности|	Появился в|
|-|-|-|
|std::lock_guard|	Простая, только захват/освобождение в конструкторе/деструкторе|	C++11|
|std::unique_lock|	Гибкая – отложенный захват, перемещение, try_lock, ручное управление|	C++11|
|std::scoped_lock|	Захват нескольких мьютексов без deadlock (вариативная шаблонность)|	C++17|
|std::shared_lock|	Разделяемая блокировка для shared_mutex / shared_timed_mutex|	C++14|


### Условные переменные (condition variables)


|Примитив|	Назначение|	Появился в|
|-|-|-|
|std::condition_variable|	Работает с std::unique_lock<std::mutex>|	C++11|
|std::condition_variable_any|	Работает с любым типом|	C++11|


### Атомарные операции

|Примитив|	Назначение|	Появился в|
|-|-|-|
|std::atomic<T>|	Атомарная обёртка для любого тривиально копируемого типа|	C++11|
|std::atomic_flag|	Простейший атомарный флаг, гарантированно lock-free|	C++11|
|std::atomic_ref<T>| (C++20)	Атомарный доступ к уже существующему объекту|	C++20|

### Семафоры (C++20)

|Примитив|	Назначение|
|-|-|
|std::counting_semaphore<Max>|	Семафор с неотрицательным счётчиком, максимальное значение – шаблонный параметр|
|std::binary_semaphore|	Алиас counting_semaphore<1>|


### Барьеры (C++20)

|Примитив|	Назначение|
|-|-|
|std::latch|	Одноразовый барьер – счётчик уменьшается, потоки ждут, пока он не станет 0|
|std::barrier|	Многоразовый барьер – синхронизация группы потоков на каждой фазе|

### Future / Promise (высокоуровневые)

Хотя они относятся к асинхронным задачам, их тоже можно считать примитивами синхронизации (передача значения между потоками, ожидание результата).

- std::future / std::shared_future
- std::promise
- std::packaged_task
- std::async


----

# Служебное

```
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <deque>
#include <queue>
#include <random>


using namespace std;

int main1() {
	setlocale(LC_ALL, "Russian");

	cout << "Вычислительный процесс 1 начал работу" << endl;
	this_thread::sleep_for(chrono::milliseconds(1000));
	for (int i{}; i < 10; i++) {
		cout << "Вычислительный процесс 1 работает" << endl;
		this_thread::sleep_for(chrono::milliseconds(2000));
	}
	cout << "Вычислительный процесс 1 закончил работу" << endl;

	cout << "Вычислительный процесс 2 начал работу" << endl;
	this_thread::sleep_for(chrono::milliseconds(1000));
	for (int i{}; i < 10; i++) {
		cout << "Вычислительный процесс 2 работает" << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
	}
	cout << "Вычислительный процесс 2 закончил работу" << endl;

	return 0;
}


void proc1() {
	cout << "Вычислительный процесс 1 начал работу" << endl;
	this_thread::sleep_for(chrono::milliseconds(1000));
	for (int i{}; i < 10; i++) {
		cout << "Вычислительный процесс 1 работает поток " << this_thread::get_id() << endl;
		this_thread::sleep_for(chrono::milliseconds(2000));
	}
	cout << "Вычислительный процесс 1 закончил работу" << endl;

}

void proc2() {
	cout << "Вычислительный процесс 2 начал работу" << endl;
	this_thread::sleep_for(chrono::milliseconds(1000));
	for (int i{}; i < 10; i++) {
		cout << "Вычислительный процесс 2 работает поток " << this_thread::get_id() << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
	}
	cout << "Вычислительный процесс 2 закончил работу" << endl;
}


void proc3(int a, int b) {
	cout << "Вычислительный процесс 3 начал работу" << endl;
	this_thread::sleep_for(chrono::milliseconds(6000));
	for (int i{}; i < 10; i++) {
		cout << "Вычислительный процесс 3 работает поток " << this_thread::get_id() << endl;
		a += b;
		cout << "a+b = " << a << endl;
		this_thread::sleep_for(chrono::milliseconds(500));
	}
	cout << "Вычислительный процесс 3 закончил работу" << endl;
}

void proc4(int& a, int b) {
	cout << "Вычислительный процесс 3 начал работу" << endl;
	this_thread::sleep_for(chrono::milliseconds(6000));
	for (int i{}; i < 10; i++) {
		cout << "Вычислительный процесс 3 работает поток " << this_thread::get_id() << endl;
		a += b;
		cout << "a+b = " << a << endl;
		this_thread::sleep_for(chrono::milliseconds(500));
	}
	cout << "Вычислительный процесс 3 закончил работу" << endl;
}


int  proc5(int a, int b) {
	cout << "Вычислительный процесс 3 начал работу" << endl;
	this_thread::sleep_for(chrono::milliseconds(6000));
	for (int i{}; i < 10; i++) {
		cout << "Вычислительный процесс 3 работает поток " << this_thread::get_id() << endl;
		a += b;
		cout << "a+b = " << a << endl;
		this_thread::sleep_for(chrono::milliseconds(500));
	}
	cout << "Вычислительный процесс 3 закончил работу" << endl;
	return a;
}

int main2() {
	setlocale(LC_ALL, "Russian");
	cout << "Программа начала работу поток " << this_thread::get_id() << endl;

	thread th(proc1);

  
	//thread th1(proc3, 10 ,11);

	int a = 10;
	int b = 11;
	//thread th1(proc4, a, b);
	//thread th1(proc4, std::ref(a), b);

	int res;
	//thread th1([&res,a,b]() {
	//	res = proc5(a, b);
	//	});

	auto f = [&res, a, b]() { res = proc5(a, b); };
	thread th1(f);


	proc2();

	th.join();
	th1.join();

	//cout << "a = " << a<< endl;
	cout << "res = " << res << endl;
	cout << "Программа отработала поток " << this_thread::get_id() << endl;

	return 0;
}





class Test {


public:


	void proc1() {
		cout << "Вычислительный процесс 1 начал работу" << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
		for (int i{}; i < 10; i++) {
			cout << "Вычислительный процесс 1 работает поток " << this_thread::get_id() << endl;
			this_thread::sleep_for(chrono::milliseconds(2000));
		}
		cout << "Вычислительный процесс 1 закончил работу" << endl;

	}

	
	void proc3(int a, int b) {
		cout << "Вычислительный процесс 3 начал работу" << endl;
		this_thread::sleep_for(chrono::milliseconds(6000));
		for (int i{}; i < 10; i++) {
			cout << "Вычислительный процесс 3 работает поток " << this_thread::get_id() << endl;
			a += b;
			cout << "a+b = " << a << endl;
			this_thread::sleep_for(chrono::milliseconds(500));
		}
		cout << "Вычислительный процесс 3 закончил работу" << endl;
	}
		
	int  proc4(int a, int b) {
		cout << "Вычислительный процесс 3 начал работу" << endl;
		this_thread::sleep_for(chrono::milliseconds(6000));
		for (int i{}; i < 10; i++) {
			cout << "Вычислительный процесс 3 работает поток " << this_thread::get_id() << endl;
			a += b;
			cout << "a+b = " << a << endl;
			this_thread::sleep_for(chrono::milliseconds(500));
		}
		cout << "Вычислительный процесс 3 закончил работу" << endl;
		return a;
	}
};

int main3() {
	setlocale(LC_ALL, "Russian");
	cout << "Программа начала работу поток " << this_thread::get_id() << endl;

	/*Test test;
	int res;
	auto f = [&res, &test]() { res = test.proc4(2, 7); };
	thread th1(f);
	proc2();
	th1.join();*/


	/*Test test;
	int res;
	
	thread th1(&Test::proc1, test);
	proc2();
	th1.join();*/

	Test test;
	int res;

	thread th1(&Test::proc3, test,33,77);
	proc2();
	th1.join();


	return 0;
}


//mutex
// lock_guard

class STimer {
	chrono::time_point<chrono::steady_clock> start, end;
	chrono::duration<float> delt;
public:
	STimer() {
		start = chrono::high_resolution_clock::now();
	}

	~STimer() {
		end = chrono::high_resolution_clock::now();
		delt = end - start;
		float result = delt.count();
		cout << " Time for programm runing is " << result<< endl;

	}
};

mutex mtx;

void print(char ch) {

	
	//this_thread::sleep_for(chrono::milliseconds(2000));
	// lock_guard<mutex> lock(mtx);
	//mtx.lock();
	for (int i{}; i < 10; i++) {
		for (int i{}; i < 10; i++){
			cout << ch;
			this_thread::sleep_for(chrono::milliseconds(20));
		}
		cout << endl;
	}
	cout << endl;
	//mtx.unlock();
	//this_thread::sleep_for(chrono::milliseconds(2000));
}

int main4() {
	setlocale(LC_ALL, "Russian");
	cout << "Программа начала работу поток " << this_thread::get_id() << endl;

	//print('*');
	//print('%');
	
	thread t1(print,'*');
	thread t2(print, '%');

	t1.join();
	t2.join();

	return 0;
}



// deadlock

mutex mtx1;
mutex mtx2;

void print1() {

	mtx1.lock();

	this_thread::sleep_for(chrono::milliseconds(1));
	
	mtx2.lock();
	for (int i{}; i < 10; i++) {
		for (int i{}; i < 10; i++) {
			cout << '*';
			this_thread::sleep_for(chrono::milliseconds(20));
		}
		cout << endl;
	}
	cout << endl;
	mtx2.unlock();
	mtx1.unlock();
	
}

void print2() {

	mtx2.lock();

	this_thread::sleep_for(chrono::milliseconds(1));

	mtx1.lock();
	for (int i{}; i < 10; i++) {
		for (int i{}; i < 10; i++) {
			cout << '#';
			this_thread::sleep_for(chrono::milliseconds(20));
		}
		cout << endl;
	}
	cout << endl;
	mtx1.unlock();
	mtx2.unlock();

}

int main5() {
	setlocale(LC_ALL, "Russian");
	cout << "Программа начала работу поток " << this_thread::get_id() << endl;

	//print('*');
	//print('%');

	thread t1(print1);
	thread t2(print2);

	t1.join();
	t2.join();

	return 0;
}


mutex m;
recursive_mutex rm;

void func(int a) {

	rm.lock();
	cout << a << " ";
	this_thread::sleep_for(chrono::milliseconds(20));
	if (a == 1) {
		cout << endl;
		rm.unlock();
		return;
	}
	a--;
	func(a);
	rm.unlock();
}

int main6() {
	setlocale(LC_ALL, "Russian");
	cout << "Программа начала работу поток " << this_thread::get_id() << endl;

	

	thread t1(func, 10);
	thread t2(func, 20);

	t1.join();
	t2.join();

	//func(10);

	//unique_lock<mutex> ul(m, defer_lock);


	return 0;
}




template <typename T>
struct concurrent_queue {
	void push(T value) {
		std::lock_guard<std::mutex> lg(m);
		q.push_back(std::move(value));
	}

	bool empty() const {
		std::lock_guard<std::mutex> lg(m);
		return q.empty();
	}

	T pop() {
		if (empty()) {			
			return -1;
		}
		std::lock_guard<std::mutex> lg(m);
		T result = q.front();
		q.pop_front();
		return result;
	}
private:
	mutable std::mutex m;
	std::deque<T> q;
};


void add(concurrent_queue<int>&q, int start) {
	
	for (int i{ start }; i < 20; i++) {
		cout << "add " << this_thread::get_id() << endl;
		this_thread::sleep_for(chrono::milliseconds(200));
		q.push(i);
		this_thread::sleep_for(chrono::milliseconds(200));
	}

	cout << "stop " << this_thread::get_id() << endl;
}

void del(concurrent_queue<int>& q) {
	this_thread::sleep_for(chrono::milliseconds(550));
	while (!q.empty()) {
		cout << "del " << this_thread::get_id() << endl;
		this_thread::sleep_for(chrono::milliseconds(300));
		cout << q.pop() << " ";
		this_thread::sleep_for(chrono::milliseconds(300));
	}
	cout << "stop " << this_thread::get_id() << endl;
}



int main7() {
	setlocale(LC_ALL, "Russian");
	cout << "Программа начала работу поток " << this_thread::get_id() << endl;
	concurrent_queue<int> q;
	
		thread th1(add, std::ref(q),1);
		thread th2(del, std::ref(q));
		thread th3(del, std::ref(q));
		thread th4(del, std::ref(q));
		thread th5(del, std::ref(q));
		th1.join();
		th2.join();
		th3.join();
		th4.join();
		th5.join();
	return 0;
}






/*
std::mutex              g_lockprint;
std::mutex              g_lockqueue;
std::condition_variable g_queuecheck;
std::queue<int>         g_codes;
bool                    g_done;
bool                    g_notified;

void workerFunc(int id, std::mt19937& generator)
{
	// стартовое сообщение
	{
		std::unique_lock<std::mutex> locker(g_lockprint);
		std::cout << "[worker " << id << "]\trunning..." << std::endl;
	}
	// симуляция работы
	std::this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));
	// симуляция ошибки
	int errorcode = id * 100 + 1;
	{
		std::unique_lock<std::mutex> locker(g_lockprint);
		std::cout << "[worker " << id << "]\tan error occurred: " << errorcode << std::endl;
	}
	// сообщаем об ошибке
	{
		std::unique_lock<std::mutex> locker(g_lockqueue);
		g_codes.push(errorcode);
		g_notified = true;
		g_queuecheck.notify_one();
	}
}

void loggerFunc()
{
	// стартовое сообщение
	{
		std::unique_lock<std::mutex> locker(g_lockprint);
		std::cout << "[logger]\trunning..." << std::endl;
	}
	// до тех пор, пока не будет получен сигнал
	while (!g_done)
	{
		std::unique_lock<std::mutex> locker(g_lockqueue);
		//while (!g_notified) // от ложных пробуждений
		//	g_queuecheck.wait(locker);
		// 
		cout << "[logger] checked" << endl;
		g_queuecheck.wait(locker, [&]() {return !g_codes.empty(); });

		// если есть ошибки в очереди, обрабатывать их
		while (!g_codes.empty())
		{
			std::unique_lock<std::mutex> locker(g_lockprint);
			std::cout << "[logger]\tprocessing error:  " << g_codes.front() << std::endl;
			g_codes.pop();
		}
		g_notified = false;
	}
}

int main(){
	// инициализация генератора псевдо-случайных чисел
	std::mt19937 generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
	// запуск регистратора
	std::thread loggerThread(loggerFunc);
	// запуск рабочих
	std::vector<std::thread> threads;
	for (int i = 0; i < 5; ++i)
		threads.push_back(std::thread(workerFunc, i + 1, std::ref(generator)));
	for (auto& t : threads)
		t.join();
	// сообщаем регистратору о завершении и ожидаем его
	g_done = true;
	loggerThread.join();
	return 0;
}


*/


/*
std::condition_variable
wait - поток начинает спать
notify_one - пробуждает один поток
notify_all - пробуждает все потоки

*/
template <typename T>
struct concurrent_queue1 {
	void push(T value) {
		std::unique_lock<std::mutex> lg(m);
		q.push_back(std::move(value));
		lg.unlock();
		cv.notify_all();
	}

	bool empty() const {
		std::lock_guard<std::mutex> lg(m);
		return q.empty();
	}

	T pop() {
		if (empty()) {
			cv.notify_all();
			return -1;
		}
		std::unique_lock<std::mutex> lg(m);
		T result = q.front();
		q.pop_front();
		cv.wait(lg);
		return result;
	}
private:
	mutable std::mutex m;
	std::deque<T> q;
	std::condition_variable cv;
};

void add1(concurrent_queue1<int>& q, int start) {

	for (int i{ start }; i < 10; i++) {
		this_thread::sleep_for(chrono::milliseconds(200));
		cout << "add " << i << "  " << this_thread::get_id() << endl;
		this_thread::sleep_for(chrono::milliseconds(200));
		q.push(i);
	}
	cout << "stop " << this_thread::get_id() << endl;
}

void del1(concurrent_queue1<int>& q) {
	this_thread::sleep_for(chrono::milliseconds(550));
	while (!q.empty()) {
		cout << "del " << this_thread::get_id() << endl;
		this_thread::sleep_for(chrono::milliseconds(400));

		cout << q.pop() << " ";
		this_thread::sleep_for(chrono::milliseconds(400));
	}
	cout << "stop " << this_thread::get_id() << endl;
}
```

```
#include <iostream>
#include <thread>
#include <chrono>

// Callback-функция, которая принимает результат
void onComplete(int result, const std::string& message) {
    std::cout << "Callback: result = " << result << ", message = " << message << std::endl;
}

// Функция, выполняющая длительную работу, затем вызывающая callback
void longTask(int durationSec, void(*callback)(int, const std::string&)) {
    std::this_thread::sleep_for(std::chrono::seconds(durationSec));
    int res = 42;
    std::string msg = "Task finished";
    callback(res, msg);
}

int main() {
    std::string externalMsg = "Hello from main";  // локальная переменная, которая умрёт при выходе из main
    // Запускаем поток, передавая callback и дополнительные данные (здесь нет захвата externalMsg, но проблема в долгом сне)
    std::thread worker(longTask, 3, onComplete);
    worker.detach();   // поток продолжает работать в фоне

    // Основной поток завершается через 1 секунду, а worker будет спать 3 секунды
    // Когда worker вызовет callback, main уже завершится, но программа не упадёт,
    // так как callback не использует данные из main. Однако если бы использовал - была бы катастрофа.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Main finished" << std::endl;
    // Здесь программа завершится, поток worker будет принудительно убит ОС
    // (не успеет вызвать callback)
}
```

```
#include <iostream>
#include <thread>
#include <fstream>

void logCallback(const std::string& message) {
    std::ofstream file("log.txt", std::ios::app);
    file << message << std::endl;
}

void backgroundTask(void(*callback)(const std::string&)) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    callback("Task completed");
}

int main() {
    std::thread t(backgroundTask, logCallback);
    t.detach();
    std::this_thread::sleep_for(std::chrono::seconds(2)); // даём время на выполнение
    std::cout << "Main exits" << std::endl;
    // Здесь мы дождались, но если бы не задержка, лог мог бы не записаться.
}
```


```

/*
В театральной кассе продаются билеты. 
Работает три кассы. 
Первый кассир обслуживает одного покупателя за 2 мин., 
второй – за 2,5 мин., 
третий – за 2,8 мин. 
Всего надо обслужить 250 человек. 
Через каждые 45 мин. 
кассиры закрывают кассу на 10 минутный технический перерыв. 
Время, за которое были обслужены все покупатели и количество покупателей, 
обслуженные каждым кассиром.
*/

#include <iostream>
#include <thread>
#include <chrono>

int customers = 250;
int count = 0;

void cashier(int id, double time_per_customer) {
    double total_work = 0.0;  // накопленное время работы в минутах
    const double work_interval = 45.0;  // минут до перерыва
    const double break_duration = 10.0; // минут перерыва

    int i = 0;
    for ( ;count < customers; ++i, ++count) {
        // Имитируем обслуживание одного покупателя
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(time_per_customer * 100)));

        total_work += time_per_customer;
        // Проверяем, не пора ли на перерыв
        if (total_work >= work_interval) {
            std::cout << "Кассир " << id << " уходит на перерыв (обслужено " << i+1 << " покупателей)\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(break_duration * 100)));
            total_work = 0.0; // сбрасываем счётчик после перерыва
        }
    }
    std::cout << "Кассир " << id << " завершил работу. Всего обслужено: " << i+1 << "\n";
}

int main() {
    std::thread t1(cashier, 1, 2.0);   // 2 мин на покупателя
    std::thread t2(cashier, 2, 2.5);   // 2.5 мин
    std::thread t3(cashier, 3, 2.8);   // 2.8 мин
    t1.join();
    t2.join();
    t3.join();
    std::cout << "Все покупатели обслужены!\n";
    return 0;
}

```
