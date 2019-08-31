#pragma once
#include "Klass.hpp"

class Instance
{
public:
    explicit Instance(Klass* klass);
    [[nodiscard]] std::string toString() const;
private:
    Klass* klass_;
};

