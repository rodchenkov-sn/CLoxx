#include "Instance.hpp"

Instance::Instance(Klass* klass):
    klass_(klass)
{
}

std::string Instance::toString() const
{
    return klass_->toString() + " instance";
}
