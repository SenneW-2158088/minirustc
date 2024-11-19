#pragma once

#include <memory>
#include <optional>

template <typename T> using U = std::unique_ptr<T>;
template <typename T> using P = std::shared_ptr<T>;
template <typename T> using Opt = std::optional<T>;
template<typename T>

constexpr T&& Move(T&& t) noexcept {
    return std::move(t);
}

template<typename T, typename... Args>
auto MU(Args&&... args) {
   return std::make_unique<T>(std::forward<Args>(args)...);
}
