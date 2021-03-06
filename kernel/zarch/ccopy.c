/***************************************************************************
Copyright (c) 2013-2018, The OpenBLAS Project
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.
3. Neither the name of the OpenBLAS project nor the names of
its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE OPENBLAS PROJECT OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "common.h"

static void ccopy_kernel_32(BLASLONG n, FLOAT *x, FLOAT *y)
{
    __asm__ volatile (
        "lgr  %%r1,%1            \n\t"
        "lgr  %%r2,%2            \n\t"
        "srlg %%r0,%0,5          \n\t"
        "0:                      \n\t"
        "pfd 1, 1024(%%r1)       \n\t"
        "pfd 2, 1024(%%r2)       \n\t"
        "mvc 0(256,%%r2),0(%%r1) \n\t"
        "agfi   %%r1,256         \n\t"
        "agfi   %%r2,256         \n\t"
        "brctg %%r0,0b               "
        :
        :"r"(n),"a"((const FLOAT (*)[n * 2])x),"a"((FLOAT (*)[n * 2])y)
        :"memory","cc","r0","r1","r2"
    );
}

int CNAME(BLASLONG n, FLOAT *x, BLASLONG inc_x, FLOAT *y, BLASLONG inc_y)
{
    BLASLONG i=0;
    BLASLONG ix=0,iy=0;

    if ( n <= 0     )  return(0);

    if ( (inc_x == 1) && (inc_y == 1 ))
    {

        BLASLONG n1 = n & -32;
        if ( n1 > 0 )
        {
            ccopy_kernel_32(n1, x, y);
            i=n1;
            ix=n1*2;
            iy=n1*2;
        }

        while(i < n)
        {
            y[iy] = x[iy] ;
            y[iy+1] = x[ix+1] ;
            ix+=2;
            iy+=2;
            i++ ;

        }


    }
    else
    {

        BLASLONG inc_x2 = 2 * inc_x;
        BLASLONG inc_y2 = 2 * inc_y;

        while(i < n)
        {
            y[iy] = x[ix] ;
            y[iy+1] = x[ix+1] ;
            ix += inc_x2 ;
            iy += inc_y2 ;
            i++ ;

        }

    }
    
    return(0);
}
