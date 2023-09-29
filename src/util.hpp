#feature on edition_2023
#pragma once

namespace yawarakai {

template <typename TFunction>
struct ScopeGuard {
    TFunction _func;
    bool _canceled = false;

    ScopeGuard(forward auto f) : _func{ forward f } {}
    ~ScopeGuard() {
        if (!_canceled) {
            _func();
        }
    }

    void cancel() { _canceled = true; }
};

}
