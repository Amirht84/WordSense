#include <iostream>
#include <random>

int main(){
	std::random_device RandomDevice;
	std::mt19937 Gen(RandomDevice());
	std::uniform_int_distribution<int> Range(10, 100);
	std::uniform_int_distribution<int> Input(-100, 100);

	int Roof = Range(Gen);
	for(int i = 0 ; i < Roof ; i++){
		std::cout << Input(Gen) << '\n';
	}
	return 0;
}
