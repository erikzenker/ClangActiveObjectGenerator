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
    virtual void add(std::size_t a, std::size_t b, const std::shared_ptr<IResultObserver>& resultObserver) = 0;
};