/*
 *
 *
 * Build: clang++ main.cpp -lboost_system -lpthread
 */

#include "ICalculator.h"
#include "ICalculatorActiveObject.h"
#include "MakeActiveObject.h"

#include <boost/asio.hpp>
#include <future>
#include <iostream>
#include <memory>
#include <thread>

class SimpleCalculator : public ICalculator {
    void add(std::size_t a, std::size_t b, const std::shared_ptr<IResultObserver>& resultObserver)
        override
    {
        resultObserver->observe(a + b);
    }
};

class PrintResultObserver : public IResultObserver {
    void observe(std::size_t result)
    {
        std::cout << "result: " << result << std::endl;
    }
};

int main()
{
    boost::asio::io_service ioService0;
    boost::asio::io_service ioService1;
    auto simpleCalculator = std::make_shared<SimpleCalculator>();
    auto printResultObserver = std::make_shared<PrintResultObserver>();
    auto simpleCalculatorActiveObject
        = make_active_object<ICalculator>(simpleCalculator, ioService0);
    auto printResultObserverActiveObject
        = make_active_object<IResultObserver>(printResultObserver, ioService1);

    simpleCalculatorActiveObject->add(1, 5, printResultObserver);
    simpleCalculatorActiveObject->add(10, 5, printResultObserver);
    simpleCalculatorActiveObject->add(20, 5, printResultObserver);

    std::thread t0([&ioService0]() { ioService0.run(); });
    std::thread t1([&ioService1]() { ioService1.run(); });
    t0.join();
    t1.join();

    return 0;
}