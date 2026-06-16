#ifndef UTILS_FIXEDPOINT_H
#define UTILS_FIXEDPOINT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FIXED32_WBITS   14
#define FIXED32_ONE     (1<<FIXED32_WBITS)
#define FIXED32_HALF    (FIXED32_ONE>>1)
#define FIXED32_MASK    (FIXED32_ONE-1)

#define FIXED32_2FLOAT(_fixed)      (float(_fixed)/FIXED32_ONE)
#define FLOAT_2FIXED32(_fvalue)     (fixed32_t)((_fvalue)*FIXED32_ONE)
#define FLOAT_2UFIXED32(_fvalue)    (ufixed32_t)((_fvalue)*FIXED32_ONE)

#define INT_2FIXED32(_ivalue)       (fixed32_t)((_ivalue)<<FIXED32_WBITS)
#define INT_2UFIXED32(_ivalue)      (ufixed32_t)((_ivalue)<<FIXED32_WBITS)

#ifdef __cplusplus
}
#endif

#endif // UTILS_FIXEDPOINT_H
