#include "avlset.h"
#include <iostream>

int main(){
	avlSet<int> Set;
	int Value;
	while(std::cin >> Value){
		Set.insert(Value);
	}
}
