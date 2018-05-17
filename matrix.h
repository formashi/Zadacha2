#ifndef MATRIX_H //содержит шаблон класса Matrix<Type>
#define MATRIX_H

#include <algorithm>
#include <iterator>
#include <vector>
#include <ostream>
#include <functional>

template <typename T>
class Matrix
{
public:
    Matrix(const size_t &rows, const size_t &cols) : __arr(rows*cols), __rows(rows), __cols(cols) {};
    Matrix(const Matrix<T> &mx) : __arr(mx.__arr.begin(), mx.__arr.end()), __rows(mx.__rows), __cols(mx.__cols) {};
    Matrix(const T *const mx[], const size_t &rows, const size_t &cols) : __arr(rows*cols), __rows(rows), __cols(cols)
    {
        size_t i;
        for (i = 0; i < __rows; ++i)
            std::copy(mx[i],
                mx[i]+cols,
                __arr.begin() + cols*i);
    };
    Matrix& operator=(const Matrix<T>&);

    class Error {};  // классы ќшибки
    class EOutOfIndex : public Error {};
    class ESizeMissmatch : public Error {};

    // –¤д матрицы
    // ############################
    class MatrixRow
    {
    public:
        MatrixRow(T* begin, const size_t &size) : __begin(begin), __size(size) {};
        T& operator[](const size_t &index) { return *(__begin+index); };
        const T& operator[](const size_t &index) const { return *(__begin+index); };
        T& at(const size_t &index)
        {
            if (__size <= index) throw EOutOfIndex();
            return *this[index];
        };
        const T& at(const size_t &index) const
        {
            if (__size <= index) throw EOutOfIndex();
            return *this[index];
        };
    private:
        T* __begin;
        size_t __size;
    };
    // ############################

    // —пециальный функтор дл¤ перемножени¤ матриц
    // ############################
    class RowMul : public std::binary_function<const T&, const T&, T>
    {
    public:
        explicit RowMul(const T& val) : __val(val) {};
        T operator()(const T &lhs, const T &rhs) { return (lhs + rhs*__val); };
    private:
        T __val;
    };
    // ############################

    MatrixRow operator[](const size_t &row) { return MatrixRow(&__arr[row*__cols], __cols); };
    const MatrixRow operator[](const size_t &row) const { return MatrixRow(&__arr[row*__cols], __cols); };

    MatrixRow at(const size_t &index)
    {
        if (__rows <= index) throw EOutOfIndex();
        return *this[index];
    };
    const MatrixRow at(const size_t &index) const
    {
        if (__rows <= index) throw EOutOfIndex();
        return *this[index];
    };

    // ”множение на число
    // ############################
    Matrix& mul(const T &mulTo)
    {
        std::transform(__arr.begin(), __arr.end(), __arr.begin(),
            std::bind2nd(std::multiplies<T>(), mulTo));
        return *this;
    };
    Matrix& operator*=(const T &mulTo)
    {
        return mul(mulTo);
    };

    // ”множение на матрицу
    // ############################
    Matrix& mul(const Matrix<T>&);
    Matrix operator*(const Matrix<T> &mx) const { return Matrix<T>(*this).mul(mx); };
    Matrix& operator*=(const Matrix<T> &mx) { return mul(mx); };

    Matrix operator-() const
    {
        Matrix<T> __result(*this);
        std::transform(__arr.begin(), __arr.end(), __result.__arr.begin(),
            std::bind1st(std::minus<T>(), T(0)));
        return __result;
    };

    // —ложение
    // ############################
    Matrix& add(const Matrix<T> &mx)
    {
        if ((mx.__rows != __rows) || (mx.__cols != __cols)) throw ESizeMissmatch();
        std::transform(__arr.begin(), __arr.end(), mx.__arr.begin(), __arr.begin(),
            std::plus<T>());
        return *this;
    };
    Matrix& operator+=(const Matrix<T> &mx) { return add(mx); };
    Matrix operator+(const Matrix<T> &mx) const { return Matrix<T>(*this).add(mx); };

    // ¬ычитание
    // ############################
    Matrix& sub(const Matrix<T> &mx)
    {
        if ((mx.__rows != __rows) || (mx.__cols != __cols)) throw ESizeMissmatch();
        std::transform(__arr.begin(), __arr.end(), mx.__arr.begin(), __arr.begin(),
            std::minus<T>());
        return *this;
    };
    Matrix& operator-=(const Matrix<T> &mx) { return sub(mx); };
    Matrix operator-(const Matrix<T> &mx) const { return Matrix<T>(*this).sub(mx); };

    // ¬ывод в поток (по умолчанию - в консоль)
    // ############################
    void print(std::ostream &out = std::cout) const
    {
        size_t i;
        for (i = 0; i < __rows; ++i)
        {
            std::copy(__arr.begin()+ __cols*i, __arr.begin() + __cols*(i + 1) - 1,
                std::ostream_iterator<T>(out, ", "));
            out << *(__arr.begin() + __cols*(i + 1) - 1) << std::endl;
        }
    };

private:
    std::vector<T> __arr;
    size_t __rows, __cols;
};

// ”множение на число
// ############################
template <typename T>
Matrix<T> operator*(const Matrix<T> &lhs, const T &rhs)
{
    return Matrix<T>(lhs).mul(rhs);
}

// ”множение числа на матрицу
// ############################
template <typename T>
Matrix<T> operator*(const T &lhs, const Matrix<T> &rhs)
{
    return Matrix<T>(rhs).mul(lhs);
}

// ќператор присваивани¤
// ############################
template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T> &mx)
{
    if (&mx == this) return *this;
    __arr.resize(mx.__arr.size());
    std::copy(mx.__arr.begin(), mx.__arr.end(), __arr.begin());
    __rows = mx.__rows;
    __cols = mx.__cols;
    return *this;
}

// ”множение матрицы на матрицу
// ############################
template <typename T>
Matrix<T>& Matrix<T>::mul(const Matrix<T> &mx)
{
    // ќшибка
    if (__cols != mx.__rows) throw ESizeMissmatch();
    typedef typename std::vector<T> VecT;
    typedef typename VecT::iterator VIter;
    VecT __result(mx.__cols*__rows);
    VIter __start(__result.begin()), __end(__result.end());
    VIter __jarr(__arr.begin());

    typename VecT::const_iterator __jmxarr, __jend(mx.__arr.end());

    // ѕеремножение по р¤дам
    while (__start != __end)
    {
        // ѕереходим в начало второй матрицы
        __jmxarr = mx.__arr.begin();
        // ѕеребираем вторую матрицу по строкам,
        // первую - по элементам
        while (__jmxarr != __jend)
        {
            // ”множение и сумму выполн¤ет класс RowMul
            std::transform(__start, __start + mx.__cols, __jmxarr,
                __start, RowMul(*__jarr));
            // —ледующа¤ строка второй матрицы
                __jmxarr += mx.__cols;
            // —ледующий элемент первой матрицы
                ++__jarr;
        }
        // ѕереходим к следующей строке результата
        __start += mx.__cols;
    }

    // ѕеремещаем результат в собственный вектор
    __arr.swap(__result);
    // ”станавливаем новое количество столбцов
    __cols = mx.__cols;

    return *this;
}

#endif // MATRIX_H