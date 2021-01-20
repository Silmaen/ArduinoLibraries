/**
 * \author argawaen
 * \date 18/01/2021
 *
 */

#pragma once

#include <cstddef>


#if __cplusplus >= 201703L
#    define ASTD_NODISCARD [[__nodiscard__]]
#else
#    define ASTD_NODISCARD
#endif

// Macro for noexcept, to support in mixed 03/0x mode.
#ifndef ASTD_NOEXCEPT
#    if __cplusplus >= 201103L
#        define ASTD_NOEXCEPT         noexcept
#        define ASTD_NOEXCEPT_IF(...) noexcept(__VA_ARGS__)
#        define ASTD_USE_NOEXCEPT     noexcept
#        define ASTD_THROW(_EXC)
#    else
#        define ASTD_NOEXCEPT
#        define ASTD_NOEXCEPT_IF(...)
#        define ASTD_USE_NOEXCEPT throw()
#        define ASTDX_THROW(_EXC) throw(_EXC)
#    endif
#endif

#ifdef __GCC__
#define ASTD_NORETURN __attribute__ ((__noreturn__))
#else
#define ASTD_NORETURN
#endif

#define ASTD_BUFFER_SIZE 8
