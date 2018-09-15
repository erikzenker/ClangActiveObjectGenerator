#pragma once

#include <cstddef>

class IResultObserver {
  public:
    virtual ~IResultObserver() = default;
    virtual void observe(std::size_t result) = 0;
};
