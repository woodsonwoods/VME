#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "792control.h"

int main()
{
	//rpcopen("192.168.0.189");

	//Initialization
	InitializeOutputFiles();
	EmptyEnable();
	DataReset();
	FindModelVersion();
	SetThresholds();
	int quit = 0;
	while (!quit)
	{
		while (DataPrepared())
		{
			ReadAnEventFrom792();
		}
		if (_kbhit())
		{
			char c = _getche();
			if (c == 'q')
			{
				quit = 1;
			}
		}
	}

	rpcclose();
}