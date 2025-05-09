#include "hardmap.h"
#include <iostream>
using namespace std;

int main() {
	hardMap<int, char> map("Files/test.hm");
	char Input;
	hardMap<int, char>::iterator It;
	bool Exit = true;
	while(Exit){
	cin >> Input;
		switch(Input){
			case 'i':{
				int Key;
				char Val;
				cin >> Key >> Val;
				map.insert(Key, Val);
			break;}case 'e':{
				int Key;
				cin >> Key;
				map.erase(Key);
			break;}case 's':{
				for(const char& Val : map){
					cout << Val << ' ';
				}
				cout << '\n';
			break;}case 'w':{
				int Id;
				cin >> Id;
				It = map.find(Id);
				cout << *It << '\n';
			break;}case 'f':{
				Exit = false;
			break;}
		}
	}
	cout << *It << '\n';
	return 0;
}

