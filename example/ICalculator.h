#pragma once

#include <cstddef>
#include <memory>

class IResultObserver{
public:
    virtual ~IResultObserver() = default;
    virtual void observe(std::size_t result) = 0;
};

class ICalculator {
public:
    virtual ~ICalculator() = default;
    virtual void add(std::size_t a, std::size_t b, IResultObserver *resultObserver) = 0;
    virtual void mul(std::size_t a, std::size_t b, IResultObserver *resultObserver) = 0;
};