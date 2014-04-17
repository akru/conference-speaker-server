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
 *                                 convolution.h
 *
 * Linear convolution and polynomial multiplication.
 *
 * The convolution routine "conv" is implemented by it's definition in time
 * domain. If the sequence to be convoluted are long, you should use the
 * fast convolution algorithm "fastConv", which is implemented in frequency
 * domain by usin FFT.
 *
 * Zhang Ming, 2010-01, Xi'an Jiaotong University.
 *****************************************************************************/


#ifndef CONVOLUTION_H
#define CONVOLUTION_H


#include <vector.h>
#include <fft.h>
#include <utilities.h>


namespace splab
{

    template<typename Type> Vector<Type> conv( const Vector<Type>&,
                                               const Vector<Type>& );
    template<typename Type> Vector<Type> convolution( const Vector<Type>&,
                                                      const Vector<Type>& );

    template<typename Type> Vector<Type> fastConv( const Vector<Type>&,
                                                   const Vector<Type>& );


    #include <convolution-impl.h>

}
// namespace splab


#endif
// CONVOLUTION_H
