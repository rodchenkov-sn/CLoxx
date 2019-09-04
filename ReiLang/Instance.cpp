#include "Instance.hpp"
#include "Interpreter.hpp"

InstanceException::InstanceException(const std::string& name)
{
	msg_ = "Undefined property '" + name + "'.";
}

char const* InstanceException::what() const
{
	return msg_.c_str();
}

Instance::Instance(Klass* klass):
    klass_(klass)
{
}

std::string Instance::toString() const
{
    return klass_->toString() + " instance";
}

Value Instance::get(const std::string& field) const
{
	if (fields_.find(field) != fields_.end()) {
		return fields_.at(field);
	}
	const auto method = klass_->findMethod(field);
	if (method) {
		return Value{ method->bind( std::const_pointer_cast<Instance>(shared_from_this()) ) }; // really bad, I know
	}
	throw InstanceException{ field };
}

void Instance::put(const std::string& field, const Value& value)
{
	if (fields_.find(field) != fields_.end()) {
		fields_[field] = value;
	} else {
		fields_.insert({ field, value });
	}
}
