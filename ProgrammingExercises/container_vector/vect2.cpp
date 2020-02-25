/* STL容器--vector类
 * vector类一些方法的使用示例
 * 来源：C Primer Plus<第6版> P679
 * Yu Zhenbo  
 * 2020-02-25 15:54:55 
 */
#include <iostream>
#include <string>
#include <vector>

struct Review{
	std::string title;
	int rating;
};
bool FillReview(Review &rr);
void ShowReview(const Review &rr);

int main()
{
	using std::cout;
	using std::vector;
	vector<Review> books;
	Review temp;
	while(FillReview(temp))
		books.push_back(temp);	
	int num = books.size();
	if(num > 0)
	{
		cout << "Thank you. You entered the following:\n"
			<< "Rating\tBook\n";
		for(int i = 0; i < num; i++)
			ShowReview(books[i]);	
		cout << "Reprising:\n"
			<< "Rating\tBook\n";
		vector<Review>::iterator pr;
		for (pr = books.begin(); pr != books.end(); pr++ )
			ShowReview(*pr);
		vector <Review> oldlist(books); // copy constructor used
		if (num >3)
		{
			//remove 2 items	
			books.erase(books.begin() + 1, books.begin() + 3);
			cout << "After erasure:\n";
			for (pr = books.begin(); pr != books.end(); pr++)
				ShowReview(*pr);
			//insert 1 item
			books.insert(books.begin(), oldlist.begin() + 1, oldlist.begin() + 2);
			cout << "After insertion:\n";
			for (pr = books.begin(); pr!= books.end(); pr++)
				ShowReview(*pr);
		}
		books.swap(oldlist);
		cout << "Swapping oldlist with books:\n";
		for (pr = books.begin(); pr != books.end(); pr++)
			ShowReview(*pr);
	}
	else
		cout << "Nothing entered, nothing gained.\n";
	return 0;
}

bool FillReview(Review & rr)
{
	std::cout << "Enter book title (quit to quit): ";
	std::getline(std::cin, rr.title);
	if(rr.title == "quit")
		return false;
	std::cout << "Enter book rating";
	std::cin >> rr.rating;
	if(!std::cin)
		return false;
	//get rid of rest of input line
	while(std::cin.get() != '\n')
		continue;
	return true;
}

void ShowReview(const Review &rr)
{
	std::cout << rr.rating << "\t" << rr.title << std::endl;
}