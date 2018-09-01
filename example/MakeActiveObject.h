#pragma once

#include <memory>

template <class TInterface, class TExecutor>
std::unique_ptr<TInterface>
make_active_object(const std::shared_ptr<TInterface>& impl, TExecutor& executor);

