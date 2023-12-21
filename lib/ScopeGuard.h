#pragma once

template <typename T>
struct ScopeGuard {

    T&& action;
    bool released{false};

    ~ScopeGuard() {
        if (!released) {
            action();
        }
    }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&) = default;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = default;
};

template <typename T>
static ScopeGuard<T> makeScopeGuard(T&& action) {
    return {std::forward<T>(action)};
}

