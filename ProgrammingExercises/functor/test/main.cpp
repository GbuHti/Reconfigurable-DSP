//声明一个函数指针
//本例中 int(*)(int,int)作为一种函数类型，声明了一个函数指针fp;
//既然是类型，即可作为函数模板的模板参数，提高函数模板的通用性；
//Yu Zhenbo  
//2020-02-26 18:00:55 
#include <iostream>

using namespace std;
int add(int a, int b)
{
	return a + b;
}

int main()
{
	int(*fp)(int,int) = add;
	cout << fp(1,3) << endl;
	return 0;
}


