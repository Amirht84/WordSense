#ifndef __HARDMAP__
#define __HARDMAP__

#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <stdexcept>
#include <filesystem>


template <typename id , typename data>
class hardMap {
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
		struct firstKey{
			int Size;
			long long Head;
		};
		#pragma pack(pop)
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
					if(!(File)) throw std::runtime_error("Err in hardMap<id , data>::dataProxy");
				}
			}
		};
		
		int Size;
		long long Head;
		std::fstream File;
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

		hardMap(const std::string& _DirName){
			std::filesystem::create_directories(_DirName);
			const std::string FileName = _DirName + "hardmap.data";
			File.open( FileName, std::ios::in | std::ios::out | std::ios::binary );
			if(!File.is_open()){
				std::ofstream CreateFile(FileName , std::ios::binary);
				CreateFile.close();
				File.open(FileName, std::ios::in | std::ios::out | std::ios::binary);

				firstKey Key = {0 , -1};
				File.write(reinterpret_cast<char*>(&Key), sizeof(firstKey));
				if(!File) throw std::runtime_error("Err in hardMap<id , data>::hardMap can't read");
				Size = Key.Size;
				Head = Key.Head;
			}else{
				File.seekg(0);
				firstKey Key;
				File.read(reinterpret_cast<char*>(&Key), sizeof(firstKey));
				if(!File) throw std::runtime_error("Err in hardMap<id , data>::hardMap can't write");
				Size = Key.Size;
				Head = Key.Head;
			}
		}
		dataProxy operator[](const id& Id){
			const std::string ErrLog = "err in hardMap<id , data>::operator[]";
			const auto CurrentPos = search(Head, Id);
			if(CurrentPos == -1 ) throw std::runtime_error(ErrLog + "invalid Id");
			hardNode Current;
			if(!read_file(CurrentPos, Current)) throw std::runtime_error(ErrLog + "can't read");
			return dataProxy(File, CurrentPos, Current);
		}

		class iterator;
		iterator begin(){
			if(Size == 0) return iterator(*this, -1);
			long long CurrentPos = Head;
			long long PredecessorPos = predecessor(CurrentPos);
			while(PredecessorPos != -1){
				CurrentPos = PredecessorPos;
				PredecessorPos = predecessor(CurrentPos);
			}
			return iterator(*this, CurrentPos);
		}
		iterator end(){return iterator(*this, -1);}
		iterator find(const id& Id){return iterator(*this, search(Head, Id));}
		void erase(const id& Id);
		void insert(const std::pair<id, data>&);

		~hardMap(){
			if(!write_key({Size , Head})) throw std::runtime_error( "err in hardMap<id , data>::~hardMap");
			File.close();
		}
};

template <typename id, typename data>
void hardMap<id , data>::erase(const id& Id){
	const std::string ErrLog = "err in hardMap<id , data>::erase";

	
	const long long ErasePos = search(Head, Id);
	if(ErasePos == -1) throw std::runtime_error(ErrLog + "Invalid Id");
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
long long hardMap<id , data>::successor(const long long& CurrentPos){
	const std::string ErrLog = "err in hardMap<id , data>::successor";
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
long long hardMap<id, data>::predecessor(const long long& CurrentPos){
	const std::string ErrLog = "err in hardMap<id , data>::predecessor";
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
void hardMap<id , data>::put(const long long& CurrentPos, hardNode New){
	const std::string ErrLog = "err in hardMap<id , data>::put";
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
void hardMap<id , data>::insert(const std::pair<id , data>& Input){
	const std::string ErrLog = "Err in hardMap<id , data>::insert";
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
		long long Offset = sizeof(firstKey);
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
long long hardMap<id , data>::search(const long long& CurrentPos, const id& Id){
	const std::string ErrLog = "err in hardMap<id , data>::search";
	if(Size == 0){
		return -1;
	}
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
bool hardMap<id , data>::read_file(const long long& Pos, hardNode& Output){
	File.seekg(Pos);
	File.read(reinterpret_cast<char*>(&Output) , sizeof(hardNode));
	if(!File){
		return false;
	}
	return true;
}

template <typename id , typename data>
bool hardMap<id , data>::write_file(const long long& Pos, const hardNode& Input){
	File.seekp(Pos);
	File.write(reinterpret_cast<const char*>(&Input), sizeof(hardNode));
	if(!(File)){
		return false;
	}
	return true;
}
template <typename id , typename data>
long long hardMap<id , data>::find_free_pos(){
	const std::string ErrLog = "Err in hardMap<id , data>::find_free_pos";
	File.seekp(0 , std::ios::end);
	long long End = File.tellp();	
	long long Offset = sizeof(firstKey);
	char IsUsed;
	while(true){
		File.seekg(Offset);
		File.read(&IsUsed , 1);
		if(!File ) throw std::runtime_error(ErrLog + "can't read file");
		if(IsUsed == NO){
			return Offset;
		}
		Offset += sizeof(hardNode);
		if(Offset == End){
			break;
		}
	}
	return Offset;
}

template <typename id , typename data>
bool hardMap<id , data>::write_key(const firstKey& Key){
	File.seekp(0);
	File.write(reinterpret_cast<const char*>(&Key) , sizeof(firstKey));
	if(!File){
		return false;
	}
	return true;
}
template <typename id,typename data>
long long hardMap<id, data>::find_down_less(long long CurrentPos){
	const std::string ErrLog = "err in hardMap<id , data>::find_down_less";
	hardNode CurrentNode;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	if(CurrentNode.LeftOffset == -1) throw std::runtime_error(ErrLog + "LeftOfset is -1");
	CurrentPos = CurrentNode.LeftOffset;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read_file");
	while(CurrentNode.RightOffset != -1){
		CurrentPos = CurrentNode.RightOffset;
		if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read_file");
	}
	return CurrentPos;
}

template <typename id,typename data>
long long hardMap<id, data>::find_down_more(long long CurrentPos){
	const std::string ErrLog = "err in hardMap<id , data>::find_down_more";
	hardNode CurrentNode;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read_file");
	if(CurrentNode.RightOffset == -1) throw std::runtime_error(ErrLog + "RightOffset is -1");
	CurrentPos = CurrentNode.RightOffset;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	while(CurrentNode.LeftOffset != -1){
		CurrentPos = CurrentNode.LeftOffset;
		if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
	}
	return CurrentPos;
}

template <typename id,typename data>
long long hardMap<id, data>::find_up_less(long long CurrentPos){
	const std::string ErrLog = "err in hardMap<id , data>::find_up_less";
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
long long hardMap<id, data>::find_up_more(long long CurrentPos){
	const std::string ErrLog = "err in hardMap<id , data>::find_up_more";
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
class hardMap<id , data>::iterator{
	private:
		hardMap<id , data>* Owner;
		long long CurrentPos;
	public:
		iterator():Owner(nullptr), CurrentPos(-1){}
		iterator(hardMap& _Owner, const long long& _CurrentPos): Owner(&_Owner), CurrentPos(_CurrentPos) {}
		iterator(const iterator& _Other): Owner(_Other.Owner), CurrentPos(_Other.CurrentPos){}
		std::pair<const id, dataProxy> operator*(){
			hardNode CurrentNode;
			if(!(Owner->read_file(CurrentPos, CurrentNode))) throw std::runtime_error("err in hardMap<id , data>::iterator::operator*");
			return std::pair<const id, dataProxy>(CurrentNode.Id , dataProxy(Owner->File, CurrentPos, CurrentNode));
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
