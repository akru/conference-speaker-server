/*
 * Copyright (c) 2008-2011 Zhang Ming (M. Zhang), zmjerry@163.com
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 2 or any later version.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details. A copy of the GNU General Public License is available at:
 * http://www.fsf.org/licensing/licenses
 */


/*****************************************************************************
 *                                dgt-impl.h
 *
 * Implementation for Discrete Gabor Transform.
 *
 * Zhang Ming, 2010-03, Xi'an Jiaotong University.
 *****************************************************************************/


/**
 * Return the daul function of input window "g".
 */
template <typename Type>
Vector<Type> daul( const Vector<Type> &gn, int N, int dM )
{
    int L = gn.size(),
        NL = 2*L-N;

    Vector<Type> hn(L);
    Vector<Type> gg = wextend( gn, NL, "right", "zpd" );

    // coefficient matrix and constant vector
    Matrix<Type> H(NL/N,L/dM);
    Vector<Type> u(NL/N);
    u[0] = Type(1.0/N);

    // evaluate matrix H
    for( int k=0; k<dM; ++k )
    {
        for( int q=0; q<NL/N; ++q )
            for( int p=0; p<L/dM; ++p )
            {
                int index = mod( k+p*dM+q*N, NL );
                H[q][p] = gg[index];
            }

        // calculate the kth part value of h
        Vector<Type> tmp = trMult( H, luSolver( multTr(H,H), u ) );
//        Vector<Type> tmp = tsvd( H, u );
        for( int i=0; i<tmp.dim(); ++i )
            hn[k+i*dM] = tmp[i];
    }
    return hn;
}


/**
 * Compute the discrete Gabor transform of "signal". The coeffitions are
 * stored in "coefs", a Ls+Lw by Lw (Ls: lengthof "signsl", Lw:
 * length of "window") matrix. The row represents frequency ordinate
 * and column represents the time ordinate.
 */
template <typename Type>
Matrix< complex<Type> > dgt( const Vector<Type> &signal,
                             const Vector<Type> &anaWin,
                             int N, int dM, const string &mode )
{
    int Ls = signal.dim();
    int Lw = anaWin.dim();
    int M = (Ls+Lw)/dM;

    Vector<Type> sn = wextend( signal, Lw, "both", mode );

    Matrix< complex<Type> > coefs(N,M);
    Vector<Type> segment(Lw);
    Vector< complex<Type> > segDFT(Lw);
    Vector< complex<Type> > tmp(N);
    complex<Type> W = polar( Type(1), Type(-2*PI/N) );

    for( int m=0; m<M; ++m )
    {
        // intercept signal by window function
        for( int i=0; i<Lw; ++i )
            segment[i] = sn[i+m*dM]*anaWin[i];

        // Fourier transform
        segDFT = fft( segment );

        // calculate the mth culumn coefficients
        for( int n=0; n<N; ++n )
            tmp[n] = pow(W,n*m*dM) * segDFT[n*Lw/N];

        coefs.setColumn( tmp, m );
    }

    return coefs;
}


/**
 * Compute the inverse discrete Gabor transform from "coefs".
 */
template <typename Type>
Vector<Type> idgt( const Matrix< complex<Type> > &coefs,
                   const Vector<Type> &synWin,
                   int N, int dM )
{
    int M = coefs.cols();
    int Lw = synWin.size();
    int Ls = dM*M-Lw;

    // reallocate for signal and initialize it by "0"
    Vector<Type> signal(Ls);

    Matrix<Type> idftCoefs(N,M);
    Vector<Type> sn(N);
    Vector< complex<Type> > Sk(N);

    for( int i=0; i<M; ++i )
    {
        Sk = coefs.getColumn(i);
        sn = ifftc2r(Sk);
        sn *= Type(N);

        idftCoefs.setColumn( sn, i );
    }

    // compulate the ith element of signal
    for( int i=0; i<Ls; ++i )
    {
        int p = ceil(i+1, dM);
        for( int m=p; m<p+Lw/dM; ++m )
        {
            int n = mod(i,N);
            signal[i] += idftCoefs[n][m]*synWin[Lw-m*dM+i];
        }
    }

    return signal;
}
