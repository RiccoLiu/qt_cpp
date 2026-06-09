#ifndef UTILS_SINGLETON_H
#define UTILS_SINGLETON_H

#include <stdio.h>

#include "compiler.h"
#include "utils_global.h"

template <typename T>
class Singleton {
public:
    static T &GetInstance() {
        static T theInstance;
        return theInstance;
    }

protected:
    Singleton() {}
    virtual ~Singleton() {}

private:
    // Singleton(const Singleton &) {}
    // Singleton &operator=(const Singleton &) {}

    DISALLOW_COPY_AND_ASSIGN(Singleton);
};

#endif // UTILS_SINGLETON_H

