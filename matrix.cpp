#include <iostream>
#include "matrix.h"  // Модуль работы с матрицами содержит шаблон класса Matrix<Type>


using namespace std;

int main (){
  // Матрица А
  const int A1[] = {2, 3};
  const int A2[] = {1, 0};
  const int A3[] = {-1, 3};
  const int *const mxA[] = {A1, A2, A3}; 
// Матрица Б
  const int B1[] = {2,  0, 1};
  const int B2[] = {1, -2, 2};
  const int B3[] = {5,  0, 7};
  const int *const mxB[] = {B1, B2, B3};
// вычисляем
  Matrix<int> a(mxA, 3, 2);
  cout << "a = ";    a.print();
  Matrix<int> b(mxB, 3, 3);
  cout << "b =";
  b.print();
  // Умножение числа на матрицу, матрицы на матрицу и вычитание// Результат заносится в матрицу С
  Matrix<int> c = (2*a) - (b*a);
  cout << "c = 2a - ba =";    // Выводим результат
  c.print();

  return 0;
}