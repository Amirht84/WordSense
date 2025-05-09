#ifndef __HSMAP__
#define __HSMAP__

#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "hardmap.h"
template <typename id , typename data>
class hsMap{
	private:
		const char YES = 'Y';
		const char NO = 'N';
		#pragma pack(push, 1)
		struct hardNode{
			char IsUsed;
			long long Parent;
			id Id;
			data Data;
			long long RightOffset;
			long long LeftOffset; 	
		};
		#pragma pack(pop)
		struct firstKey{
			int Size;
			long long Head;
		};

		struct dataProxy{
			std::fstream* File;
			long long CurrentPos;
			hardNode Current;
			bool IsWrite;
			dataProxy(std::fstream& _File, const long long& _Pos, const hardNode& _Node):
				File(&_File),
				CurrentPos(_Pos),
				Current(_Node), 
				IsWrite(false){}

			dataProxy& operator=(const data& Data){
				Current.Data = Data;
				IsWrite = true;
				return *this;
			}
			operator data(){
				return Current.Data;	
			}
			~dataProxy(){
				if(IsWrite){
					File->seekp(CurrentPos);
					File->write(reinterpret_cast<char*>(&Current), sizeof(hardNode));
					if(!(File)){
						throw std::runtime_error("Err in 'hsMap::dataProxy::~dataProxy()' can't write");
					}		
				}
			}
		};
	
		int Size;
		long long Head;
		std::fstream DataFile;
		hardMap<int, firstKey> Keys;
		typename hardMap<int , firstKey>::iterator FirstKey;
		long long find_free_pos();
		bool write_key(const firstKey& Key);
		bool write_file(const long long& Pos, const hardNode& Input);
		bool read_file(const long long& Pos, hardNode& Output);
		long long search(const long long& CurrentPos, const id& Id);
		void put(const long long& , hardNode);
		long long find_down_less(long long);
		long long find_down_more(long long);
		long long find_up_less(long long);
		long long find_up_more(long long);
		long long successor(const long long& Pos);
		long long predecessor(const long long& CurrentPos);
	public:
		hsMap(const std::string& _DirName): Size(0), Head(-1), Keys(_DirName + "keys/"){
			std::filesystem::create_directories(_DirName);
			const std::string FileName = _DirName + "data.dat";
			DataFile.open(FileName, std::ios::in | std::ios::out | std::ios::binary );
			if(!DataFile.is_open()){
				std::ofstream CreateFile(FileName , std::ios::binary);
				CreateFile.close();
				DataFile.open(FileName, std::ios::in | std::ios::out | std::ios::binary);
				if(!DataFile.is_open()) throw std::runtime_error("err in 'hsMap<id, data>::hsMap'");
			}
		}
		dataProxy operator[](const id& Id){
			const std::string ErrLog = "err in 'hsMap<id, data>::operator[]'";
			const auto CurrentPos = search(Head, Id);
			if(CurrentPos == -1) throw std::runtime_error(ErrLog + "invalid Id");
			hardNode Current;
			if(!read_file(CurrentPos, Current)) throw std::runtime_error(ErrLog + "can't read");
			return dataProxy(DataFile, CurrentPos, Current);
		}
		class iterator;
		iterator find(const id& Id){
			return iterator(*this, search(Head, Id));
		}
		iterator begin(){
			if(Size == 0) return iterator(*this, -1);

			long long CurrentPos = Head;
			long long PredecessorPos = predecessor(CurrentPos);
			while(PredecessorPos != -1){
				CurrentPos = PredecessorPos;
				PredecessorPos = predecessor(CurrentPos);
			}
			return iterator(*this , CurrentPos);
		}
		iterator end(){return iterator(*this, -1);}
		void erase(const id& Id);
		void insert(const std::pair<id, data>& Input);
		void change_id(const int&);
		void make_id(const int&);
		~hsMap(){
			if(!write_key({Size , Head})) throw std::runtime_error("err in 'hsMap<id, data>::~hsMap'");
			DataFile.close();
		}
};
template<typename id, typename data>
void hsMap<id, data>::make_id(const int& MapId){
	FirstKey = Keys.find(MapId);
	if(FirstKey != Keys.end()) std::runtime_error("err 'inhsMap<id, data>::make_id'");
	Keys.insert({MapId, {0 , -1}});
}
void make_id(const int&);
template<typename id, typename data>
void hsMap<id, data>::change_id(const int& MapId){
	FirstKey = Keys.find(MapId);
	if(FirstKey == Keys.end()) throw std::runtime_error("err in 'hsMap<id, data>::change_id'");
	firstKey Temp = (*FirstKey).second;
	Size = Temp.Size;
	Head = Temp.Head;
}
template <typename id, typename data>
void hsMap<id , data>::erase(const id& Id){
	const std::string ErrLog = "err in hsMap<id, data>::erase";
	const long long ErasePos = search(Head, Id);

	if(ErasePos == -1){
		 throw std::runtime_error(ErrLog + "Invalid Id");
	}
	hardNode EraseNode;

	if(!read_file(ErasePos ,EraseNode)) throw std::runtime_error(ErrLog + "can't read");

	auto manage_parent = [this, &ErrLog, &EraseNode, &ErasePos](const long long& NewChild){
		if(EraseNode.Parent != -1){
			hardNode ParentNode;
			if(!read_file(EraseNode.Parent, ParentNode)) throw std::runtime_error(ErrLog + "can't read");
			if(ParentNode.RightOffset == ErasePos){
				ParentNode.RightOffset = NewChild;
			}else{
				ParentNode.LeftOffset = NewChild;
			}
			if(!write_file(EraseNode.Parent, ParentNode)) throw std::runtime_error(ErrLog + "can't write");
		}else{
			Head = NewChild;
		}
	};

	if(EraseNode.RightOffset == -1 && EraseNode.LeftOffset == -1){
		manage_parent(-1);
	}else if(EraseNode.RightOffset == -1 || EraseNode.LeftOffset == -1){
		long long ChildPos;
		if(EraseNode.RightOffset != -1){
			ChildPos = EraseNode.RightOffset;
		}else{
			ChildPos = EraseNode.LeftOffset;
		}

		manage_parent(ChildPos);

		hardNode ChildNode;
		if(!read_file(ChildPos, ChildNode)) throw std::runtime_error(ErrLog + "can't read");
		ChildNode.Parent = EraseNode.Parent;
		if(!write_file(ChildPos, ChildNode)) throw std::runtime_error(ErrLog + "can't write");
	}else{
		const long long SuccessorPos = find_down_more(ErasePos);
		hardNode SuccessorNode;
		if(!read_file(SuccessorPos, SuccessorNode)) throw std::runtime_error(ErrLog + "can't read");

		manage_parent(SuccessorPos);
	
		if(SuccessorPos == EraseNode.RightOffset){
			SuccessorNode.LeftOffset = EraseNode.LeftOffset;
			hardNode ChildNode;
			if(!read_file(EraseNode.LeftOffset, ChildNode)) throw std::runtime_error(ErrLog + "can't read");
			ChildNode.Parent = SuccessorPos;
			if(!write_file(EraseNode.LeftOffset, ChildNode)) throw std::runtime_error(ErrLog + "can't write");
		}else{
			if(SuccessorNode.RightOffset != -1){
				hardNode SuccessorChildNode;
				if(!read_file(SuccessorNode.RightOffset, SuccessorChildNode)) throw std::runtime_error(ErrLog + "can't read");
				SuccessorChildNode.Parent = SuccessorNode.Parent;
				if(!write_file(SuccessorNode.RightOffset, SuccessorChildNode)) throw std::runtime_error(ErrLog + "can't write");
			}
			hardNode SuccessorParentNode;
			if(!read_file(SuccessorNode.Parent, SuccessorParentNode)) throw std::runtime_error(ErrLog + "can't read");
			SuccessorParentNode.LeftOffset = SuccessorNode.RightOffset;
			if(!write_file(SuccessorNode.Parent, SuccessorParentNode)) throw std::runtime_error(ErrLog + "can't write");

			hardNode ChildNode;
			SuccessorNode.RightOffset = EraseNode.RightOffset;
			if(!read_file(EraseNode.RightOffset,ChildNode)) throw std::runtime_error(ErrLog + "can't read");
			ChildNode.Parent = SuccessorPos;
			if(!write_file(EraseNode.RightOffset, ChildNode)) throw std::runtime_error(ErrLog + "can't write");
			SuccessorNode.LeftOffset = EraseNode.LeftOffset;
			if(!read_file(EraseNode.LeftOffset, ChildNode)) throw std::runtime_error(ErrLog + "can't read");
			ChildNode.Parent = SuccessorPos;
			if(!write_file(EraseNode.LeftOffset, ChildNode)) throw std::runtime_error(ErrLog + "can't write");
		}

		SuccessorNode.Parent = EraseNode.Parent;
		
		if(!write_file(SuccessorPos, SuccessorNode)) throw std::runtime_error(ErrLog + "can't write");
	}

	EraseNode.LeftOffset = -1;
	EraseNode.RightOffset = -1;
	EraseNode.Parent = -1;
	EraseNode.IsUsed = NO;
	if(!write_file(ErasePos, EraseNode)) throw std::runtime_error(ErrLog + "can't write");
	
	--Size;
	if(!write_key({Size, Head})) throw std::runtime_error(ErrLog + "can't write");
}
template<typename id, typename data>
long long hsMap<id , data>::successor(const long long& CurrentPos){
	const std::string ErrLog = "err in 'hsMap<id, data>::successor'";
	hardNode CurrentNode;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	if(CurrentNode.RightOffset != -1){
		return find_down_more(CurrentPos);
	}else if(CurrentNode.Parent != -1){
		return find_up_more(CurrentPos);
	}else{
		return -1;
	}
}
template<typename id, typename data>
long long hsMap<id, data>::predecessor(const long long& CurrentPos){
	const std::string ErrLog = "err in 'hsMap<id, data>::predecessor'";
	hardNode CurrentNode;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	if(CurrentNode.LeftOffset != -1){
		return find_down_less(CurrentPos);
	}else if(CurrentNode.Parent != -1){
		return find_up_less(CurrentPos);
	}else{
		return -1;
	}
}

template <typename id , typename data>
void hsMap<id , data>::put(const long long& CurrentPos, hardNode New){
	const std::string ErrLog = "err in 'hsMap<id, data>::put'";
	hardNode Current;	
	if(!read_file(CurrentPos, Current)) throw std::runtime_error(ErrLog + "can't read");

	if(New.Id < Current.Id){
		if(Current.LeftOffset == -1){
			long long FreePos = find_free_pos();
			New.Parent = CurrentPos;
			if(!write_file(FreePos, New)) throw std::runtime_error(ErrLog + "can't write");
			Current.LeftOffset = FreePos;
			if(!write_file(CurrentPos, Current)) throw std::runtime_error(ErrLog + "can't write");
			return; 
		}else{
			put(Current.LeftOffset, New);
			return;
		}
	}else if(New.Id > Current.Id){
		if(Current.RightOffset == -1){
			long long FreePos = find_free_pos();
			New.Parent = CurrentPos;
			if(!write_file(FreePos, New)) throw std::runtime_error(ErrLog + "can't write");
			Current.RightOffset = FreePos;
			if(!write_file(CurrentPos, Current)) throw std::runtime_error(ErrLog + "can't write");
			return;
		}else{
			put(Current.RightOffset, New);
			return;
		}
	}
	throw std::runtime_error(ErrLog + " not logical act");
}
template <typename id , typename data>
void hsMap<id , data>::insert(const std::pair<id, data>& Input){
	const std::string ErrLog = "Err 'inhsMap<id, data>::insert'";
	if(Size != 0 && search(Head, Input.first) != -1){
		return;
	}
	hardNode New;
	New.IsUsed = YES;
	New.Parent = -1;
	New.RightOffset = -1;
	New.LeftOffset = -1;
	New.Id = Input.first;
	New.Data = Input.second;

	if(Size == 0){
		long long Offset = find_free_pos();
		if(!write_file(Offset, New)) throw std::runtime_error(ErrLog + "can't write");
		Head = Offset;
		Size = 1;
		if(!write_key({Size, Head})) throw std::runtime_error(ErrLog + "can't write");
	}else{
		put(Head, New);
		Size++;
		if(!write_key({Size, Head})) throw std::runtime_error(ErrLog + "can't write");
	}
}

template <typename id , typename data>
long long hsMap<id , data>::search(const long long& CurrentPos, const id& Id){
	const std::string ErrLog = "err 'inhsMap<id, data>::find_here'";
	hardNode Current;
	if(!read_file(CurrentPos, Current)) throw std::runtime_error(ErrLog + "can't read");
	if(Id == Current.Id){
		return CurrentPos;
	}else if(Id < Current.Id){
		if(Current.LeftOffset != -1){
			return search(Current.LeftOffset, Id);
		}else{
			return -1;
		}
	}else if(Id > Current.Id){
		if(Current.RightOffset != -1){
			return search(Current.RightOffset, Id);
		}else{
			return -1;
		}
	}
	throw std::runtime_error(ErrLog + " not logical act");
	return -1;
}

template <typename id , typename data>
bool hsMap<id , data>::read_file(const long long& Pos, hardNode& Output){
	DataFile.seekg(Pos);
	DataFile.read(reinterpret_cast<char*>(&Output) , sizeof(hardNode));
	if(!DataFile){
		return false;
	}
	return true;
}

template <typename id , typename data>
bool hsMap<id , data>::write_file(const long long& Pos, const hardNode& Input){
	DataFile.seekp(Pos);
	DataFile.write(reinterpret_cast<const char*>(&Input), sizeof(hardNode));
	if(!(DataFile)){
		return false;
	}
	return true;
}
template <typename id , typename data>
long long hsMap<id , data>::find_free_pos(){
	const std::string ErrLog = "Err 'inhsMap<id, data>:: find_free_pos'";
	DataFile.seekp(0 , std::ios::end);
	long long End = DataFile.tellp();	
	long long Offset = 0;
	char IsUsed;
	while(Offset != End){
		DataFile.seekg(Offset);
		DataFile.read(&IsUsed , 1);
		if(!DataFile) throw std::runtime_error(ErrLog + " can't read file");
		if(IsUsed == NO){
			return Offset;
		}
		Offset += sizeof(hardNode);
	}
	return Offset;
}

template <typename id , typename data>
bool hsMap<id , data>::write_key(const firstKey& Key){
	(*FirstKey).second = Key;
	return true;
}
template <typename id,typename data>
long long hsMap<id, data>::find_down_less(long long CurrentPos){
	const std::string ErrLog = "err in 'hsMap<id, data>::find_down_less'";
	hardNode CurrentNode;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	if(CurrentNode.LeftOffset == -1) throw std::runtime_error(ErrLog + "LeftOffset is -1");
	CurrentPos = CurrentNode.LeftOffset;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	while(CurrentNode.RightOffset != -1){
		CurrentPos = CurrentNode.RightOffset;
		if(!read_file(CurrentPos, CurrentNode)) std::runtime_error(ErrLog + "can't read");
	}
	return CurrentPos;
}

template <typename id,typename data>
long long hsMap<id, data>::find_down_more(long long CurrentPos){
	const std::string ErrLog = "err in 'hsMap<id, data>::find_down_more'";
	hardNode CurrentNode;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	if(CurrentNode.RightOffset == -1) throw std::runtime_error(ErrLog + "RightOffset is -1");
	CurrentPos = CurrentNode.RightOffset;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	while(CurrentNode.LeftOffset != -1){
		CurrentPos = CurrentNode.LeftOffset;
		if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error (ErrLog + "can't read");
	}
	return CurrentPos;
}

template <typename id,typename data>
long long hsMap<id, data>::find_up_less(long long CurrentPos){
	const std::string ErrLog = "err in <find_up_less>";
	hardNode CurrentNode;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	
	long long ChildPos = CurrentPos;
	CurrentPos = CurrentNode.Parent;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	while(ChildPos != CurrentNode.RightOffset){
		if(CurrentNode.Parent == -1) return -1;
		ChildPos = CurrentPos;
		CurrentPos = CurrentNode.Parent;
		if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	}
	return CurrentPos;
}

template <typename id,typename data>
long long hsMap<id, data>::find_up_more(long long CurrentPos){
	const std::string ErrLog = "err in <find_up_more>";
	hardNode CurrentNode;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	long long ChildPos = CurrentPos;
	CurrentPos = CurrentNode.Parent;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	while(ChildPos != CurrentNode.LeftOffset){
		if(CurrentNode.Parent == -1) return -1;
		ChildPos = CurrentPos;
		CurrentPos = CurrentNode.Parent;
		if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	}
	return CurrentPos;
}
template<typename id, typename data>
class hsMap<id , data>::iterator{
	private:
		hsMap<id , data>* Owner;
		long long CurrentPos;
	public:
		iterator():Owner(nullptr), CurrentPos(-1){}
		iterator(hsMap& _Owner, const long long& _CurrentPos): Owner(&_Owner), CurrentPos(_CurrentPos) {}
		iterator(const iterator& _Other): Owner(_Other.Owner), CurrentPos(_Other.CurrentPos){}
		std::pair<id, dataProxy> operator*(){
			hardNode CurrentNode;
			if(!(Owner->read_file(CurrentPos, CurrentNode)))
				throw std::runtime_error("err in 'hsMap<id, data>::iterator::operator*' can't read");

			return std::pair<id, dataProxy>(CurrentNode.Id, dataProxy(Owner->DataFile, CurrentPos, CurrentNode));
		}
		iterator& operator++(){
			CurrentPos = (Owner->successor(CurrentPos));
			return *this;
		}
		iterator& operator++(int){
			auto Temp = *this;
			CurrentPos = (Owner->successor(CurrentPos));
			return Temp;
		}
		bool operator==(const iterator& _Other){
			return (_Other.CurrentPos == CurrentPos);
		}
		bool operator!=(const iterator& _Other){
			return !(*this == _Other);
		}
};

#endif
