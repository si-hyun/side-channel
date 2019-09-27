#include "Analysis.h"

int LOG(struct tm *t, __int64 start_time, __int64 end_time)
{
	FILE	*fp = NULL;
	char	FILE_MERGE[_FILE_NAME_SIZE_] = "";

	sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%04d_%02d_%02d_%02d_%02d_%02d\\Log.csv", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	fopen_s(&fp, FILE_MERGE, "wb");
	if (fp == NULL) {
		printf(" -----------------------------------------------------------------------\n");
		printf("|                          Failed To Write Log                          |\n");
		printf(" -----------------------------------------------------------------------\n");
		return _FAIL_;
	}

	// Log ���� ��� ����
	fprintf(fp, "time = %I64d hours %I64d min %I64d second\n", (end_time - start_time) / 3600, ((end_time - start_time) / 60) % 60, (end_time - start_time) % 60);
	fprintf(fp, "���� ���               : %s\n", _FOLD_);
	fprintf(fp, "���� �̸�               : %s\n", _TRACE_FILE_);
	fprintf(fp, "�� �̸�               : %s\n", _PLAIN_FILE_);
	fprintf(fp, "���� ��	                  : %d\n", _TRACE_NUM_);
	fprintf(fp, "���� �м� ���� ���� : %d\n", _START_POINT_);
	fprintf(fp, "���� �м� ��    ���� : %d\n", _END_POINT_);

	fclose(fp);

	return _PASS_;
}

int POINT_VERIFY(unsigned int *POINTS, unsigned int *TRACE_NUM)
{
	trace_HEADER	in_trace = { 0, };
	FILE			*fp = NULL;
	char			FILE_MERGE[_FILE_NAME_SIZE_] = "";

	// ���� ���� ���� (�б� ���)
	sprintf_s(FILE_MERGE, _FILE_NAME_SIZE_ * sizeof(char), "%s\\%s.trace", _FOLD_, _TRACE_FILE_);
	fopen_s(&fp, FILE_MERGE, "rb");
	if (fp == NULL) {
		printf(" -----------------------------------------------------------------------\n");
		printf("|                          Failed To Read Trace                         |\n");
		printf(" -----------------------------------------------------------------------\n");
		return _FAIL_;
	}

	// ���� ��� �б� (32 ����Ʈ)
	fread(in_trace.strID, sizeof(unsigned char), 20, fp);
	fread(&in_trace.trace_no, sizeof(unsigned int), 1, fp);
	fread(&in_trace.point_no, sizeof(unsigned int), 1, fp);
	fread(in_trace.strEND, sizeof(unsigned char), 4, fp);

	fclose(fp);

	printf(" ---------------------------Trace Information---------------------------\n");
	printf("|	The Type Of Point	:	FLOAT				|\n");
	printf("|	The Number Of Trace	:	%d				|\n", in_trace.trace_no);
	printf("|	The Number Of Point	:	%d				|\n", in_trace.point_no);
	printf(" -----------------------------------------------------------------------\n");

	*POINTS = in_trace.point_no;
	*TRACE_NUM = in_trace.trace_no;

	return _PASS_;
}