#pragma once
/*
 * This file should be generated by the clang plugin
 */
#include "ICalculator.h"
#include "MakeActiveObject.h"

#include <boost/asio/io_service.hpp>

#include <memory>

class IResultObserverActiveObject : public IResultObserver {
  public:
    IResultObserverActiveObject(
        const std::shared_ptr<IResultObserver>& impl, boost::asio::io_service& ioService)
        : m_impl(impl)
        , m_ioService(ioService)
    {
    }

  public: // IResultObserver
    void observe(std::size_t result) override
    {
        m_ioService.post([this, result]() { m_impl.lock()->observe(result); });
    }

  private:
    std::weak_ptr<IResultObserver> m_impl;
    boost::asio::io_service& m_ioService;
};

template <>
std::unique_ptr<IResultObserver> make_active_object<IResultObserver, boost::asio::io_service>(
    const std::shared_ptr<IResultObserver>& impl, boost::asio::io_service& ioService)
{
    return std::make_unique<IResultObserverActiveObject>(impl, ioService);
}

class ICalculatorActiveObject : public ICalculator {
  public:
    ICalculatorActiveObject(
        const std::shared_ptr<ICalculator>& impl, boost::asio::io_service& ioService)
        : m_impl(impl)
        , m_ioService(ioService)
    {
    }

  public: // ICalculator
    void add(std::size_t a, std::size_t b, IResultObserver* resultObserver) override
    {
        m_ioService.post(
            [this, a, b, resultObserver]() { m_impl.lock()->add(a, b, resultObserver); });
    }

  private:
    std::weak_ptr<ICalculator> m_impl;
    boost::asio::io_service& m_ioService;
};

template <>
std::unique_ptr<ICalculator> make_active_object<ICalculator, boost::asio::io_service>(
    const std::shared_ptr<ICalculator>& impl, boost::asio::io_service& ioService)
{
    return std::make_unique<ICalculatorActiveObject>(impl, ioService);
}
