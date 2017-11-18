
#ifndef __COMM_H_
#define __COMM_H_
#include "string.h"

#ifndef NULL
#define NULL	 (void*)(0)
#else
#endif

#define  DBG(fmt,val)

typedef unsigned char   Bool;
typedef unsigned char   U8;  	/*unsigned 8 bit definition */
typedef unsigned short  U16; 	/*unsigned 16 bit definition*/
typedef unsigned int    U32; 	/*unsigned 32 bit definition*/
	
typedef signed   char     S8;   	/*signed 8 bit definition */
typedef signed   short    S16;  	/*signed 16 bit definition*/
typedef signed   int     S32;  	/*signed 32 bit definition*/
typedef unsigned short   COUNTER;


#ifdef WIN32
typedef char    CHAR;
#define __far
#else
typedef  char   CHAR;

#endif

#define TRUE			1
#define FALSE			0
#define WINK      2
#define UNKNOWN		2

#define BLINK           (0x02u)
#define ERR             (0x03u)

#define EMPTY		    0

#define ENABLE  		1
#define DISABLE 		0
#define SUCCESS  		1
#define FAIL    		0
#define OPEN			1
#define CLOSE			0
#define MASTER			0
#define SLAVE			1
#define INVALID_ID		0xff
#define NO_SPEED  0
#define YES 1
#define NO 0

#define  INPUT_MODE			 0
#define  OUTPUT_MODE         1

#define  ON                 1
#define  OFF                0
#define  CFG_ON				1
#define  CFG_OFF			0

#define  BIT0			(0x01u)
#define  BIT1			(0x02u)
#define  BIT2			(0x04u)
#define  BIT3			(0x08u)
#define  BIT4			(0x10u)
#define  BIT5			(0x20u)
#define  BIT6			(0x40u)
#define  BIT7			(0x80u)
#define  BIT8			(0x0100u)
#define  BIT9			(0x0200u)
#define  BIT10			(0x0400u)
#define  BIT11			(0x0800u)
#define  BIT12			(0x1000u)
#define  BIT13			(0x2000u)
#define  BIT14			(0x4000u)
#define  BIT15			(0x8000u)
#define  MASK0          (U8)(~BIT0)
#define  MASK1          (U8)(~BIT1)
#define  MASK2          (U8)(~BIT2)
#define  MASK3			(U8)(~BIT3)
#define  MASK4			(U8)(~BIT4)
#define  MASK5			(U8)(~BIT5)
#define  MASK6			(U8)(~BIT6)
#define  MASK7			(U8)(~BIT7)
#define  MASK8			(U16)(~BIT8)
#define  MASK9			(U16)(~BIT9)
#define  MASK10			(U16)(~BIT10)
#define  MASK11			(U16)(~BIT11)
#define  MASK12			(U16)(~BIT12)
#define  MASK13			(U16)(~BIT13)
#define  MASK14			(U16)(~BIT14)
#define  MASK15			(U16)(~BIT15)

#define  RANGE_LOFF_ROFF(VARIABLE,LOW,HIGH)	 	((((VARIABLE)>=(LOW)) && ((VARIABLE) <= (HIGH)))? 1:0)
#define  RANGE_LOPEN_ROFF(VARIABLE,LOW,HIGH)	((((VARIABLE)>(LOW)) && ((VARIABLE) <= (HIGH)))? 1:0)
#define  RANGE_LOPEN_ROPEN(VARIABLE,LOW,HIGH)	((((VARIABLE)>(LOW)) && ((VARIABLE) < (HIGH)))? 1:0)
#define  RANGE_LOFF_ROPEN(VARIABLE,LOW,HIGH)	((((VARIABLE)>=(LOW)) && ((VARIABLE) < (HIGH)))? 1:0)
#define  MAX(A,B)				(((A)>(B))?1:0)
#define  IS_SAME(A,B)			(((A) == (B))?1:0)
#define  IS_UN_SAME(A,B)		(((A) == (B))?0:1)


#define  CNT_ADD_OVER(name,max_value)		(((name)++) >= (max_value))?(1):(0)
#define  CNT_SUB_OVER(name,min_value)		(((name)--) <= (min_value))?(1):(0)

#define  U32_INVALID_VALUE   	(0xffffffffuL)
#define  U16_INVALID_VALUE		(0xffffuL)

#define  IS_BIT_ONE(VAR,n)		(((VAR)>>n)&(0x1))	
#define  SET_BIT(VAR,n)			((VAR) |=(1<<n))
#define  CLR_BIT(VAR,n)			((VAR) &= ~(1<<n))
#define  IS_GROUP32_BIT_ONE(BIT_MAP, x) (((BIT_MAP[(x)>>5]) >> ((x)&0x1F)) & (0x1))
#define  SET_GROUP32_BIT(BIT_MAP, x) ((BIT_MAP[(x)>>5]) |= (1<< ((x)&0x1F)))
#define  RESET_GROUP32_BIT(BIT_MAP, x) ((BIT_MAP[(x)>>5]) &=(~ (1<< ((x)&0x1F))))
#define  EVENT					1
#define  PERIOD				    0
#define  ACK					2

#define  RELEASE                  (1u)
#define  DEBUG_INTRX              (2u)
#define  ERROR_STATE              (-1)
#define  DEBUG_VERSION			  (0)
#define  LED_DRIVER_AS110         (1)


#define  BASE_10MS               (10)
#define  BASE_20MS				 (20)
#define  BASE_50MS				 (50)
#define  BASE_100MS				 (100)
#define  BASE_200MS				 (200)
#define  BASE_500MS				 (500)
#define  BASE_1000MS			 (1000)

#define  TimeMs(time_ms,base_time_ms)    ((time_ms)/(base_time_ms))					  		


#define  LITTLE_ENDIAN	 0
#define	 BIG_ENDIAN		 1
#define  MCU_MODE		 BIG_ENDIAN

enum
{
    ACT_RESET,
    ACT_TOUCH,
    ACT_TRUE,
    ACT_FALSE,
    ACT_TIMER,
};

#define ARRAY_SIZE(a) (sizeof (a) / sizeof (*a))

#define ERROR_CODE S8

#define STATUS_UNUSED 2
#define STATUS_REMAIN  1
#define STATUS_NORMAL 0
#define ERR_POINTER_0 -1
#define ERR_NOT_INIT -2
#define ERR_PARAMETER -3
#define ERR_NOT_FOUND -4
#define ERR_FULL -5
#define ERR_EMPTY -6
#define ERR_RAM -7
#define ERR_BUS -8
#define ERR_TIMEOUT -9
#define ERR_ALREADY_INIT -10
#define ERR_RESPONSE -11
#define ERR_IO -12
#define ERR_OPEN -13
#define ERR_CLOSE -14
#define ERR_START -15
#define ERR_STOP -16
#define ERR_USED -17
#define ERR_FAIL -18

#endif      /* __COMM_H_ */
