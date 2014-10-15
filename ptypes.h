/***************************************************************************
 PROJECT   : Post Security Device
 COPYRIGHT : 2000, Pitney Bowes, Inc.
 AUTHOR    : 
 MODULE    : ptype.h
 
 Description :
    The ptype.h file is used to define the primitive types.

  MODIFICATION HISTORY :
	01/12/2001  Davies	Added volatile definitions.
    05\18\2000  Davies  Added definition of NULL_CHAR.

    Checkin : 04/06/2000 by Amy
              modify file format according to the agreement in code review 
              meeting             

    Created : 1/22/2000
 ****************************************************************************/

#ifndef _PTYPES_
#define _PTYPES_


#ifdef WIN32
/****************************************************************************/
/*                                                                          */
/* This section defines the primitive types for the Microsoft C compiler.   */
/*                                                                          */
/****************************************************************************/
        #define PACKED  
        typedef unsigned char           UINT8;
        typedef signed   char           SINT8;
        typedef unsigned short          UINT16;
        typedef signed   short          SINT16;
        typedef unsigned long           UINT32;
        typedef signed   long           SINT32;
        typedef unsigned __int64        UINT64;
        typedef signed   __int64        SINT64;
        typedef unsigned short          BOOL;

// Volatile definitions for MSDEV are the same as non-volatile

        typedef unsigned char           VUINT8;
        typedef signed   char           VSINT8;
        typedef unsigned short          VUINT16;
        typedef signed   short          VSINT16;
        typedef unsigned long           VUINT32;
        typedef signed   long           VSINT32;
        typedef unsigned __int64        VUINT64;
        typedef signed   __int64        VSINT64;
        typedef unsigned short          VBOOL;

#else

#ifdef __arm
/****************************************************************************/
/*                                                                          */
/* This section defines the primitive types for the ARM ANSI C compiler.    */
/*                                                                          */
/****************************************************************************/

    #define    PACKED                   __packed
    typedef    unsigned char            UINT8;
    typedef    signed   char            SINT8;    
    typedef    unsigned short           UINT16;        
    typedef    signed   short           SINT16;        
    typedef    unsigned long            UINT32;
    typedef    signed   long            SINT32;
    typedef    long     long            UINT64;
    typedef    signed   long long       SINT64;    // only for ARM software    
    typedef    unsigned short           BOOL;

// added to circumvent the compiler optimizing away valid code
    typedef    volatile unsigned char            VUINT8;
    typedef    volatile signed   char            VSINT8;    
    typedef    volatile unsigned short           VUINT16;        
    typedef    volatile signed   short           VSINT16;        
    typedef    volatile unsigned long            VUINT32;
    typedef    volatile signed   long            VSINT32;
    typedef    volatile long     long            VUINT64;
    typedef    volatile signed   long long       VSINT64;    // only for ARM software    
    typedef    volatile unsigned short           VBOOL;

#else
#error "Unknown C compiler"
#endif 

#endif
/****************************************************************************/
/*                                                                          */
/* This section defines the primitive types for both C compilers.           */
/*                                                                          */
/****************************************************************************/
#ifndef NULL
    #define NULL ((void *) 0)              
#endif

#ifndef NULL_CHAR
    #define NULL_CHAR '\0'
#endif

#ifndef TRUE
    #define TRUE    1
    #define FALSE   0
#endif

#endif // _PTYPES_
