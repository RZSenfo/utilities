#pragma once


#ifndef INVOKER_WRAPPER_H
#define INVOKER_WRAPPER_H

#include "intercept.hpp"

class invoker_t {
public:
    bool locked = false;
    unsigned int scope_count = 0;
    invoker_t() {}
    ~invoker_t() {}
    void lock() {
        intercept::client::host::functions.invoker_lock();
        locked = true;
    }
    void unlock() {
        locked = false;
        intercept::client::host::functions.invoker_unlock();
    }
};
static thread_local std::unique_ptr<invoker_t> _invoker;

class invoker_wrapper {
public:
    invoker_wrapper() {
        _invoker->scope_count++;
        if (!_invoker->locked) {
            _invoker->lock();
        }
    }
    ~invoker_wrapper() {
        if (!(--_invoker->scope_count)) {
            _invoker->unlock();
        }
    }
};

#define INVOKER invoker_wrapper

#endif // !INVOKER_WRAPPER_H
