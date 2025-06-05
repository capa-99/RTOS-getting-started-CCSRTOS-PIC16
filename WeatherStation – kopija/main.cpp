#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h>
using namespace std;
#include "WeatherData.h"

WeatherData* wd = new WeatherData();

BOOL WINAPI ConsoleHandler(DWORD signal) {
	if (signal == CTRL_CLOSE_EVENT) {
		wd->~WeatherData();
	}
	return TRUE;
}

int main()
{
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	short width = 90;
	short height = 35;
	COORD bufferSize = { width, height };
	SetConsoleScreenBufferSize(hConsole, bufferSize);
	SMALL_RECT windowSize = { 0, 0, width - 1, height - 1 };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

	while (1)
	{
		wd->readData();
		wd->displayData();
	}
	
	
	return 0;
}