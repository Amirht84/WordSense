#include <set>
#include <random>
#include <iostream>
#include "hsmap.h"

int main(){
	hsMap<int, char> Test("Test/");
	std::set<int> MapIds;

	std::random_device RandomDevice;
	std::mt19937 Gen(RandomDevice());
	std::uniform_int_distribution<> MapIdMaker(1, 1000);
	std::uniform_int_distribution<> IdMaker(1, 100);
	std::uniform_int_distribution<char> DataMaker('a', 'z');
	std::uniform_int_distribution<> WayMaker(1, 3);
	std::uniform_int_distribution<> LoopMaker(100, 1000);
	std::uniform_int_distribution<> LoopInsertMaker(10 , 25);


	int LoopsCount = LoopMaker(Gen);
	std::cout << "User is going to do " << LoopsCount << "actions\n";
	for(int i = 0 ; i < LoopsCount ; i++){
		switch(WayMaker(Gen)){
			case 1:{
				int InsertCount = LoopInsertMaker(Gen);
				for(int j = 0 ; j < InsertCount ; j++ ){
					if(MapIds.empty()){
						break;
					}else{
						int Id = IdMaker(Gen);
						char Data = DataMaker(Gen);
						Test.insert({Id, Data});
						std::cout << "Insert {" << Id << ',' << Data << '}' << '\n';
					}
				}
			break;}case 2:{
				std::cout << "Get display\n";
				for(auto It : Test){
					int temp1 = It.first;
					char temp2 = It.second;
				}
			break;}case 3:{
				int MapId = MapIdMaker(Gen);
				if(MapIds.find(MapId) != MapIds.end()){
					std::cout << "Change id to " << MapId << '\n';
					Test.change_id(MapId);
				}else{
					std::cout << "Make id as " << MapId << '\n';
					Test.make_id(MapId);
					MapIds.insert(MapId);
					std::cout << "Change id to " << MapId << " after making\n";
					Test.change_id(MapId);
				}
			break;}
		}
	}
	return 0;
}
