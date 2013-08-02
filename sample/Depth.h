#pragma once
//Depth:20130628
#include <iostream>

namespace symbolia
{
class Depth{
public:
	Depth():depth_int_(0){}
    explicit Depth(int depth_int) : depth_int_(depth_int){}
	
	auto OneStepDeeper()const -> const Depth {
		return Depth(depth_int_+1);	
	}

	auto operator==(int right)const -> const bool {
		return depth_int_ == right;
	}
	
	auto operator!=(int right)const -> const bool {
		return !(*this == right);
	}

	friend auto operator<<(std::ostream& os, const Depth& depth) -> std::ostream&;
private:
	int depth_int_;
};

auto operator<<(std::ostream& os, const Depth& depth) -> std::ostream& {
	os << depth.depth_int_;
	return os;
}
}

