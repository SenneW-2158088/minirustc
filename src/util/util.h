#pragma once

#include <memory>
#include <optional>

template <typename T> using U = std::unique_ptr<T>;
template <typename T> using P = std::shared_ptr<T>;
template <typename T> using Opt = std::optional<T>;
