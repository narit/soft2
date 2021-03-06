/**
 *  学籍番号: 03-1230006
 *  氏名: 岩成達哉
 *      課題4: より早く収束するアルゴリズムを実装
 *          準ニュートン法に黄金分割法を組み込んで実装した
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "optimize.h"

#define EPSILON 0.00000001  // 誤差の閾値
#define TRY_NUM 1000000     // 試行回数の限界
#define TAU ((sqrt(5.0) - 1.0) / 2.0)   // 黄金率


/* ベクトルの大きさを計算する */
double calc_norm(const int dim, double v[])
{
    double tmp = 0;
    int i = 0;
    for (i = 0; i < dim; i++)
        tmp += v[i] * v[i];
    
    tmp = sqrt(tmp);
    return tmp;
}

/*
 *  直線探索
 *  f(x(k) + α * d(k)) を最小にするαを求める
 */
double line_search(const int dim, const double x[], const double d[], 
                   double (*get_value)(const double []))
{
    double a = 0.0;
    double b = 1.0;
    
    double width = b - a;
    double right = b - TAU * width;
    double left  = a + TAU * width;
    double f_right, f_left;
    double *tmp = (double *)malloc(dim * sizeof(double));
    int i;
    
    // 右側の値
    for (i = 0; i < dim; i++)
        tmp[i] = x[i] + left * d[i];
    f_right = get_value(tmp);
    
    // 右側の値
    for (i = 0; i < dim; i++)
        tmp[i] = x[i] + right * d[i];
    f_right = get_value(tmp);
    
    // 左側の値
    for (i = 0; i < dim; i++)
        tmp[i] = x[i] + left * d[i];
    f_left = get_value(tmp);
    
    // 最適なαを探す
    while (width >= EPSILON)
    {
        // 調べる点の値を更新
        if (f_right >= f_left)
        {
            a       = right;
            width   = b - a;
            right   = left;
            f_right = f_left;
            left    = a + TAU * width;
            
            for (i = 0; i < dim; i++)
                tmp[i] = x[i] + left * d[i];
            f_left = get_value(tmp);
        }
        else
        {
            b       = left;
            width   = b - a;
            left    = right;
            f_left  = f_right;
            right   = b - TAU * width;
            
            for (i = 0; i < dim; i++)
                tmp[i] = x[i] + right * d[i];
            f_right = get_value(tmp);
        }
    }
    
    free(tmp);
    
    return (a + b) / 2.0;   // αを返す
}

/* 最適化問題を解く */
int optimize(const int dim, double x[],
             void (*calc_grad)(const double [], double []),
             void (*calc_hessian)(const double [], const double [], double []),
             double (*get_value)(const double []))
{
    double *grad    = (double *)malloc(dim * sizeof(double));   // 勾配
    double *next_grad = (double *)malloc(dim * sizeof(double)); // 次の勾配
    double *d       = (double *)malloc(dim * sizeof(double));   // 探索の方向d[k] = -H[k]∇f(x[k])
    double *next_x  = (double *)malloc(dim * sizeof(double));
    double *y       = (double *)malloc(dim * sizeof(double));   // y[k] = x[k+1] - x[k]
    double *s       = (double *)malloc(dim * sizeof(double));   // s[k] = ∇f(x[k+1]) - ∇f(x[k])
    double *hes     = (double *)calloc(dim * dim, sizeof(double));  // ヘッシアン
    int i, j;
    
    for (i = 0; i < dim; i++)   // 初めは単位行列に
        hes[i * (dim + 1)] = 1.0;
    
    int iter = 0;
    while (++iter < TRY_NUM)
    {
        calc_grad(x, grad);    // 勾配を計算
        
        // 初期化
        for (i = 0; i < dim; i++)
            d[i] = 0.0;
        
        for (i = 0; i < dim; i++)
        {
            for (j = 0; j < dim; j++)
                d[i] -= hes[dim * i + j] * grad[j];    // 探索方向d[k]を計算
        }
        
        const double alpha = line_search(dim, x, d, get_value);  // 変化の割合α[k]を求める
        for (i = 0; i < dim; i++)
        {
            next_x[i] = x[i] + alpha * d[i];  // 次の値
            y[i]      = next_x[i] - x[i];     // 判定条件用
        }
        
        const double norm = calc_norm(dim, y);   // 判定条件用
        
        // 途中経過の表示
        printf("%3d norm = %7.4f", iter, norm);
        for (i = 0; i < dim; i++)
            printf(", x[%d] = %7.4f", i, x[i]);
        printf(", f = %7.4f\n", get_value(x));   // 関数値を表示
        
        if (norm < EPSILON) break;
        
        // sの計算
        calc_grad(x, grad);
        calc_grad(next_x, next_grad);
        for (i = 0; i < dim; i++)
            s[i] = next_grad[i] - grad[i];
        
        // 更新
        for (i = 0; i < dim; i++)
            x[i] = next_x[i];
        
        // ヘッシアンの更新
        calc_hessian(y, s, hes);
    }
    
    free(grad);
    free(next_grad);
    free(d);
    free(next_x);
    free(y);
    free(s);
    free(hes);
    
    return iter;
}