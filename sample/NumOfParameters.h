#pragma once
//NumOfParameters:20130628
#include <iostream>

namespace symbolia
{
class NumOfParameters{
public:
	NumOfParameters():num_of_params_int_(0){}
    explicit NumOfParameters(int num_of_params_int) : 
		num_of_params_int_(num_of_params_int){}

	auto ToInt()const -> int {
		return num_of_params_int_;	
	}

private:
	int num_of_params_int_;
};

auto operator<<(std::ostream& os, const NumOfParameters& num_of_params) -> std::ostream& {
	os << "NumOfParameters: " << num_of_params.ToInt();
	return os;
}
}

