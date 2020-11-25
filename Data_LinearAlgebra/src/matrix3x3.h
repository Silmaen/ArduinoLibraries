//
// Created by damien.lachouette on 23/11/2020.
//

#pragma once
#include "vector3d.h"

namespace math {

    /**
     * @brief simple class handling 3x3 matrices
     */
    class Matrix3x3 {
    public:
        Matrix3x3()                      = default;
        ~Matrix3x3()                     = default;
        Matrix3x3(const Matrix3x3 &)     = default;
        Matrix3x3(Matrix3x3 &&) noexcept = default;
        Matrix3x3 &operator=(const Matrix3x3 &o) = default;
        Matrix3x3 &operator=(Matrix3x3 &&o) noexcept = default;

        /**
         * @brief direct component accessor (warning: there is no checks on inputs)
         * @param i index [0..8]
         * @return
         */
        float  operator[](uint8_t i) const { return data[i]; }
        float &operator[](uint8_t i) { return data[i]; }
        /**
         * @brief Two indices accessor starting from 1 (warning: there is no checks on inputs)
         * @param i row index {1,2,3}
         * @param j column  index {1,2,3}
         * @return
         */
        float  operator()(uint8_t i, uint8_t j) const { return data[j - 1 + 3 * (i - 1)]; }
        float &operator()(uint8_t i, uint8_t j) { return data[j - 1 + 3 * (i - 1)]; }

        /**
         * @brief setup a null matrix
         */
        void makeNull() {
            for (uint8_t i = 0; i < 9; ++i) {
                data[i] = 0;
            }
        }

        /**
         * @brief setup this matrix as identity
         */
        void makeIdentity() {
            makeNull();
            data[0] = 1.f;
            data[4] = 1.f;
            data[8] = 1.f;
        }

        /**
         * @brief setup a matrix with a constant diagonal
         * @param val the value to put on the diagonal
         */
        void setDiagonal(const float &val) {
            makeNull();
            data[0] = val;
            data[4] = val;
            data[8] = val;
        }

        /**
         * @brief setup a matrix with a constant diagonal
         * @param val1 the value xx on the diagonal
         * @param val2 the value yy put on the diagonal
         * @param val3 the value zz put on the diagonal
         */
        void setDiagonal(const float &val1, const float &val2, const float &val3) {
            makeNull();
            data[0] = val1;
            data[4] = val2;
            data[8] = val3;
        }

        /**
         * @brief setup a matrix with a constant diagonal
         * @param val the value to put on the diagonal
         */
        void setDiagonal(const vec3f &val) {
            makeNull();
            data[0] = val.x();
            data[4] = val.y();
            data[8] = val.z();
        }

        /**
         * @brief setup a matrix with a constant diagonal
         * @param col1 the first column
         * @param col2 the second column
         * @param col3 the third column
         */
        void setByColumn(const vec3f &col1, const vec3f &col2, const vec3f &col3) {
            data[0] = col1.x();
            data[1] = col2.x();
            data[2] = col3.x();
            data[3] = col1.y();
            data[4] = col2.y();
            data[5] = col3.y();
            data[6] = col1.z();
            data[7] = col2.z();
            data[8] = col3.z();
        }

        /**
         * @brief setup a matrix with a constant diagonal
         * @param lin1 the first line
         * @param lin2 the second line
         * @param lin3 the third line
         */
        void setByLine(const vec3f &lin1, const vec3f &lin2, const vec3f &lin3) {
            data[0] = lin1.x();
            data[1] = lin1.y();
            data[2] = lin1.z();
            data[3] = lin2.x();
            data[4] = lin2.y();
            data[5] = lin2.z();
            data[6] = lin3.x();
            data[7] = lin3.y();
            data[8] = lin3.z();
        }

        /**
         * @brief construc a rotation matrix based on the rotation between two vectors
         * @param a the input vector
         * @param b the vact that should be the result of this matric * a
         */
        void makeRotationBetweenVectors(const vec3f &a, const vec3f& b);

        /**
         * @brief construct a rotation matrix based on the look_at vector (X) and up vector (Z)
         * @param lookAt the X vector
         * @param up the vector most probably the Z vector
         */
        void makeRotationLookAndUp(const vec3f &lookAt, const vec3f& up);

        /**
         * @brief equality operator
         * @param o the other matrix to compare
         * @return true if the matrix are the same
         */
        bool operator==(const Matrix3x3 &o) const noexcept {
            for (uint8_t i = 0; i < 9; ++i) {
                if (data[i] != o.data[i])
                    return false;
            }
            return true;
        }

        /**
         * @brief inequality operator
         * @param o the other matrix to compare
         * @return true if the matrix are not the same
         */
        bool operator!=(const Matrix3x3 &o) const noexcept {
            for (uint8_t i = 0; i < 9; ++i) {
                if (data[i] != o.data[i])
                    return true;
            }
            return false;
        }

        /**
         * @brief test for a null matrix
         * @return true if the matrix is null
         */
        bool isNull() const;

        /**
         * @brief test for the identity matrix
         * @return true if the matrix is the identity
         */
        bool isIdentity() const;

        /**
         * @brief test if the matrix is diagonal
         * @return true if the matrix is diagonal
         */
        bool isDiagonal() const;

        /**
         * @brief test if the matrix is symmetrical
         * @return true if the matrix is symmetrical
         */
        bool isSymmetric() const;

        /**
         * @brief test if a matrix is orthogonal (M*M^T =I )
         * @return true if the matrix is orthogonal
         */
        bool isOthogonal() const;

        /**
         * @brief test if the matrix is a rotation matrix
         * @return true if the matrix is a rotation matrix
         */
        bool isRotation() const;

        /**
         * @brief addition operator
         * @param o the other matrix to add to this matrix
         * @return this matrix
         */
        Matrix3x3 &operator+=(const Matrix3x3 &o) {
            for (uint8_t i = 0; i < 9; ++i) {
                data[i] += o.data[i];
            }
            return *this;
        }

        /**
         * @brief addition operator
         * @param o the other matrix to add to the matrix
         * @return the result
         */
        Matrix3x3 operator+(const Matrix3x3 &o) const {
            Matrix3x3 a(*this);
            a += o;
            return a;
        }

        /**
         * @brief sub operator
         * @param o the other matrix to sub to this matrix
         * @return this matrix
         */
        Matrix3x3 &operator-=(const Matrix3x3 &o) {
            for (uint8_t i = 0; i < 9; ++i) {
                data[i] -= o.data[i];
            }
            return *this;
        }

        /**
         * @brief sub operator
         * @param o the other matrix to sub to the matrix
         * @return the result
         */
        Matrix3x3 operator-(const Matrix3x3 &o) const {
            Matrix3x3 a(*this);
            a -= o;
            return a;
        }

        /**
         * @brief product operator
         * @param o the other matrix to multiply to this matrix
         * @return this matrix
         */
        Matrix3x3 &operator*=(const Matrix3x3 &o);

        /**
         * @brief product operator
         * @param o the other matrix to multiply to the matrix
         * @return the result
         */
        Matrix3x3 operator*(const Matrix3x3 &o) const;

        /**
         * @brief product operator
         * @param o the other matrix to multiply to the matrix
         * @return the result
         */
        vec3f operator*(const vec3f &o) const;

        /**
         * @brief product operator
         * @param o the other matrix to multiply to the matrix
         * @return the result
         */
        friend vec3f operator*(const vec3f &o, const Matrix3x3 &m);

        /**
         * @brief scalar product operator
         * @param o the other matrix to multiply to this matrix
         * @return this matrix
         */
        Matrix3x3 &operator*=(const float &o);

        /**
         * @brief scalar product operator
         * @param o the scalar to multiply to the matrix
         * @return the result
         */
        Matrix3x3 operator*(const float &o) const;

        /**
         * @brief scalar product operator
         * @param o the scalar to multiply to the matrix
         * @return the result
         */
        friend Matrix3x3 operator*(const float &o, const Matrix3x3 &m);
        /**
         * @brief scalar division operator
         * @param o the other matrix to divide to this matrix
         * @return this matrix
         */
        Matrix3x3 &operator/=(const float &o);

        /**
         * @brief scalar division operator
         * @param o the scalar to divide to the matrix
         * @return the result
         */
        Matrix3x3 operator/(const float &o) const;

        /**
         * @brief double contracting product
         * @param o the scalar to multiply
         * @return the result
         */
        float doubleDot(const Matrix3x3 &o) const;

        /**
         * @brief get the norm of the matrix
         * @return the matrix norm
         */
        float norm() const { return sqrtf(doubleDot(*this)); }

        /**
         * @brief transpose this matrix
         * @return the transposed matrix
         */
        Matrix3x3 &transpose();

        /**
         * @brief get a transposed matrix of this matrix
         * @return a transposed matrix
         */
        Matrix3x3 transposed() const;

        /**
         * @brief compute  the matrix determinant
         * @return the determinant
         */
        float det() const;

        /**
         * @brief invert this matrix if possible (else, do nothing)
         * @return this matrix
         */
        Matrix3x3 &invert();

        /**
         * @brief get the invert matrix of this one if possible (else, get a copy of this matrix)
         * @return the inverted matrix of a copy of this one if not invertible
         */
        Matrix3x3 inverted() const;

    private:
        float data[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    };

} // namespace math