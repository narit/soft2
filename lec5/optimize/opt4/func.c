/**
 *  学籍番号: 03-1230006
 *  氏名: 岩成達哉
 *      課題4: より早く収束するアルゴリズムを実装
 *          準ニュートン法に黄金分割法を組み込んで実装した
 *          初期値は ./a.out x0 x1 ... と入力する(デフォルトは0.0)
 */

#include <math.h>
#include <stdlib.h>
#include "func.h"

/* 次元を返す */
int f_dimension()
{
    return 3;
}

/* 関数値を返す */
double f_value(const double x[])
{
    // cos(x) + sin(y) + cos(z)
    return cos(x[0]) + sin(x[1]) + cos(x[2]);
}

/* 関数を表示する */
void f_print()
{
    printf("f = cos(x) + sin(y) + cos(z)\n");
}

/* 勾配を計算する */
void f_gradient(const double x[], double g[])
{
    g[0] = -sin(x[0]);  // ∂f/∂x = -sin(x)
    g[1] =  cos(x[1]);  // ∂f/∂y =  cos(y)
    g[2] = -sin(x[2]);  // ∂f/∂z = -sin(z)
}

/* 正方行列同士の掛け算 private */
void mal_matrix(const int dim, const double mat1[], const double mat2[], double dest[])
{
    int i, j, k;
    
    // 初期化
    for (i = 0; i < dim * dim; i++)
        dest[i] = 0.0;
    
    // 計算
    for (i = 0; i < dim; i++)
    {
        for (j = 0; j < dim; j++)
        {
            for (k = 0; k < dim; k++)
                dest[i * dim + j] += mat1[i * dim + k] * mat2[i + dim * k];
        }
    }
}

/**
 *  BFGS公式により擬似ヘッシアンを作る
 *  H[k+1] = A H[k] A + B
 *  ただし，A = (I - y[(s)^T] / [(y)^T]s), B = y[(y)^T] / [(y)^T]s
 *  http://www.sist.ac.jp/~suganuma/kougi/other_lecture/SE/opt/nonlinear/nonlinear.htm#2.7
 */
void f_quasi_hessian(const double y[], const double s[], double hes[])
{
    int dim = f_dimension();    // 次元
    double *mat[3];             // 途中計算の行列
    const int mat_num = 3;      // 途中計算の行列の数
    double *tmp = (double *)malloc(dim * dim * sizeof(double));
    double c = 0.0;             // c = [(y)^T]s
    int i, j;
    
    // メモリ確保
    for (i = 0; i < mat_num; i++)
        mat[i] = (double *)malloc(dim * dim * sizeof(double));
    
    // 定数c = [(y)^T]s
    for (i = 0; i < dim; i++)
        c += y[i] * s[i];
    
    // mat[0] = I
    // mat[1] = y[(s)^T] / [(y)^T]s
    // mat[2] = y[(y)^T] / [(y)^T]s
    for (i = 0; i < dim; i++)
    {
        for (j = 0; j < dim; j++)
        {
            // -- 単位行列を作る --
            if (i == j)
                mat[0][i * dim + j] =  1.0;
            else
                mat[0][i * dim + j] =  0.0;
            // ------------------
            
            // 計算用の行列
            mat[1][i * dim + j] = y[i] * s[j] / c;
            mat[2][i * dim + j] = y[i] * y[j] / c;
        }
    }
     
    // mat[0] = mat[0] - mat[1] = I - y[(s)^T] / [(y)^T]s
    for (i = 0; i < dim; i++)
    {
        for (j = 0; j < dim; j++)
            mat[0][i * dim + j] -= mat[1][i * dim + j];
    }
    
    // tmp = mat[0] * hes
    mal_matrix(dim, mat[0], hes, tmp);
    // hes = tmp * mat[0]
    mal_matrix(dim, tmp, mat[0], hes);
    
    // hes = hes + mat[2]
    for (i = 0; i < dim * dim; i++)
        hes[i] += mat[2][i];
    
    free(tmp);
    for (i = 0; i < mat_num; i++)
        free(mat[i]);
}
