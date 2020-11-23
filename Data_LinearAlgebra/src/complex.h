//
// Created by damien.lachouette on 23/11/2020.
//

#pragma once
#include "bases.h"

namespace math {

    /**
     * @brief class to handle complex number
     */
    class Complex {
    public:
        Complex() = default;
        explicit Complex(float re, float im) : real{re}, imaginary{im} {}
        explicit Complex(float re) : real{re}, imaginary{0} {}
        ~Complex()                   = default;
        Complex(const Complex &)     = default;
        Complex(Complex &&) noexcept = default;
        virtual Complex &operator=(const Complex &o) = default;
        virtual Complex &operator=(Complex &&o) noexcept = default;

        Complex &fromModArg(const float &mod, const float &arg) noexcept;

        /**
         * @brief equality between Complex
         * @param o the other Complex to compare
         * @return true if the Complex are identical
         */
        bool operator==(const Complex &o) const noexcept { return (real == o.real) && (imaginary == o.imaginary); }

        /**
         * @brief inequality between Complex
         * @param o the other Complex to compare
         * @return true if the Complex are not identical
         */
        bool operator!=(const Complex &o) const noexcept { return (real != o.real) || (imaginary != o.imaginary); }

        /**
         * @brief addition operator
         * @param o the other Complex to add to this one
         * @return this Complex
         */
        Complex &operator+=(const Complex &o) noexcept {
            real += o.real;
            imaginary += o.imaginary;
            return *this;
        }

        /**
         * @brief sub operator
         * @param o the other Complex to sud to this one
         * @return this Complex
         */
        Complex &operator-=(const Complex &o) noexcept {
            real -= o.real;
            imaginary -= o.imaginary;
            return *this;
        }

        /**
         * @brief addition operator
         * @param o the other Complex to add to this one
         * @return the sum of Complex
         */
        Complex operator+(const Complex &o) const noexcept {
            Complex res(*this);
            res += o;
            return res;
        }

        /**
         * @brief sub operator
         * @param o the other Complex to sud to this one
         * @return the sub of Complex
         */
        Complex operator-(const Complex &o) const noexcept {
            Complex res(*this);
            res -= o;
            return res;
        }

        /**
         * @brief scalar product operator
         * @param o the value to multiply to this Complex
         * @return this Complex
         */
        Complex &operator*=(const float &o) noexcept {
            real *= o;
            imaginary *= o;
            return *this;
        }

        /**
         * @brief scalar product operator (Complex * scalar)
         * @param o the value to multiply to this Complex
         * @return the resulting Complex
         */
        Complex operator*(const float &o) const noexcept {
            Complex res(*this);
            res *= o;
            return res;
        }
        /**
         * @brief scalar product operator
         * @param o the value to multiply to this Complex
         * @return this Complex
         */
        Complex &operator*=(const Complex &o) noexcept {
            float tr  = real;
            real      = real * o.real - imaginary * o.imaginary;
            imaginary = tr * o.imaginary + imaginary * o.real;
            return *this;
        }

        /**
         * @brief scalar product operator (Complex * scalar)
         * @param o the value to multiply to this Complex
         * @return the resulting Complex
         */
        Complex operator*(const Complex &o) const noexcept {
            Complex res(*this);
            res *= o;
            return res;
        }

        /**
         * @brief scalar product operator (scalar * Complex)
         * @param o the value to multiply to the given Complex
         * @param a the Complex to multiply
         * @return the resulting Complex
         */
        friend Complex operator*(const float &o, const Complex &a) noexcept {
            Complex res(a);
            res *= o;
            return res;
        }

        /**
         * @brief scalar division operator
         * @param o the value to divide to all component of this Complex
         * @return this Complex
         */
        Complex &operator/=(const float &o) {
            real /= o;
            imaginary /= o;
            return *this;
        }

        /**
         * @brief scalar division operator (Complex / scalar)
         * @param o the value to divide to all component of Complex
         * @return the resulting Complex
         */
        Complex operator/(const float &o) const {
            Complex res(*this);
            res /= o;
            return res;
        }

        /**
         * @brief scalar division operator
         * @param o the value to divide to all component of this Complex
         * @return this Complex
         */
        Complex &operator/=(const Complex &o);

        /**
         * @brief scalar division operator (Complex / scalar)
         * @param o the value to divide to all component of Complex
         * @return the resulting Complex
         */
        Complex operator/(const Complex &o) const {
            Complex res(*this);
            res /= o;
            return res;
        }

        // getters
        float  re() const noexcept { return real; }
        float  im() const noexcept { return imaginary; }
        float &re() noexcept { return real; }
        float &im() noexcept { return imaginary; }

        float  operator[](uint8_t iIndex) const { return iIndex==0?real:imaginary; }
        float &operator[](uint8_t iIndex) { return iIndex==0?real:imaginary; }

        /**
         * @brief get the module of the Complex
         * @return the Complex module
         */
        float mod() const { return sqrtf(real * real + imaginary * imaginary); }
        float norm() const { return mod(); }

        /**
         * @brief get the argument of the Complex
         * @return argument of the Complex
         */
        float arg() const;
        float phase() const { return arg(); }

        /**
         * @brief get the conjugate Complex of this one
         * @return the conjugate Complex
         */
        Complex conjugate() const { return Complex(real, -imaginary); }

        /**
         * @brief conjugate this Complex
         * @return this Complex
         */
        Complex &conjuge() {
            imaginary *= -1.F;
            return *this;
        }

    private:
        float real = 0;      ///< real part of the Complex
        float imaginary = 0; ///< imaginary part of the Complex
    };

    Complex sqrt(const Complex& c);

} // namespace math
