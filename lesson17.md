[В оглавление](README.md)

# Занятие 16

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
