#include "MyMath.h"

#define MAX_ITERA 60        //svd分解函数要用
#define MIN_DOUBLE (1e-30) //svd分解函数要用
#define PRECISION  0.00000000001 //迭代精度
#define PI 3.14159265

CMyMath::CMyMath(void)
{
}

CMyMath::~CMyMath(void)
{
}

///矩阵基本运算///

//乘法
void CMyMath::MatrixMultiply(double **a,int n,int m,int k,double **b,double **c)
{
	int i,j,u;
	for(i=0;i<n;i++) 
	{
		for(j=0;j<k;j++) 
		{ 
			*(*(c+i)+j)=0.0; 
			for(u=0;u<m;u++) 
				*(*(c+i)+j)+=*(*(a+i)+u)**(*(b+u)+j); 
		} 
	}
	return;
}

//转置
void CMyMath::MatrixTranspose(double **a,int n,int m,double **c)
{
	for(int i=0;i<n;i++) 
	{ 
		for(int j=0;j<m;j++) 
			*(*(c+j)+i)=*(*(a+i)+j);
	}
	return;
}

//行列式
double  CMyMath::MatrixDet(double **a,int n)
{
	int i=0, j=0, c=0; 
	double **b;
	b=new double*[n];
	for(i=0;i<n;i++)
		b[i]=new double[n];

	int p=0, q=0;
	double sum=0;
	if(n == 1) 
	{
		for(i=0;i<n;i++)
			delete []b[i];
		delete []b;
		return a[0][0];
	}

	for(i=0;i<n;i++)
	{ 
		for(c=0;c<n-1;c++) 
		{ 
			if(c<i) 
				p=0; 

			else 
				p=1; 

			for(j=0;j<n-1;j++) 
			{ 
				b[c][j]=a[c+p][j+1]; 
			} 
		} 
		if(i%2==0) q=1; 
		else q=(-1); 

		sum=sum+a[i][0]*q*MatrixDet(b,n-1); 
	} 
	for(i=0;i<n;i++)
		delete []b[i];
	delete []b;
	return sum; 
}

//逆
void CMyMath::MatrixInverse(double **a,int n,double **inv)
{
	int ni,i,j;
	double **c=NULL;   
	c=new double*[n];   
	for(ni=0;ni<n;ni++)   
		c[ni]=new double[2*n];
	//增广矩阵
	for(i=0;i<n;i++)
	{
		for(j=0;j<n+n;j++)
		{
			if(j<n)
			{
				c[i][j]=*(*(a+i)+j);
			}
			else
			{
				if (j-n==i)
				{
					c[i][j]=1;
				} 
				else
				{
					c[i][j]=0;
				}
			}
		}
	}
	
	//消元
	int kx,jx,ix;
	for (kx=0;kx<n;kx++)
	{
		double t=1/c[kx][kx];
		int k1=kx+1;
		
		for(jx=kx;jx<n+n;jx++)
		{
			c[kx][jx]=c[kx][jx]*t;//本行归一化
		}
		
		for(jx=k1;jx<n;jx++)
		{
			double temp=c[jx][kx];
			for(ix=kx;ix<n+n;ix++)
			{
				c[jx][ix]=c[jx][ix]-temp*c[kx][ix];//执行消去操作
			}
		}
	}
	
	int ft,ik,il;
	for(ik=n-2;ik>=0;ik--)
	{
		for(ft=ik;ft<n-1;ft++)
		{
			double temp=c[ik][ft+1];
			for(il=0;il<n+n;il++)
				c[ik][il]=c[ik][il]-temp*c[1+ft][il];//执行消去操作
		}
	}
	
	for(i=0;i<n;i++) 
	{ 
		for(j=0;j<n;j++) 
			*(*(inv+i)+j)=c[i][j+n];
	}

	//delete
	for(i=0;i<n;i++)
	{
		delete []c[i];
		c[i]=NULL;
	}
	delete []c;
	c=NULL;
	return;
}

//最小二乘
void CMyMath::LeastSquare(double **a,int n,int m,int k,double **b,double **x)
{
	//只适用线性方程组
	int ni,i;
	double **at=NULL;   
	at=new double*[m];   
	for(ni=0;ni<m;ni++)   
		at[ni]=new double[n];
	MatrixTranspose(a,n,m,at);	//系数矩阵的转置

	double **ata=NULL;   
	ata=new double*[m];   
	for(ni=0;ni<m;ni++)   
		ata[ni]=new double[m];
	MatrixMultiply(at,m,n,m,a,ata);//at*a

	double **inv_ata=NULL;   
	inv_ata=new double*[m];   
	for(ni=0;ni<m;ni++)   
		inv_ata[ni]=new double[m];
	MatrixInverse(ata,m,inv_ata);//ata的逆矩阵

	double **atb=NULL;   
	atb=new double*[m];   
	for(ni=0;ni<m;ni++)   
		atb[ni]=new double[k];
	MatrixMultiply(at,m,n,k,b,atb);//at*b

	MatrixMultiply(inv_ata,m,m,k,atb,x);//at*b
    //delete
	for(i=0;i<m;i++)
	{
		delete []at[i];
		at[i]=NULL;
	}
	delete []at;
	at=NULL;
    
	for(i=0;i<m;i++)
	{
		delete []ata[i];
		ata[i]=NULL;
	}
	delete []ata;
	ata=NULL;

	for(i=0;i<m;i++)
	{
		delete []inv_ata[i];
		inv_ata[i]=NULL;
	}
	delete []inv_ata;
	inv_ata=NULL;

	for(i=0;i<m;i++)
	{
		delete []atb[i];
		atb[i]=NULL;
	}
	delete []atb;
	atb=NULL;
	return;
}