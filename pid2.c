#include <iostream>

using namespace std;
double getSum(double x)
{
 return x;
}
template<class... Args>
double getSum(double x, Args... args)
{ return x + getSum(args...);
}


void print() // ничего не делает
{}
template <class First, class... Other>
void print(const First& first, const Other&... other)
{
 cout << first;
 print(other...);
}
template <class... Args>
void println(const Args&... args)
{
 print(args...);
 cout << endl;
}
struct Point
{
 int x;
 int y;
 friend std::ostream& operator<<(std::ostream& output,
 const Point& p)
 { output << "(" << p.x << "," << p.y << ")";
 return output;
 }
};
int main()
{
 println("Hello, world");
 println("Pi = ", 3.14, '\n', 2, " * ", 2, " = ", 2 * 2);

 Point point{ 12,2 };
 println("Point: ", point);

 return 0;
}

int main_2222()
{
 double sum = getSum(1, 2, 3, 4, 5);
 cout << "sum = " << sum << endl;
 sum = getSum(25, 0, 5, 60);
 cout << "sum = " << sum << endl;
 
 return 0;
}