#pragma once

class ISimpleClass {
public:
    virtual ~ISimpleClass() = default;
    virtual void foo(int a) = 0;
};