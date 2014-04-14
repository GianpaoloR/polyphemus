/* -*-c++-*- */
 /*
  *  $Id: CgiDefs_8h-source.html,v 1.3 2008/01/19 21:53:39 sebdiaz Exp $
  *
  *  Copyright (C) 1996 - 2004 Stephen F. Booth <sbooth@gnu.org>
  *                       2007 Sebastien DIAZ <sebastien.diaz@gmail.com>
  *  Part of the GNU cgicc library, http://www.gnu.org/software/cgicc
  *
  *  This library is free software; you can redistribute it and/or
  *  modify it under the terms of the GNU Lesser General Public
  *  License as published by the Free Software Foundation; either
  *  version 3 of the License, or (at your option) any later version.
  *
  *  This library is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  *  Lesser General Public License for more details.
  *
  *  You should have received a copy of the GNU Lesser General Public
  *  License along with this library; if not, write to the Free Software
  *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
  */

 #ifndef _CGIDEFS_H_
 #define _CGIDEFS_H_ 1
 
 // Include information from configure
 #if HAVE_CONFIG_H
 #  include "config.h"
 #endif

#  define HOST "Linux"
#  define VERSION "3.2.4"

 // Win32-specific setup
 #ifdef WIN32
 
 // export library symbols
 #  ifdef CGICC_EXPORTS
 #    define CGICC_API __declspec(dllexport)
 #  else
 #    define CGICC_API __declspec(dllimport)
 #  endif
 
 #  define HOST "Win32"
 #  define VERSION "3.2.4"
 
 #else
 #  define CGICC_API
 #endif /* WIN32 */
 
 #endif /* ! _CGIDEFS_H_ */
