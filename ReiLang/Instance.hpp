#pragma once
#include "Klass.hpp"
#include <map>

class InstanceException : public std::exception
{
public:
	explicit InstanceException(const std::string& name);
	[[nodiscard]] char const* what() const override;
private:
	std::string msg_;
};

class Instance : std::enable_shared_from_this<Instance>
{
public:
    explicit Instance(Klass* klass);
    [[nodiscard]] std::string toString() const;
	[[nodiscard]] Value get(const std::string& field) const;
	void put(const std::string& field, const Value& value);
private:
    Klass* klass_;
	std::map<std::string, Value> fields_;
};

