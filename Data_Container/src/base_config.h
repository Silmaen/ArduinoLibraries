/**
 * \author argawaen
 * \date 18/01/2021
 *
 */

#pragma once

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_SAMD)
#    include <cstdint>
#elif defined(ARDUINO_ARCH_AVR)
#    include <stdint.h>
#endif

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

namespace astd {

    using a_size_t = uint16_t;
    using a_diff_t = uint16_t;

} // namespace astd
