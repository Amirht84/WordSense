#ifndef __HARDTREE__
#define __HARDTREE__

#include "hsmap.h"
#include "idmaker.h"
#include <filesystem>
#include <vector>

template<typename data>
class hardTree{
	private:
		const char YES = 'Y';
		const char NO = 'N';
		#pragma pack(push, 1)
		struct hardNode{
			char IsUsed;
			long long Parent;
			data Data;
			int ChildsMapId;
		};
		#pragma pack(pop)
		long long RootAdd;
		hsMap<data, long long> Adds;
		idMaker<int> Ids;
		long long ThisPlace;
		std::fstream File;
		long long find_free_pos();
		bool read_file(const long long&, hardNode&);
		bool write_file(const long long&, const hardNode&);
	public:
		hardTree(const std::string& _DirName): Adds(_DirName + "Adds/"), Ids(_DirName + "Ids/"){
			std::filesystem::create_directories(_DirName);
			const std::string FileName = _DirName + "hardtree.data";
			File.open(FileName, std::ios::in | std::ios::out | std::ios::binary);
			if(!File){
				std::fstream CreateFile(FileName, std::ios::out | std::ios::binary);
				CreateFile.close();
				File.open(FileName, std::ios::in | std::ios::out | std::ios::binary);
				if(!File) throw std::runtime_error("err in  hardTree<data>::hardTree invalid file address");

				RootAdd = sizeof(RootAdd);
				File.write(reinterpret_cast<char*>(&RootAdd), sizeof(RootAdd));
				hardNode Root;
				Root.ChildsMapId = 0;
				Root.IsUsed = YES;
				Root.Parent = -1;
				File.seekp(RootAdd);
				File.write(reinterpret_cast<char*>(&Root), sizeof(Root));
			}else{
				File.seekg(0);
				File.read(reinterpret_cast<char*>(&RootAdd), sizeof(RootAdd));
			}
			ThisPlace = RootAdd;
		}
		hardTree(const std::string& _DirName, const int& Range): Adds(_DirName + "Adds/"), Ids(_DirName + "Ids/"){
			std::filesystem::create_directories(_DirName);
			const std::string FileName = _DirName + "hardtree.data";
			File.open(FileName, std::ios::in | std::ios::out | std::ios::binary);
			if(!File){
				std::fstream CreateFile(FileName, std::ios::out | std::ios::binary);
				CreateFile.close();
				File.open(FileName, std::ios::in | std::ios::out | std::ios::binary);
				if(!File) throw std::runtime_error("err in  hardTree<data>::hardTree invalid file address");

				RootAdd = sizeof(RootAdd);
				File.write(reinterpret_cast<char*>(&RootAdd), sizeof(RootAdd));
				hardNode Root;
				Root.ChildsMapId = 0;
				Root.IsUsed = YES;
				Root.Parent = -1;
				File.seekp(RootAdd);
				File.write(reinterpret_cast<char*>(&Root), sizeof(Root));
			}else{
				File.seekg(0);
				File.read(reinterpret_cast<char*>(&RootAdd), sizeof(RootAdd));
			}
			ThisPlace = RootAdd;

			Ids.put_new_range(1 , Range);
		}
		void mkdir(const data& Data);
		void cd(const data& Data){
			hardNode CurrentNode;
			if(!read_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in  hardTree<data>::cd can't read");
			Adds.change_id(CurrentNode.ChildsMapId);
			ThisPlace = Adds[Data];
		}
		void cd_up(){
			hardNode CurrentNode;
			if(!read_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in  hardTree<data>::cd can't read");
			ThisPlace = CurrentNode.Parent;	
		}
		std::vector<data> ls(){
			std::vector<data> Output;
			hardNode CurrentNode;
			if(!read_file(ThisPlace ,CurrentNode)) throw std::runtime_error("err in  hardTree<data>::ls can't read");
			if(CurrentNode.ChildsMapId != 0){
				Adds.change_id(CurrentNode.ChildsMapId);
				for(auto It = Adds.begin(); It != Adds.end() ; ++It){
					Output.push_back((*It).first);
				}
			}
			return Output;
		}
};
template<typename data>
long long hardTree<data>::find_free_pos(){
	File.seekp(0 , std::ios::end);
	long long End = File.tellp();
	long long Offset = sizeof(long long);
	char IsUsed;
	while(Offset != End){
		File.seekg(Offset);
		File.read(&IsUsed , 1);
		if(!File) throw std::runtime_error("error in hardTree<data>::find_free_pos can't read");
		if(IsUsed == NO){
			return Offset;
		}
		Offset += sizeof(hardNode);
	}
	return Offset;
}
template<typename data>
bool hardTree<data>::read_file(const long long& Pos, hardNode& Output){
	File.seekg(Pos);
	File.read(reinterpret_cast<char*>(&Output) , sizeof(hardNode));
	if(!File){
		return false;
	}
	return true;
}

template <typename data>
bool hardTree<data>::write_file(const long long& Pos, const hardNode& Input){
	File.seekp(Pos);
	File.write(reinterpret_cast<const char*>(&Input), sizeof(hardNode));
	if(!(File)){
		return false;
	}
	return true;
}

template <typename data>
void hardTree<data>::mkdir(const data& Data){
	hardNode CurrentNode;
	if(!read_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in  hardTree<data>::mkdir can't read");
	if(CurrentNode.ChildsMapId == 0){
		int Id = Ids.get_id();
		Adds.make_id(Id);
		Adds.change_id(Id);
		CurrentNode.ChildsMapId = Id;
		if(!write_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in  hardTree<data>::mkdir can't write");
	}else{
		Adds.change_id(CurrentNode.ChildsMapId);
	}
	long long NewPos = find_free_pos();
	Adds.insert({Data, NewPos});
	hardNode New;
	New.IsUsed = YES;
	New.ChildsMapId = 0;
	New.Parent = ThisPlace;
	New.Data = Data;
	if(!write_file(NewPos, New)) throw std::runtime_error("err in  hardTree<data>::mkdir can't write");
}
#endif
