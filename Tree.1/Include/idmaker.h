#ifndef __IDMAKER__
#define __IDMAKER__

#include <queue>
#include <string>
#include <fstream>
#include <stdexcept>
#include <filesystem>

template<typename data>
class idMaker{
	private:
		std::queue<data> Ids;
		std::string FileName;
		void produce_range(const data&, const data&, std::queue<data>&);
	public:
		idMaker(const std::string& _DirName){
			std::filesystem::create_directories(_DirName);
			FileName = _DirName + "idmaker.dat";
			std::ifstream File(FileName);
			if(File){
				int Id;
				while(File >> Id){
					Ids.push(Id);
				}
				File.close();
			}
		}
		~idMaker(){
			std::ofstream File(FileName);
			while(!Ids.empty()){
				File << Ids.front() << ' ';
				Ids.pop();
			}
			File.close();
		}
		void put_new_range(const data&, const data&);
		data get_id();
};
template<typename data>
void idMaker<data>::produce_range(const data& First, const data& End, std::queue<data>& Output){
	data Midle = (First + End) / 2;
	if(First == End){ return;}
	Output.push(Midle);
	produce_range(First , Midle , Output);
	produce_range(Midle + 1, End, Output);
}
template<typename data>
void idMaker<data>::put_new_range(const data& First, const data& End){
	while(!Ids.empty()){
		Ids.pop();
	}
	produce_range(First, End, Ids);
}
template<typename data>
data idMaker<data>::get_id(){
	if(Ids.empty()){
		throw std::runtime_error("Err in 'idMaker<T>::get_id' queue is empty");
	}
	data Output = Ids.front();
	Ids.pop();
	return Output;
}
#endif
