#pragma once
//BasicTree:20130614
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include <boost/lexical_cast.hpp>

namespace semantia
{
template<class Value>
class BasicTree{
public:
	using Ptr = std::shared_ptr<BasicTree>;
	using ValueToString = std::function<std::string (const Value& value)>;

	static auto Create(const Value& token) -> const Ptr {
		return Ptr(new BasicTree<Value>(token));		
	}

	auto GetValue()const -> const Value {
		return value_;	
	}

	auto AddChild(const BasicTree::Ptr& child_node) -> void {
		child_tree_list_.push_back(child_node);
	}

	auto ReverseChildren() -> void {
		std::reverse(child_tree_list_.begin(), child_tree_list_.end());	
	}

	auto ResetChildIndex() -> void {
		child_index_=0;
		for(const auto& child : child_tree_list_){
			child->ResetChildIndex();
		}
	}

	auto EnumurateChild() -> const Ptr {
		if(child_tree_list_.size() <= child_index_){
			return Ptr();	
		}
		const auto child = child_tree_list_[child_index_];	
		++child_index_;
		return child;
	}

	auto GetNumOfChildren()const -> const int {
		return child_tree_list_.size();
	}

	auto IsNil()const -> const bool {
		return value_ == Value();
	}

	auto ToString()const -> const std::string {
		return ToString([](const Value& value) -> std::string { 
			return boost::lexical_cast<std::string>(value); });
	}
	
	auto ToString(const ValueToString& value_to_string)const -> const std::string {
		if(child_tree_list_.empty()){
			return value_to_string(value_);
		}
		std::vector<std::string> str_buffer;
		if(!IsNil()){
			str_buffer.push_back("(");
			str_buffer.push_back(value_to_string(value_));
			str_buffer.push_back(" ");	
		}
		for(unsigned int i = 0; i < child_tree_list_.size(); ++i){
			if(i > 0){
				str_buffer.push_back(" ");	
			}
			str_buffer.push_back(child_tree_list_.at(i)->ToString(value_to_string));
		}
		if(!IsNil()){
			str_buffer.push_back(")");
		}
		std::string result;
		for(const auto& str : str_buffer){
			result = result + str;
		}
		return result;
	}

private:
    BasicTree() : value_(), child_tree_list_(), child_index_(0){}
    BasicTree(const Value& value) : value_(value), child_tree_list_(), child_index_(0){}

	Value value_;
	std::vector<BasicTree::Ptr> child_tree_list_;
	unsigned int child_index_;
};

template<class Value>
auto operator<<(std::ostream& os, const BasicTree<Value>& tree) -> std::ostream& {
	os << tree.ToString();
	return os;
}
}
