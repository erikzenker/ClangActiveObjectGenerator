#include "ICalculator.hpp"
#include "ICalculatorActiveObject.hpp"
#include "IResultObserver.hpp"
#include "IResultObserverActiveObject.hpp"

#include <boost/asio.hpp>
#include <future>
#include <iostream>
#include <memory>
#include <thread>

class SimpleCalculator : public ICalculator {
    void add(std::size_t a, std::size_t b, IResultObserver* resultObserver) override
    {
        resultObserver->observe(a + b);
    }

    void mul(std::size_t a, std::size_t b, IResultObserver *resultObserver) override
    {
        resultObserver->observe(a * b);
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
    auto simpleCalculatorActiveObject
        = make_active_object<ICalculator>(simpleCalculator, ioService0);
    auto printResultObserver = std::make_shared<PrintResultObserver>();
    auto printResultObserverActiveObject
        = make_active_object<IResultObserver>(printResultObserver, ioService1);

    simpleCalculatorActiveObject->add(1, 5, printResultObserverActiveObject.get());
    simpleCalculatorActiveObject->add(10, 5, printResultObserverActiveObject.get());
    simpleCalculatorActiveObject->add(20, 5, printResultObserverActiveObject.get());

    std::thread t0([&ioService0]() { ioService0.run(); });
    std::thread t1([&ioService1]() { ioService1.run(); });
    t0.join();
    t1.join();

    return 0;
}