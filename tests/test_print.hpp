#pragma once

template <typename... Args>
inline void print(Args&&... args) {
    (std::cout << ... << std::forward<Args>(args));
}

template <typename... Args>
inline void println(Args&&... args) {
    print(std::forward<Args>(args)...);
    std::cout << '\n';
}
