#pragma once
//Offset:20130628
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>

namespace symbolia
{
class Offset{
public:
	Offset():offset_int_(0){}
    explicit Offset(int offset_int) : offset_int_(offset_int){}

	auto ToInt()const -> int {
		return offset_int_;	
	}
	

private:
	int offset_int_;
};

auto operator<<(std::ostream& os, const Offset& offset) -> std::ostream& {
	os << offset.ToInt();
	return os;
}

auto OffsetToString(const Offset& offset) -> const std::string {
	if(offset.ToInt()>=0){
		return "+"+boost::lexical_cast<std::string>(offset.ToInt());	
	}
	else {
		return boost::lexical_cast<std::string>(offset.ToInt());	
	}
}
}

