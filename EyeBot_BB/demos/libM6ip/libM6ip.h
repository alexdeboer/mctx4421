#ifndef _LIBM6IP_H_
#define _LIBM6IP_H_

//General types definitions, should later on move to a common types header file?
#if !defined(_BYTE)
#define _BYTE
typedef unsigned char BYTE;
#endif

#if !defined(_BOOL)
#define _BOOL
typedef unsigned char BOOL;
#define TRUE    1
#define FALSE   0
#define PERHAPS 2
#endif


/* image size */
#define imagecolumns 354
#define imagerows 290


/* Function prototypes */
typedef BYTE image[imagerows][imagecolumns];
typedef BYTE colimage[imagerows][imagecolumns][3];

#endif // _LIBM6IP_H_

