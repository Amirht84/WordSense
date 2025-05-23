#include <iostream>
#include <set>

int main(){
	std::set<int> Set;
	int Input;
	while(std::cin >> Input){
		Set.insert(Input);
	}
	for(auto It : Set){
		std::cout << "-------------------------------\n";
		std::cout << It << ' ';
	}
}
