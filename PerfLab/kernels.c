/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "gpzlx1",              /* Team name */

    "Gong",     /* First member full name */
    "gpzlx1@mail.edu.cn",  /* First member email address */

    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr1[] = "rotate1";
void rotate1(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    for (i = 0; i < dim; i++){
		for (j = 0; j < dim; j++)
			dst[(dim - 1 - j) * dim + i] = src[i * dim + j]; 
	}
}


char rotate_descr2[] = "rotate2";
void rotate2(int dim, pixel *src, pixel *dst) 
{
    int i, j;
	int k;
    for (j = 0; j < dim; j++){ 
		k = (dim - 1 - j) * dim;
		for (i = 0; i < dim; i++)
			dst[k + i] = src[i * dim + j]; //RIDX(i,j,n) ((i)*(n)+(j))
	}
}

/*char rotate_descr3[] = "rotate3";
void rotate3(int dim, pixel *src, pixel *dst) 
{
    int i, j;
	int k;
	int k2,k3,k1;
	int dim1,dim2,dim3;
    for (j = 0; j < dim; j++){ 
		k = (dim - 1 - j) * dim;
		k1 = k + 1;
		k2 = k1 + 1;
		k3 = k2 + 1;
		dim1 = dim + j;
		dim2 = dim1 + dim;
		dim3 = dim2 + dim;
		for (i = 0; i < dim; i+=4){
			dst[k + i] = src[i * dim + j]; 
			dst[k1 + i] = src[i * dim + dim1]; 
			dst[k2 + i] = src[i * dim + dim2]; 
			dst[k3 + i] = src[i * dim + dim3]; 
		}
	}
}*/

char rotate_descr4[] = "rotate4";
void rotate4(int dim, pixel *src, pixel *dst) 
{
    int i, j, k, h;
	int block = 32;
	int b = dim / block;
	int para = (b-1) * block;
	int p_d, p_s;
	for(i = 0; i < dim; i+=block){
		for(j = 0; j < dim; j+=block){
			p_d = (para - j) * dim + i;
			p_s = i * dim + j;
			for(k = 0; k < block; k++){
				for(h = 0; h < block; h++){
					dst[ p_d + (block - k - 1) * dim + h]= src[p_s + h * dim + k];
				}
			}
		}
	}
}

char rotate_descr5[] = "rotate5";
void rotate5(int dim, pixel *src, pixel *dst) 
{
    int i, j, k, h;
	int block = 32;
	int para = dim - block;
	int p_d, p_s;
	for(i = 0; i < dim; i+=block){
		for(j = 0; j < dim; j+=block){
			p_d = (para - j) * dim + i;
			p_s = i * dim + j;
			for(k = 0; k < block; k++){
				for(h = 0; h < block; h+=2){
					dst[p_d + (block - k - 1) * dim + h]= src[p_s + h * dim + k];
					dst[p_d + (block - k - 1) * dim + h + 1]= src[p_s + h * dim + k + dim];
				}
			}
		}
	}
}

char rotate_descr[] = "rotate";
void rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j, k, h;
	int block = 32;
	int para = dim - block;
	pixel * p_d, *p_s;
	for(i = 0; i < dim; i+=block){
		for(j = 0; j < dim; j+=block){
			p_d = dst + ((para - j) * dim + i);
			p_s = src + i * dim + j;
			for(k = 0; k < block; k++){
				for(h = 0; h < block; h+=2){
					*(p_d + (block - k - 1) * dim + h)= *(p_s + h * dim + k);
					*(p_d + (block - k - 1) * dim + h + 1)= *(p_s + h * dim + k + dim);
				}
			}
		}
	}
}
/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate1, rotate_descr1);   
	add_rotate_function(&rotate2, rotate_descr2);   
	add_rotate_function(&rotate4, rotate_descr4);  
	add_rotate_function(&rotate5, rotate_descr5); 
	add_rotate_function(&rotate, rotate_descr);  
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
 typedef struct {
	 int red;
	 int green;
	 int blue;
} p_sum;
 
char smooth_descr1[] = "smooth1";
void smooth1(int dim, pixel *src, pixel *dst){
	int i, j;
	int red,green,blue,num;
	p_sum sum[512 * 512];
	for(i = 0; i < dim; i++){
		for(j = 0; j < dim; j++){
			red = 0;
			green = 0;
			blue = 0;
			for(int k = max(i-1, 0); k <= min(i+1, dim-1); k++){
				red += (int)src[RIDX(k,j,dim)].red;
				green += (int)src[RIDX(k,j,dim)].green;
				blue += (int)src[RIDX(k,j,dim)].blue;
			}
				sum[RIDX(i, j, dim)].red = red;
				sum[RIDX(i, j, dim)].green = green;
				sum[RIDX(i, j, dim)].blue = blue;
		}
	}
	
	for(i = 0; i < dim; i++){
		for(j = 0; j < dim; j++){
			red = 0;
			green = 0;
			blue = 0;
			for(int k = max(j-1, 0); k <= min(j+1, dim - 1); k++){
				red += sum[RIDX(i, k, dim)].red;
				green += sum[RIDX(i, k, dim)].green;
				blue += sum[RIDX(i, k, dim)].blue;
			}
			
			if((i == 0 || i == dim - 1) && (j == 0 || j == dim - 1))
				num = 4;
			else if( i == 0 || i == dim - 1 || j == 0 || j == dim - 1)
				num = 6;
			else	
				num = 9;
			
			dst[RIDX(i, j, dim)].red = (unsigned short)(red/num);
			dst[RIDX(i, j, dim)].green = (unsigned short)(green/num);
			dst[RIDX(i, j, dim)].blue = (unsigned short)(blue/num);
		}
	}
		
}


char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst){
	int i, j;
	int num;
	pixel_sum sum[530 * 530];
	

		
	for(j = 0; j < dim; j++){
		sum[RIDX(0, j, dim)].red = src[RIDX(0, j, dim)].red + src[RIDX(1, j, dim)].red;
		sum[RIDX(0, j, dim)].green = src[RIDX(0, j, dim)].green + src[RIDX(1, j, dim)].green;
		sum[RIDX(0, j, dim)].blue = src[RIDX(0, j, dim)].blue + src[RIDX(1, j, dim)].blue;
		sum[RIDX(0, j, dim)].num = 2;
			
			
			
		for(i = 1; i < dim - 1; i++){
			sum[RIDX(i, j, dim)].red = src[RIDX(i-1, j, dim)].red + src[RIDX(i, j, dim)].red + src[RIDX(i+1, j, dim)].red;
			sum[RIDX(i, j, dim)].green = src[RIDX(i-1, j, dim)].green + src[RIDX(i, j, dim)].green + src[RIDX(i+1, j, dim)].green;
			sum[RIDX(i, j, dim)].blue = src[RIDX(i-1, j, dim)].blue + src[RIDX(i, j, dim)].blue + src[RIDX(i+1, j, dim)].blue;
			sum[RIDX(i, j, dim)].num = 3;
		}
			
		sum[RIDX(dim - 1, j, dim)].red = src[RIDX(dim - 2, j, dim)].red + src[RIDX(dim - 1, j, dim)].red;
		sum[RIDX(dim - 1, j, dim)].green = src[RIDX(dim - 2, j, dim)].green + src[RIDX(dim - 1, j, dim)].green;
		sum[RIDX(dim - 1, j, dim)].blue = src[RIDX(dim - 2, j, dim)].blue + src[RIDX(dim - 1, j, dim)].blue;
		sum[RIDX(dim - 1, j, dim)].num = 2;

	}
	

		
	for(i = 0; i < dim; i++){
 		num = sum[RIDX(i, 0, dim)].num + sum[RIDX(i, 1, dim)].num;
		dst[RIDX(i, 0, dim)].red = (unsigned short)((sum[RIDX(i, 0, dim)].red + sum[RIDX(i, 1, dim)].red)/num);
		dst[RIDX(i, 0, dim)].green = (unsigned short)((sum[RIDX(i, 0, dim)].green + sum[RIDX(i, 1, dim)].green)/num);
		dst[RIDX(i, 0, dim)].blue = (unsigned short)((sum[RIDX(i, 0, dim)].blue + sum[RIDX(i, 1, dim)].blue)/num);
		
		for(j = 1; j < dim-1; j++){
			num = sum[RIDX(i, j-1, dim)].num + sum[RIDX(i, j, dim)].num+ sum[RIDX(i, j+1, dim)].num;
			dst[RIDX(i, j, dim)].red = (unsigned short)((sum[RIDX(i, j-1, dim)].red + sum[RIDX(i, j, dim)].red + sum[RIDX(i, j+1, dim)].red)/num);
			dst[RIDX(i, j, dim)].green = (unsigned short)((sum[RIDX(i, j-1, dim)].green + sum[RIDX(i, j, dim)].green + sum[RIDX(i, j+1, dim)].green)/num);
			dst[RIDX(i, j, dim)].blue = (unsigned short)((sum[RIDX(i, j-1, dim)].blue + sum[RIDX(i, j, dim)].blue + sum[RIDX(i, j+1, dim)].blue)/num);
			}
						
		num = sum[RIDX(i, dim-2, dim)].num + sum[RIDX(i, dim - 1, dim)].num;
		dst[RIDX(i, dim - 1, dim)].red = (unsigned short)((sum[RIDX(i, dim-2, dim)].red + sum[RIDX(i, dim-1, dim)].red)/num);
		dst[RIDX(i, dim - 1, dim)].green = (unsigned short)((sum[RIDX(i, dim-2, dim)].green + sum[RIDX(i, dim-1, dim)].green)/num) ;
		dst[RIDX(i, dim - 1, dim)].blue = (unsigned short)((sum[RIDX(i, dim-2, dim)].blue + sum[RIDX(i, dim-1, dim)].blue)/num) ;
	}
				
}


char smooth_descr5[] = "smooth5";
void smooth5(int dim, pixel *src, pixel *dst){
	int i, j;
	int num;
	pixel_sum sum[541 * 541];
	

		
	for(i = 0; i < dim; i++){
		sum[RIDX(i, 0, dim)].red = src[RIDX(i, 0, dim)].red + src[RIDX(i, 1, dim)].red;
		sum[RIDX(i, 0, dim)].green = src[RIDX(i, 0, dim)].green + src[RIDX(i, 1, dim)].green;
		sum[RIDX(i, 0, dim)].blue = src[RIDX(i, 0, dim)].blue + src[RIDX(i, 1, dim)].blue;
		sum[RIDX(i, 0, dim)].num = 2;
			
			
			
		for(j = 1; j < dim - 1; j++){
			sum[RIDX(i, j, dim)].red = src[RIDX(i, j-1, dim)].red + src[RIDX(i, j, dim)].red + src[RIDX(i, j+1, dim)].red;
			sum[RIDX(i, j, dim)].green = src[RIDX(i, j-1, dim)].green + src[RIDX(i, j, dim)].green + src[RIDX(i, j+1, dim)].green;
			sum[RIDX(i, j, dim)].blue = src[RIDX(i, j-1, dim)].blue + src[RIDX(i, j, dim)].blue + src[RIDX(i, j+1, dim)].blue;
			sum[RIDX(i, j, dim)].num = 3;
		}
			
		sum[RIDX(i,  dim-1, dim)].red = src[RIDX(i,  dim-1, dim)].red + src[RIDX(i, dim-2, dim)].red;
		sum[RIDX(i,  dim-1, dim)].green = src[RIDX(i,  dim-1, dim)].green + src[RIDX(i, dim-2, dim)].green;
		sum[RIDX(i,  dim-1, dim)].blue = src[RIDX(i,  dim-1, dim)].blue + src[RIDX(i, dim-2, dim)].blue;
		sum[RIDX(i,  dim-1, dim)].num = 2;

	}
	

		
	for(j = 0; j < dim; j++){
 		num = sum[RIDX(0, j, dim)].num + sum[RIDX(1, j, dim)].num;
		dst[RIDX(0, j, dim)].red = (unsigned short)((sum[RIDX(0, j, dim)].red + sum[RIDX(1, j, dim)].red)/num);
		dst[RIDX(0, j, dim)].green = (unsigned short)((sum[RIDX(0, j, dim)].green + sum[RIDX(1, j, dim)].green)/num);
		dst[RIDX(0, j, dim)].blue = (unsigned short)((sum[RIDX(0, j, dim)].blue + sum[RIDX(1, j, dim)].blue)/num);
		
		for(i = 1; i < dim-1; i++){
			num = sum[RIDX(i-1, j, dim)].num + sum[RIDX(i, j, dim)].num+ sum[RIDX(i+1, j, dim)].num;
			dst[RIDX(i, j, dim)].red = (unsigned short)((sum[RIDX(i-1, j, dim)].red + sum[RIDX(i, j, dim)].red + sum[RIDX(i+1, j, dim)].red)/num);
			dst[RIDX(i, j, dim)].green = (unsigned short)((sum[RIDX(i-1, j, dim)].green + sum[RIDX(i, j, dim)].green + sum[RIDX(i+1, j, dim)].green)/num);
			dst[RIDX(i, j, dim)].blue = (unsigned short)((sum[RIDX(i-1, j, dim)].blue + sum[RIDX(i, j, dim)].blue + sum[RIDX(i+1, j, dim)].blue)/num);
			}
						
		num = sum[RIDX(dim-1, j, dim)].num + sum[RIDX(dim-1, j, dim)].num;
		dst[RIDX(dim-1, j, dim)].red = (unsigned short)((sum[RIDX(dim-1, j, dim)].red + sum[RIDX(dim-2, j, dim)].red)/num);
		dst[RIDX(dim-1, j, dim)].green = (unsigned short)((sum[RIDX(dim-1, j, dim)].green + sum[RIDX(dim-2, j, dim)].green)/num) ;
		dst[RIDX(dim-1, j, dim)].blue = (unsigned short)((sum[RIDX(dim-1, j, dim)].blue + sum[RIDX(dim-2, j, dim)].blue)/num) ;
	}
				
}


char smooth_descr9[] = "smooth9";
void smooth9(int dim, pixel *src, pixel *dst)
{
    pixel_sum sum[530][513];
    int i, j, num;
    for(i=0;i<dim; i++)
    {
       sum[i][0].red = (src[RIDX(i, 0, dim)].red+src[RIDX(i, 1, dim)].red);
       sum[i][0].blue = (src[RIDX(i, 0, dim)].blue+src[RIDX(i, 1,dim)].blue);
       sum[i][0].green = (src[RIDX(i, 0, dim)].green+src[RIDX(i, 1,dim)].green);
       sum[i][0].num = 2;
       
	   for(j=1;j<dim-1; j++)
        {
           sum[i][j].red = (src[RIDX(i, j-1, dim)].red+src[RIDX(i, j,dim)].red+src[RIDX(i, j+1, dim)].red);
           sum[i][j].blue = (src[RIDX(i, j-1, dim)].blue+src[RIDX(i, j,dim)].blue+src[RIDX(i, j+1, dim)].blue);
           sum[i][j].green = (src[RIDX(i, j-1, dim)].green+src[RIDX(i, j,dim)].green+src[RIDX(i, j+1, dim)].green);
           sum[i][j].num = 3;
        }
       
	   sum[i][dim-1].red = (src[RIDX(i, dim-2, dim)].red+src[RIDX(i, dim-1,dim)].red);
       sum[i][dim-1].blue = (src[RIDX(i, dim-2, dim)].blue+src[RIDX(i,dim-1, dim)].blue);
       sum[i][dim-1].green = (src[RIDX(i, dim-2, dim)].green+src[RIDX(i,dim-1, dim)].green);
       sum[i][dim-1].num = 2;
    }
    for(j=0;j<dim; j++)
    {
        num =sum[0][j].num+sum[1][j].num;
        dst[RIDX(0,j, dim)].red = (unsigned short)((sum[0][j].red+sum[1][j].red)/num);
        dst[RIDX(0,j, dim)].blue = (unsigned short)((sum[0][j].blue+sum[1][j].blue)/num);
        dst[RIDX(0,j, dim)].green = (unsigned short)((sum[0][j].green+sum[1][j].green)/num);
        
		for(i=1;i<dim-1; i++)
        {
           num =sum[i-1][j].num+sum[i][j].num+sum[i+1][j].num;
           dst[RIDX(i, j, dim)].red = (unsigned short)((sum[i-1][j].red+sum[i][j].red+sum[i+1][j].red)/num);
           dst[RIDX(i, j, dim)].blue = (unsigned short)((sum[i-1][j].blue+sum[i][j].blue+sum[i+1][j].blue)/num);
           dst[RIDX(i, j, dim)].green = (unsigned short)((sum[i-1][j].green+sum[i][j].green+sum[i+1][j].green)/num);
        }
       
	   num =sum[dim-1][j].num+sum[dim-2][j].num;
       dst[RIDX(dim-1, j, dim)].red = (unsigned short)((sum[dim-2][j].red+sum[dim-1][j].red)/num);
       dst[RIDX(dim-1, j, dim)].blue = (unsigned short)((sum[dim-2][j].blue+sum[dim-1][j].blue)/num);
       dst[RIDX(dim-1, j, dim)].green = (unsigned short)((sum[dim-2][j].green+sum[dim-1][j].green)/num);
    }
}

/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
	add_smooth_function(&smooth1, smooth_descr1);
	add_smooth_function(&smooth5, smooth_descr5);
	add_smooth_function(&smooth9, smooth_descr9);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}

