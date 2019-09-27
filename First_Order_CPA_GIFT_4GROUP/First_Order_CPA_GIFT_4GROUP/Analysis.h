#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_

/********************************************************************************************/
/*                                         변수 설정                                        */
/********************************************************************************************/

/*--- 분석 종류 (Normal : 0,  Known Key : 1) ---*/
#define _METHOD_					0

#if !_METHOD_
/*--- 분석 위치 (PlainText Load : 0,  1R AddRoundKey : 1,  1R SubBytes : 2) ---*/
#define _TARGET_					2
#else
/*--- 분석 위치 (1R AddRoundKey : 1,  1R SubBytes : 2,  1R MixColumns :  3) ---*/
#define _TARGET_					2
#endif

/*--- 파일 경로 및 이름 ('\' 두 번 입력해야 함) ---*/
#define       _FOLD_				"C:\\Users\\SICADA\\source\\repos\\First_Order_CPA_GIFT_4GROUP\\First_Order_CPA_GIFT_4GROUP\\ChipWhisperer_Normal_GIFT_1R_1000tr_3640pt"
#define _TRACE_FILE_				"ChipWhisperer_Normal_GIFT_1R_1000tr_3640pt"
#define _PLAIN_FILE_				"ChipWhisperer_Normal_GIFT_1R_1000tr_3640pt_plain"
#if _METHOD_
#define   _KEY_FILE_				"ChipWhisperer_Normal_GIFT_1R_1000tr_3640pt_key"
#endif

/*--- 분석할 파형 수 (1 ~ 전체 파형 수) ---*/
#define _TRACE_NUM_					1000

/*--- 분석할 포인트 범위 (1 ~ 전체 포인트 수) ---*/
#define _START_POINT_				200
#define _END_POINT_					2000

/*--- 분석할 바이트 범위 (1 ~ 16) ---*/
#define _START_BOX_					1
#define _END_BOX_					16

/*--- 출력할 후보키 수 (Default : 2) ---*/
#define _CANDIDATES_				4

/********************************************************************************************/

/*--- 평문 길이 (128-bit : 16,  64-bit : 8) ---*/
#define	_PLAIN_SIZE_				8

/*--- 키 길이 (128-bit : 16,  64-bit : 8) ---*/
#define	_KEY_SIZE_					8

/********************************************************************************************/

typedef struct TRACE_HEADER {
	unsigned char strID[20];
	unsigned int trace_no;
	unsigned int point_no;
	unsigned char strEND[4];
} trace_HEADER;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define _FILE_NAME_SIZE_	1000
#define _PASS_				1
#define _FAIL_				0

int LOG(struct tm *t, __int64 start_time, __int64 end_time);
int POINT_VERIFY(unsigned int *POINTS, unsigned int *TRACE_NUM);
int First_Order_CPA(struct tm *time, unsigned int POINTS, unsigned int TRACE_NUM);

///////////////////////////////////////////// Must Implement These Fuction ///////////////////////////////////////////////
void Encryption(unsigned char plainText[], unsigned char RK[][32], unsigned char cipherText[]);
void KeySchedule(unsigned char rk[][32], unsigned char key[128]);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////// GIFT Functions //////////////////////////////////////////////////////
void SubCell(unsigned char *nib);
void PermBit(unsigned char *dt);
void AddRoundKey(unsigned char dt[64], unsigned char RK[32], unsigned char RoundCon);
void Nibble_Bit(unsigned char dt[64], unsigned char nib[16]);
void Bit_Nibble(unsigned char nib[16], unsigned char dt[64]);
void KeySchedule(unsigned char rk[][32], unsigned char key[128]);
void Seperate(unsigned char *out, unsigned char *in, int num);
void Bit_Byte(unsigned char out[8], unsigned char dt[64]);

#endif 