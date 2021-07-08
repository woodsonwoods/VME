#ifndef __792CONTROL__
#define __792CONTROL__

#include "windows.h"
#include "rpccalls.h"
#include <stdio.h>
#include <conio.h>   
#include <stdlib.h>
#include <string.h>
#include <time.h>

//发生错误时，应data reset而不是程序中断

#define DELAY			Sleep(100)
/*******************************************************************************/
/*                              Global Variables                               */
/*******************************************************************************/
#define BASE_ADDR_792			0xEE0000

#define BIT_SET_1_REGISTER		0x1006
#define BIT_CLEAR_1_REGISTER	0x1008
#define BIT_SET_2_REGISTER		0x1032
#define BIT_CLEAR_2_REGISTER	0x1034
#define STATUS_1_REGISTER		0x100E

#define EVENT_COUNT_RESET		0x1040
#define IPED_REGISTER			0x1060

// DATA
#define DATATYPE_MASK			0x06000000
#define DATATYPE_HEADER			0x02000000
#define DATATYPE_CHDATA			0x00000000
#define DATATYPE_EOB			0x04000000
#define DATATYPE_FILLER			0x06000000
FILE* of_allchannels = NULL;
char path[255];

// Version
#define VERSION_REGISTER		0x8032
char version_792[3];
int nb_ch_792 = 0;

// Thresholds
#define THRESHOLD_ADDRESS	0x1080
int thresholds[32] = { 0 };

// Random Memory Access Test Mode
#define W_MEMORY_TEST_ADDRESS	0x1036
#define R_MEMORY_TEST_ADDRESS	0x1064
#define MEMORY_TEST_WORD_HIGH	0x1038
#define MEMORY_TEST_WORD_LOW	0x103A

// Acquisition Test Mode
#define TEST_EVENT_WRITE		0x103E

/*******************************************************************************/
/*                                Read & Write                                 */
/*******************************************************************************/
short read_reg_short(short shortaddr)
{
	short readdata;
	ReadVMEA24Short(BASE_ADDR_792 + shortaddr, &readdata);
	return readdata;
}
long read_reg_long(short shortaddr)
{
	long readdata;
	ReadVMEA24Long(BASE_ADDR_792 + shortaddr, &readdata);
	return readdata;
}
int write_reg_short(short shortaddr, short writedata)
{
	return WriteVMEA24Short(BASE_ADDR_792 + shortaddr, writedata);
}
int write_reg_long(short shortaddr, long writedata)
{
	return WriteVMEA24Long(BASE_ADDR_792 + shortaddr, writedata);
}

/*******************************************************************************/
/*                               Initialization                                */
/*******************************************************************************/
void InitializeOutputFiles()
{
	sprintf_s(path, sizeof(path), ".");
}
void EmptyEnable()
{
	write_reg_short(BIT_SET_2_REGISTER, 0x5000);
	DELAY;
}

void DataReset()
{
	write_reg_short(BIT_SET_2_REGISTER, 0x4);
	DELAY;
	write_reg_short(BIT_CLEAR_2_REGISTER, 0x4);
	DELAY;
}

int FindModelVersion()
{
	short sdata = read_reg_short(VERSION_REGISTER);
	short ver = sdata & 0xFF;
	switch (ver)
	{
	case 0x11:
		strcpy_s(version_792, 3, "AA");
		nb_ch_792 = 32;
		return 1;
	case 0x13:
		strcpy_s(version_792, 3, "AC");
		nb_ch_792 = 32;
		return 1;
	case 0xE1:
		strcpy_s(version_792, 3, "NA");
		nb_ch_792 = 16;
		return 1;
	case 0xE3:
		strcpy_s(version_792, 3, "NC");
		nb_ch_792 = 16;
		return 1;
	default:
		strcpy_s(version_792, 3, "-");
		printf("ERROR: Can't find the version and the number of channels of 792\n");
		return 0;
	}
}

void SetThresholds()
{
	for (int i = 0; i < nb_ch_792; i++)
	{
		if (nb_ch_792 == 16)
		{
			write_reg_short(THRESHOLD_ADDRESS + i * 4, thresholds[i] / 16);
			DELAY;
		}
		else
		{
			write_reg_short(THRESHOLD_ADDRESS + i * 2, thresholds[i] / 16);
			DELAY;
		}
	}

}

/*******************************************************************************/
/*                               Read An Event                                 */
/*******************************************************************************/
int DataPrepared()
{
	short sr1data = read_reg_short(STATUS_1_REGISTER);
	return sr1data & 0x1;
}

int ReadAnEventFrom792()
{
	int ADCdata[32];
	for (int i = 0; i < nb_ch_792; i++)
	{
		ADCdata[i] = 0;
	}
	// Header
	long longword = read_reg_long(0x0000);
	if ((longword & DATATYPE_MASK) != DATATYPE_HEADER)
	{
		printf("ERROR: The First Word is not a Header\n");
		return 0;
	}
	int nch = (longword >> 8) & 0x3F; // number of accepted channels

	// Channel data
	for (int i = 0; i < nch; i++)
	{
		longword = read_reg_long(0x0000);
		if ((longword & DATATYPE_MASK) != DATATYPE_CHDATA)
		{
			printf("ERROR: This Word is not a Datum\n");
			return 0;
		}
		int channelid;
		if (nb_ch_792 == 16)
		{
			channelid = (int)((longword >> 17) & 0x3F);
		}
		else
		{
			channelid = (int)((longword >> 16) & 0x3F);
		}
		ADCdata[channelid] = longword & 0xFFF;
	}

	// EOB
	longword = read_reg_long(0x0000);
	if ((longword & DATATYPE_MASK) != DATATYPE_EOB)
	{
		printf("ERROR: This Word is not a Datum\n");
		return 0;
	}
	else
	{
		// output
		char tmp[255];
		sprintf_s(tmp, sizeof(tmp), "%s\\792.txt", path);
		fopen_s(&of_allchannels, tmp, "w");
		if (of_allchannels == NULL)
			printf("ERROR: Can't open output file for writing\n");
		else
		{
			time_t now;
			time(&now);
			struct tm t;
			localtime_s(&t, &now);
			fprintf(of_allchannels, "%d/%d/%d-%d:%d:%d\t", 1900 + t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

			fprintf(of_allchannels, "Event_Num. %d\tADCvalues\t", longword & 0xFFFFFF);
			for (int i = 0; i < nb_ch_792; i++)
			{
				fprintf(of_allchannels, "%d\t", ADCdata[i]);
			}
			fprintf(of_allchannels, "\n");
		}
	}
	return 1;
}


/*******************************************************************************/
/*                            End of the Program                               */
/*******************************************************************************/
void Close792()
{
	if (of_allchannels != NULL) fclose(of_allchannels);
}

#endif