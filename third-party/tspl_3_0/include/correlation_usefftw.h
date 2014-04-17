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
 *                            correlation_usefftw.h
 *
 * Compute correlation by using FFTW, which is very efficiently for large
 * scale vectors.
 *
 * Zhang Ming, 2010-10, Xi'an Jiaotong University.
 *****************************************************************************/


#ifndef CORRELATION_USEFFTW_H
#define CORRELATION_USEFFTW_H


#include <convolution_usefftw.h>


namespace splab
{

    template<typename Type>
    Vector<Type> fastCorrFFTW( const Vector<Type>&, const string &opt="none" );

    template<typename Type>
    Vector<Type> fastCorrFFTW( const Vector<Type>&, const Vector<Type>&,
                               const string &opt="none" );

    template<typename Type>
    static void biasedProcessing( Vector<Type> &, const string &opt );


    #include <correlation_usefftw-impl.h>

}
// namespace splab


#endif
// CORRELATION_USEFFTW_H
