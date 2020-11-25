#pragma once
#include "math.h"
//written by Ren Yu
//2015 3 13
class CMyMath
{
public:
	CMyMath(void);
	~CMyMath(void);
public:
	///矩阵基本运算///
	static void MatrixMultiply(double **a,int n,int m,int k,double **b,double **c);//c为返回结果//乘法
	static void MatrixTranspose(double **a,int n,int m,double **c);//c为返回结果//转置
	static double  MatrixDet(double **a,int n);//行列式
	static void MatrixInverse(double **a,int n,double **inv);//inv为返回结果//逆
	static void LeastSquare(double **a,int n,int m,int k,double **b,double **x);//x为返回结果//最小二乘
};
