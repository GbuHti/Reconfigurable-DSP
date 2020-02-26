// functor.cpp -- using a functor
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>

template <class T> //functor class defines operator()()
class TooBig
{
	private:
		T cutoff;
	public:
		TooBig(const T &t): cutoff(t){};
		bool operator()(const T &v){ return v > cutoff;}
};

bool TooBig2(int v, int cutoff)
{
	return v > cutoff;
}

template <class T>  //相当于一个接口，使得已有函数（TooBig2)能够满足不同的接口（remove_if要求一个谓词参数）
class TooBig2_if
{
	private:
		T cutoff;
	public:
		TooBig2_if(const T& t):cutoff(t){};
		bool operator()(const T &v){ return TooBig2(v, cutoff);}
	
};

void outint(int n) {std::cout << n << " ";}



int main()
{
	using std::list;
	using std::cout;
	using std::endl;

	TooBig<int> f100(100); // limit = 100
	TooBig2_if<int> f50(50);
	int vals[10] = {49, 100, 90, 180, 22, 210, 415, 88, 188, 201};
	list<int> yadayada(vals, vals + 10); //range constructor
	list<int> etcetera(vals, vals + 10); 
	list<int> yzb(vals, vals + 10);
	cout << "Original lists: \n";
	for_each(yadayada.begin(), yadayada.end(), outint);
	cout << endl;
	for_each(etcetera.begin(), etcetera.end(), outint);
	cout << endl;
	yadayada.remove_if(f100);	//use a neamed function object
	etcetera.remove_if(TooBig<int>(200)); //construct a function object
	yzb.remove_if(f50);
	cout << "Trimmed lists:\n";
	for_each(yadayada.begin(), yadayada.end(), outint);
	cout << endl;
	for_each(etcetera.begin(), etcetera.end(), outint);
	cout << endl;
	for_each(yzb.begin(), yzb.end(), outint);
	cout << endl;
	
	return 0;
}
