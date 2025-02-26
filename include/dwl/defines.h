#pragma once

#define field_parent_ptr(ptr, T, field) ((T *)((u8 *)(ptr) - offsetof(T, field)))

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;
typedef signed long isize;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned long usize;

typedef float f32;
typedef double f64;

typedef i32 fixed_t;
