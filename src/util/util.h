#pragma once

#include <memory>

template <typename T> using U = std::unique_ptr<T>;
template <typename T> using P = std::shared_ptr<T>;
