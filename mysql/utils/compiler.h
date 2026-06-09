#ifndef UTILS_COMPILER_H
#define UTILS_COMPILER_H

#if __cplusplus >= 201103L
#define DISALLOW_COPY_AND_ASSIGN(TypeName)                                     \
TypeName(const TypeName &) = delete;                                       \
    void operator=(const TypeName &) = delete
#else
#define DISALLOW_COPY_AND_ASSIGN(TypeName)                                     \
TypeName(const TypeName &);                                                \
    void operator=(const TypeName &)
#endif

#endif // UTILS_COMPILER_H
