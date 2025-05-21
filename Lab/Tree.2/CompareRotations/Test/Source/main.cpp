#include "hardmap.h"
#include <iostream>

int main(){
	hardMap<int, char> Test("Test/");
	int Input;
	while(std::cin >> Input){
		Test.insert({Input, 'i'});
	}
	return 0;
}

