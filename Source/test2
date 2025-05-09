#include "hardmap.h"
#include <iostream>
using namespace std;

int main() {
	hardMap<int, char> map("mydata.bin");
	map.insert(4, 'a');
	map.insert(3, 'b');
	map.insert(2, 'c');
	map[4] = 'l';

	map.erase(4);

	for(char It : map){
		cout << char(It) << '\n';
	}
	return 0;
}
