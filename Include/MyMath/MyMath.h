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
	///�����������///
	static void MatrixMultiply(double **a,int n,int m,int k,double **b,double **c);//cΪ���ؽ��//�˷�
	static void MatrixTranspose(double **a,int n,int m,double **c);//cΪ���ؽ��//ת��
	static double  MatrixDet(double **a,int n);//����ʽ
	static void MatrixInverse(double **a,int n,double **inv);//invΪ���ؽ��//��
	static void LeastSquare(double **a,int n,int m,int k,double **b,double **x);//xΪ���ؽ��//��С����
};
