#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

//===================================================================
//                  Заглушки для VS
//===================================================================

#ifndef     _PROJECT_IAR_
    #define inline
    #define __noreturn
    #define __packed
    #define __no_init
#endif  // _PROJECT_IAR_

//===================================================================
//              Переобъявление стандартных типов
//===================================================================

typedef unsigned long int               u64;
typedef   signed long int               s64;

typedef unsigned int                    u32;
typedef int                             s32;
typedef unsigned int                    uint32_t;

typedef unsigned short int              u16;
typedef short int                       s16;

typedef unsigned char                   u8;
typedef signed char                     s8;

typedef float                           f32;
typedef double                          f64;

typedef signed int                      _iq;


#ifndef bool

typedef enum{
    false   = 0,
    true    = !false,
}bool;

#endif
//===================================================================
#endif // GLOBAL_TYPES_H
