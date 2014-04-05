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
 *                               toeplitz-impl.h
 *
 * Implementationfor Toeplitz matrices generator.
 *
 * Zhang Ming, 2010-11, Xi'an Jiaotong University.
 *****************************************************************************/


/**
 * Returns a nonsymmetric Toeplitz matrix T having cn as its first column and
 * rn as its first row. If the first elements of cn and rn are different, the
 * column element is used.
 */
template <typename Type>
Matrix<Type> toeplitz( const Vector<Type> &cn, const Vector<Type> &rn )
{
    int M = cn.size(),
        N = rn.size();
    Matrix<Type> T(M,N);

    // main diagonal and below the main diagonal
    for( int d=0; d<M; ++d )
        for( int i=0; i<M-d; ++i )
            T[i+d][i] = cn[d];

    // above the main diagonal
    for( int d=1; d<N; ++d )
        for( int i=0; i<N-d; ++i )
            T[i][i+d] = rn[d];

    return T;
}


/**
 * Returns the symmetric or Hermitian Toeplitz matrix formed from vector rn,
 * where rn defines the first row of the matrix.
 */
template <typename Type>
Matrix<Type> toeplitz( const Vector<Type> &rn )
{
    int N = rn.size();
    Matrix<Type> T(N,N);

    // main diagonal
    for( int i=0; i<N; ++i )
        T[i][i] = rn[0];

    // above and below the main diagonal
    for( int d=1; d<N; ++d )
        for( int i=0; i<N-d; ++i )
            T[i][i+d] = T[i+d][i] = rn[d];

    return T;
}
