#pragma once

#include "IResultObserver.h"

#include <cstddef>
#include <memory>


class ICalculator {
public:
    virtual ~ICalculator() = default;
    virtual void add(std::size_t a, std::size_t b, IResultObserver *resultObserver) = 0;
    virtual void mul(std::size_t a, std::size_t b, IResultObserver *resultObserver) = 0;
};