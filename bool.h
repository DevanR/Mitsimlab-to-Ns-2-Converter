//-*-c++-*------------------------------------------------------------
// bool.h
//
// Qi Yang
// Copyright (C) 1997
// Massachusetts Institue of Technology
// All Rights Reserved
//
// This file contains the class definition/implementation for the class
//--------------------------------------------------------------------

#ifndef BOOL_HEADER
#define BOOL_HEADER

#if defined(__sgi) && !defined(__GNUC__) && !defined(_BOOL)
# define INTBOOL 1
typedef int bool;
#   ifndef false
#     define false 0
#   endif
#   ifndef true
#     define true  1
#   endif
# endif

#if defined(SUNOS) && !defined(__GNUC__) && !defined(_BOOL)
# define INTBOOL 1
typedef int bool;
#   ifndef false
#     define false 0
#   endif
#   ifndef true
#     define true  1
#   endif
# endif

#endif // BOOL_HEADER
