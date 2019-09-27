#include "Analysis.h"
#include "Table.h"

int First_Order_CPA(struct tm *time, unsigned int POINTS, unsigned int TRACE_NUM)
{
	/********************************************************************************************/
	/*                                         변수 선언                                        */
	/********************************************************************************************/
	trace_HEADER	in_trace = { 0, };

	FILE			*fp = NULL;
	FILE			*fp2 = NULL;
	FILE			*fpp = NULL;
	FILE			*fpt = NULL;

	char			FOLD_MERGE[_FILE_NAME_SIZE_] = "";
	char			FILE_MERGE[_FILE_NAME_SIZE_] = "";

	unsigned char	*Plaintext = NULL;
	unsigned char	*RealPlain = NULL;
	unsigned char	*Correct_Key = NULL;
	unsigned char	*dt = NULL;
	unsigned char	*nibble = NULL;
	unsigned char	ZERORC[16] = { 0, };
	unsigned char	solbit[16] = { 0, };
	unsigned char	master_key_bits[32] = { 0, };

	unsigned int	*H_S = NULL;
	unsigned int	*H_SS = NULL;
	unsigned int	i = 0;
	unsigned int	m = 0;
	unsigned int	n = 0;
	unsigned int	Byte = 0;
	unsigned int	Guess_Key = 0;
	unsigned int	Guess_Key_Num = 0;
	unsigned int	Key = 0;
	unsigned int	Key_HW = 0;
	unsigned int	R_Key = 0;
	unsigned int	Right_Key = 0;
	unsigned int	Index = 0;
	unsigned int	Percent = 0;
	unsigned int	Check = 0;
	unsigned int	Candidates = 0;
	unsigned int	Interval_Point = 0;

	unsigned __int16 right = 0;

	float			F_Temp = 0.;

	double			*Temp_Points = NULL;
	double			*MaxPeak = NULL;
	double			*W_CS = NULL;
	double			*W_CSS = NULL;
	double			**W_HS = NULL;
	double			Correlation = 0.;
	double			Correlation_L = 0.;
	double			Correlation_R = 0.;
	double			Max = 0.;
	double			Max_Sec = 0.;
	double			Ratio = 0.;

	// 분석할 포인트 길이 계산
	Interval_Point = _END_POINT_ - _START_POINT_ + 1;

	// Known Key 분석 또는 Plaintext Load 분석 시 Guess Key를 1개로 설정 & 그 외 분석은 256개로 설정
#if _METHOD_ || !_TARGET_
	Guess_Key_Num = 1;
#else
	Guess_Key_Num = 4;
#endif

	/********************************************************************************************/
	/*                                  변수 설정 값 오류 검사                                  */
	/********************************************************************************************/
	if (_START_POINT_ < 1 || _START_POINT_ > _END_POINT_) {
		printf(" -----------------------------------------------------------------------\n");
		printf("|                분석 범위의 시작 지점이 적절치 않습니다.               |\n");
		printf(" -----------------------------------------------------------------------\n");
		return _FAIL_;
	}

	if (_END_POINT_ > POINTS) {
		printf(" -----------------------------------------------------------------------\n");
		printf("|                 분석 범위의 끝 지점이 적절치 않습니다.                |\n");
		printf(" -----------------------------------------------------------------------\n");
		return _FAIL_;
	}

	if (TRACE_NUM < _TRACE_NUM_) {
		printf(" -----------------------------------------------------------------------\n");
		printf("|                    분석 파형 수가 적절치 않습니다.                    |\n");
		printf(" -----------------------------------------------------------------------\n");
		return _FAIL_;
	}

	/********************************************************************************************/
	/*                                       변수 동적할당                                       /
	/********************************************************************************************/
	// 평문 저장
	Plaintext = (unsigned char *)malloc(_PLAIN_SIZE_ * sizeof(unsigned char));
	RealPlain = (unsigned char *)malloc(_PLAIN_SIZE_ * sizeof(unsigned char) / 2);
	// 암호키 저장
	Correct_Key = (unsigned char *)malloc(_KEY_SIZE_ * sizeof(unsigned char));

	// 실제 데이터 값 저장
	dt = (unsigned char *)calloc(_PLAIN_SIZE_*8, sizeof(unsigned char));
	nibble = (unsigned char *)calloc(_PLAIN_SIZE_ * 2, sizeof(unsigned char));

	// 분석 포인트 저장
	Temp_Points = (double *)malloc(Interval_Point * sizeof(double));

	// 중간값 E[X] 저장
	H_S = (unsigned int *)malloc(Guess_Key_Num * sizeof(unsigned int));
	// 중간값 E[X^2] 저장
	H_SS = (unsigned int *)malloc(Guess_Key_Num * sizeof(unsigned int));

	// 파형 측정값 E[Y] 저장
	W_CS = (double *)malloc(Interval_Point * sizeof(double));
	// 파형 측정값 E[Y^2] 저장 
	W_CSS = (double *)malloc(Interval_Point * sizeof(double));

	// E[XY] 저장
	W_HS = (double **)malloc(Guess_Key_Num * sizeof(double *));
	for (Guess_Key = 0; Guess_Key < Guess_Key_Num; Guess_Key++)
		W_HS[Guess_Key] = (double *)malloc(Interval_Point * sizeof(double));

	// 각 키 후보군 별 MAXPEAK 저장
	MaxPeak = (double *)malloc(Guess_Key_Num * sizeof(double));

	/********************************************************************************************/
	/*                                   First Order CPA 시작                                   */
	/********************************************************************************************/
	// 후보키 개수 설정
	Candidates = _CANDIDATES_;
	if (Candidates < 2 || Candidates > Guess_Key_Num)
		Candidates = 2;

	// 평문 파일 열기 (읽기 모드)
	sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%s\\%s.txt", _FOLD_, _PLAIN_FILE_);
	fopen_s(&fpp, FILE_MERGE, "r");
	if (fpp == NULL) {
		printf(" -----------------------------------------------------------------------\n");
		printf("|                        Failed To Read Plaintext                       |\n");
		printf(" -----------------------------------------------------------------------\n");
		return _FAIL_;
	}

	// 파형 파일 열기 (읽기 모드)
	sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%s\\%s.trace", _FOLD_, _TRACE_FILE_);
	fopen_s(&fpt, FILE_MERGE, "rb");
	if (fpt == NULL) {
		printf(" -----------------------------------------------------------------------\n");
		printf("|                          Failed To Read Trace                         |\n");
		printf(" -----------------------------------------------------------------------\n");
		return _FAIL_;
	}

	// 파형 헤더 읽기 (32 바이트)
	fread(in_trace.strID, sizeof(unsigned char), 20, fpt);
	fread(&in_trace.trace_no, sizeof(unsigned int), 1, fpt);
	fread(&in_trace.point_no, sizeof(unsigned int), 1, fpt);
	fread(in_trace.strEND, sizeof(unsigned char), 4, fpt);

#if _METHOD_
	// 키 파일 열기 (읽기 모드)
	sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%s\\%s.txt", _FOLD_, _KEY_FILE_);
	fopen_s(&fp, FILE_MERGE, "r");
	if (fp == NULL) {
		printf(" -----------------------------------------------------------------------\n");
		printf("|                           Failed To Read Key                          |\n");
		printf(" -----------------------------------------------------------------------\n");
		return _FAIL_;
	}

	// 키 불러오기
	for (i = 0; i < _KEY_SIZE_; i++)
		fscanf_s(fp, "%hhx", &Correct_Key[i]);

	fclose(fp);
#else
	for (i = 0; i < _KEY_SIZE_; i++)
		Correct_Key[i] = 0;
#endif

	sprintf_s(FOLD_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%04d_%02d_%02d_%02d_%02d_%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

	// Byte 분석 위치
	for (Byte = _START_BOX_ - 1; Byte < _END_BOX_; Byte++) {
		// 결과 저장할 폴더 생성
		sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "mkdir %s\\%d_Byte", FOLD_MERGE, Byte + 1);
		system(FILE_MERGE);

		// 값 초기화
		for (m = 0; m < Interval_Point; m++) {
			W_CS[m] = 0.0;
			W_CSS[m] = 0.0;
			for (Guess_Key = 0; Guess_Key < Guess_Key_Num; Guess_Key++)
				W_HS[Guess_Key][m] = 0.0;
		}

		for (Guess_Key = 0; Guess_Key < Guess_Key_Num; Guess_Key++) {
			MaxPeak[Guess_Key] = 0;
			H_S[Guess_Key] = 0;
			H_SS[Guess_Key] = 0;
		}

		printf(" -----------------------------------------------------------------------\n");
		printf("|                            Loading Trace...                           |\n");
		printf(" -----------------------------------------------------------------------\n");

		for (n = 0; n < _TRACE_NUM_; n++) {
			// 진행률 표시
			Percent = (unsigned int)((double)n / (double)_TRACE_NUM_ * 100);
			if (Percent % 10 == 0 && Percent != 0 && Check != Percent) {
				printf("%d%%\t", Percent);
				if (Percent == 90)
					printf("\n");
				Check = Percent;
			}

			// 평문 불러오기
			fseek(fpp, (_PLAIN_SIZE_ * 3 + 2) * n, SEEK_SET);
			for (i = 0; i < _PLAIN_SIZE_; i++)
				fscanf_s(fpp, "%hhx", &Plaintext[i]);

			for (i = 0; i < (_PLAIN_SIZE_/2); i++)
			{
				RealPlain[i] = Plaintext[i << 1] << 4;
				RealPlain[i] |= Plaintext[(i << 1) + 1];
			}

			// 평문 -> 실제 데이터 값
			Seperate(dt, RealPlain, 64);

			// 애드라운드 키 제외한 1라운드 진행
			Bit_Nibble(nibble, dt);
			SubCell(nibble);
			Nibble_Bit(dt, nibble);
			PermBit(dt);
			Bit_Nibble(nibble, dt);

			// 포인트 값 불러오기
			fseek(fpt, 32 + (in_trace.point_no * n + _START_POINT_ - 1) * 4, SEEK_SET);
			for (m = 0; m < Interval_Point; m++) {
				fread(&F_Temp, sizeof(float), 1, fpt);
				Temp_Points[m] = (double)F_Temp;
			}

			// E[Y], E[Y^2] 계산
			for (m = 0; m < Interval_Point; m++) {
				W_CS[m] += Temp_Points[m];
				W_CSS[m] += Temp_Points[m] * Temp_Points[m];
			}

			for (Guess_Key = 0; Guess_Key < Guess_Key_Num; Guess_Key++) {
				// 중간값 위치 (PlainText Load : 0,  1R AddRoundKey : 1,  1R SubBytes : 2)
#if _TARGET_ == 0
				Key = Plaintext[Byte];
#elif _TARGET_ == 1
				Key = Plaintext[Byte] ^ (Guess_Key + Correct_Key[Byte]);
#elif _TARGET_ == 2
				if (Byte == 0 || Byte == 15)
					Key = GS[nibble[Byte] ^ Guess_Key ^ 0x8];
				else
					Key = GS[nibble[Byte] ^ Guess_Key];
#elif _TARGET_ == 3
				AddSubShiftMix(Plaintext, Correct_Key);
				Key = Plaintext[Byte];
#else
				printf(" -----------------------------------------------------------------------\n");
				printf("|                          Failed to Set Target                         |\n");
				printf(" -----------------------------------------------------------------------\n");
				return _FAIL_;
#endif

				// Hamming Weight 계산
				Key_HW = (Key & 1) + ((Key >> 1) & 1) + ((Key >> 2) & 1) + ((Key >> 3) & 1) + ((Key >> 4) & 1) + ((Key >> 5) & 1) + ((Key >> 6) & 1) + ((Key >> 7) & 1);

				// E[X], E[X^2] 계산
				H_S[Guess_Key] += Key_HW;
				H_SS[Guess_Key] += (Key_HW * Key_HW);

				// E[XY] 계산
				for (m = 0; m < Interval_Point; m++)
					W_HS[Guess_Key][m] += (double)Key_HW * Temp_Points[m];
			} // Guess_Key
		} // n (_TRACE_NUM_)

		printf(" -----------------------------------------------------------------------\n");
		printf("|              Correlation Power Analysis On Key Candidates             |\n");
		printf(" -----------------------------------------------------------------------\n");

		// 키에 대한 상관계수 계산 및 결과 값 저장
		for (Guess_Key = 0; Guess_Key < Guess_Key_Num; Guess_Key++) {
#if _TARGET_
			sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%s\\%d_Byte\\%03d(0x%02X).txt", FOLD_MERGE, Byte + 1, Correct_Key[Byte] + Guess_Key, Correct_Key[Byte] + Guess_Key);
#else
			sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%s\\%d_Byte\\%d.txt", FOLD_MERGE, Byte + 1, Byte + 1);
#endif
			fopen_s(&fp, FILE_MERGE, "w");

			for (m = 0; m < Interval_Point; m++) {
				Correlation_L = (double)_TRACE_NUM_ * W_HS[Guess_Key][m] - W_CS[m] * (double)H_S[Guess_Key];
				Correlation_R = ((double)_TRACE_NUM_ * (double)H_SS[Guess_Key] - (double)H_S[Guess_Key] * (double)H_S[Guess_Key]) * ((double)_TRACE_NUM_ * W_CSS[m] - W_CS[m] * W_CS[m]);

				if (Correlation_R <= (double)0)
					Correlation = (double)0;
				else {
					Correlation = Correlation_L / sqrt(Correlation_R);
					Correlation = fabs(Correlation);
				}

				fprintf_s(fp, "%f\n", Correlation);

				if (MaxPeak[Guess_Key] < Correlation) {
					MaxPeak[Guess_Key] = Correlation;

					// Known Key 분석 또는 Plaintext Load 분석 시 상관계수가 높게 뜨는 지점 출력
#if _METHOD_ || !_TARGET_
					Index = (unsigned int)m;

					sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%s\\%d_Result.txt", FOLD_MERGE, Byte + 1);
					fopen_s(&fp2, FILE_MERGE, "w");

					fprintf_s(fp2, "Point		: %d\n", Index + _START_POINT_);
					fprintf_s(fp2, "Correlation	: %f", MaxPeak[Guess_Key]);

					fclose(fp2);
#endif
				}
			}

			fclose(fp);
		}

#if !_METHOD_ && _TARGET_
		// MAX PEAK 파일로 저장
		sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%s\\%d_MAX_PEAK.txt", FOLD_MERGE, Byte + 1);
		fopen_s(&fp, FILE_MERGE, "w");

		// 최종 분석 키 저장
		sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%s\\%d_RIGHT_KEY.txt", FOLD_MERGE, Byte + 1);
		fopen_s(&fp2, FILE_MERGE, "w");

		Max = 0;
		for (Guess_Key = 0; Guess_Key < Guess_Key_Num; Guess_Key++) {
			fprintf_s(fp, "%f\n", MaxPeak[Guess_Key]);

			if (Max < MaxPeak[Guess_Key]) {
				Max = MaxPeak[Guess_Key];
				R_Key = Guess_Key;
			}
		}

		fclose(fp);

		fprintf_s(fp2, "  1st  0x%02X  %f\n", R_Key, Max);
		solbit[Byte] = R_Key;

		MaxPeak[R_Key] = 0.0;

		for (i = 1; i < Candidates; i++) {
			for (Guess_Key = 0; Guess_Key < Guess_Key_Num; Guess_Key++) {
				if (Max_Sec < MaxPeak[Guess_Key]) {
					Max_Sec = MaxPeak[Guess_Key];
					Right_Key = Guess_Key;
				}
			}

			if (i == 1) {
				fprintf_s(fp2, "  2nd  0x%02X  %f\n", Right_Key, Max_Sec);
#if _TARGET_ != 1
				Ratio = Max / Max_Sec;
#endif
			}
			else if (i == 2) {
				fprintf_s(fp2, "  3rd  0x%02X  %f\n", Right_Key, Max_Sec);
#if _TARGET_ == 1
				Ratio = Max / Max_Sec;
#endif
			}
			else
				fprintf_s(fp2, "%3dth  0x%02X  %f\n", i + 1, Right_Key, Max_Sec);

			MaxPeak[Right_Key] = 0.0;
			Max_Sec = 0.0;
			Right_Key = 0;
		}

		fprintf_s(fp2, "\nRatio  %f\n", Ratio);

		fclose(fp2);
#endif
	}

	fclose(fpp);
	fclose(fpt);

	/********************************************************************************************/
	/*                                       동적 할당 해제                                     */
	/********************************************************************************************/
	free(Plaintext);
	free(Correct_Key);
	free(Temp_Points);
	free(H_S);
	free(H_SS);
	free(W_CS);
	free(W_CSS);
	for (Guess_Key = 0; Guess_Key < Guess_Key_Num; Guess_Key++)
		free(W_HS[Guess_Key]);
	free(W_HS);
	free(MaxPeak);

	for (i = 0; i < 16; i++)
	{
		master_key_bits[i] = solbit[i] & 1;
		master_key_bits[16 + i] = solbit[i] >> 1;
	}

	unsigned char input[16] = { 0, };
	//bits to input
	for (i = 0; i < 16; i++) {
		solbit[i] = 0;
		for (int j = 0; j < 4; j++) {
			input[i] ^= master_key_bits[4 * i + j] << j;
		}
	}

	fp = fopen("RightKey.txt", "w");

	for (i = 0; i < 8; i++)
	{
		fprintf(fp,"%01X", input[i]);
		if ((i + 1) % 2 == 0)
			fprintf(fp," ");
	}

	fclose(fp);

	return _PASS_;
}







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
	unsigned char dt[64] = { 0, };
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