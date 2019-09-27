/*
#include "Analysis.h"
#include "Table.h"

#define PIN_TRIGG 5
#define R 28        //The number of rounds


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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////// Test Vector ///////////////////////////////////////////////////////
unsigned char PT[8] = { 0xd7,0xa8,0xb9,0xa7,0x27,0x7c,0x05,0x4c };
unsigned char CT[8] = { 0, };
unsigned char key[16] = { 0x7e,0x44,0x05,0x7c,0xff,0x6f,0x9f,0x1a,0x31,0x72,0xcb,0x6b,0xe1,0x37,0x19,0xdb };
unsigned char mk[128] = { 0, };
unsigned char RK[R][32] = { 0, };
unsigned char dt[64] = { 0, };
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KeySchedule(unsigned char rk[][32], unsigned char key[128])
{
	unsigned char tmp[32] = { 0, };
	unsigned char tmp2[12] = { 0, };
	unsigned char tt[4] = { 0, };
	memcpy(rk[0], key, 32);

	for (int i = 0; i < 27; i++)
	{
		memcpy(tmp, key, 32);
		memcpy(key, &key[32], 96);
		memcpy(&key[96], tmp, 32);
		memcpy(tt, &key[112], 2);
		memcpy(&key[112], &key[114], 14);
		memcpy(&key[126], tt, 2);
		memcpy(tmp2, &key[96], 12);
		memcpy(&key[96], &key[108], 4);
		memcpy(&key[100], tmp2, 12);
		memcpy(rk[i + 1], key, 32);
	}

}

void SubCell(unsigned char *nib)
{
	int i;
	for (i = 0; i <= 15; i++)
		nib[i] = GS[nib[i]];
}

void PermBit(unsigned char *dt)
{
	unsigned char tmp[64] = { 0, };
	int i;
	for (i = 0; i <= 63; i++)
		tmp[BP[i]] = dt[i];
	memcpy(dt, tmp, 64);
}

void AddRoundKey(unsigned char dt[64], unsigned char RK[32], unsigned char RoundCon) {

	for (int i = 0; i < 16; i++) {
		dt[4 * i] = dt[4 * i] ^ RK[i];
		dt[4 * i + 1] = dt[4 * i + 1] ^ RK[16 + i];
	}

	for (int j = 0; j < 6; j++) {

		dt[4 * j + 3] = dt[4 * j + 3] ^ ((RoundCon >> j) & 1);
	}

	dt[63] = dt[63] ^ 1;
}


void Nibble_Bit(unsigned char dt[64], unsigned char nib[16]) {

	int i;

	for (i = 0; i < 16; i++)
	{
		dt[4 * i] = nib[i] & 1;
		dt[4 * i + 1] = (nib[i] >> 1) & 1;
		dt[4 * i + 2] = (nib[i] >> 2) & 1;
		dt[4 * i + 3] = (nib[i] >> 3) & 1;
	}//16nibble -> 64bit
};

void Bit_Nibble(unsigned char nib[16], unsigned char dt[64])
{
	for (int i = 0; i < 16; i++)
		nib[i] = dt[4 * i] | (dt[4 * i + 1] << 1) | (dt[4 * i + 2] << 2) | (dt[4 * i + 3] << 3);
}

void Seperate(unsigned char *out, unsigned char *in, int num)
{
	for (int i = 0; i < num; i += 8)
	{
		out[i] = (in[i / 8] >> 4) & 1;
		out[i + 1] = (in[i / 8] >> 5) & 1;
		out[i + 2] = (in[i / 8] >> 6) & 1;
		out[i + 3] = (in[i / 8] >> 7) & 1;
		out[i + 4] = (in[i / 8]) & 1;
		out[i + 5] = (in[i / 8] >> 1) & 1;
		out[i + 6] = (in[i / 8] >> 2) & 1;
		out[i + 7] = (in[i / 8] >> 3) & 1;
	}
}

void Bit_Byte(unsigned char out[8], unsigned char dt[64])
{
	for (int i = 0; i < 8; i++)
		out[i] = dt[8 * i] | (dt[8 * i + 1] << 1) | (dt[8 * i + 2] << 2) | (dt[8 * i + 3] << 3) | \
		(dt[8 * i + 4] << 4) | (dt[8 * i + 5] << 5) | (dt[8 * i + 6] << 6) | (dt[8 * i + 7] << 7);
}

void Encryption(unsigned char pt[], unsigned char rk[][32], unsigned char ct[])
{
	// To do implementation for your cipher
	Seperate(dt, pt, 64);

	unsigned char nibble[16] = { 0, };
	for (int i = 0; i < 28; i++)
	{
		Bit_Nibble(nibble, dt);
		SubCell(nibble);
		Nibble_Bit(dt, nibble);
		PermBit(dt);
		Bit_Nibble(nibble, dt);
		AddRoundKey(dt, rk[i], RC[i]);
	}
	Bit_Byte(ct, dt);
}
*/