#include <iostream>
#include "hsmap.h"
using namespace std;

int main(){


	char Input = ' ';
	hsMap<int, char> Map;
	while(Input != 'f'){
		cin >> Input;
		switch(Input){
		case 'i':{
			int Id;
			char Data;
			cin >> Id >> Data;
			Map.insert({Id, Data});
		break;}case 'e':{
			int Id;
			cin >> Id;
			Map.erase(Id);
		break;}case 'r':{
			int Id;
			char Data;
			cin >> Id >> Data;
			Map[Id] = Data;
		break;}case 'd':{
			for(pair<int, char> Input : Map){
				cout << "Key: " << Input.first << "Data: " << Input.second << ' ';
			}
			cout << '\n';
		break;}case 's':{
			int Id;
			cin >> Id;
			auto It = Map.find(Id);
			cout << "Key: " << (*It).first << "Data: " << (*It).second << '\n';
		break;}case 'm':{
			int MapId;
			cin >> MapId;
			Map.make_id(MapId);
		break;} case 'c':{
			int MapId;
			cin >> MapId;
			Map.change_id(MapId);
		break;}
		}
	}
}
