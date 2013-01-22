/**
 *  学籍番号: 03-1230006
 *  氏名: 岩成達哉
 *      課題4: より早く収束するアルゴリズムを実装
 *          準ニュートン法に黄金分割法を組み込んで実装した
 *          初期値は ./a.out x0 x1 ... と入力する(デフォルトは0.0)
 */

#ifndef _FUNC_H_
#define _FUNC_H_

int f_dimension();
double f_value(const double []);
void f_gradient(const double [], double []);
void f_print();
void f_quasi_hessian(const double [], const double [], double []);

#endif
