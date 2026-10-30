#include <chrono>
struct Timer {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    double ms() const {
        return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
    }
};