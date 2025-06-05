#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <ctime>
#include <math.h>
#include <iomanip>
using namespace std;

#define NIS_LATITUDE 43.3

constexpr const char* DIGITS[12][6] = {
	{
		u8"╭━━━╮ ",
		u8"┃╭━╮┃ ",
		u8"┃┃ ┃┃ ",
		u8"┃┃ ┃┃ ",
		u8"┃╰━╯┃ ",
		u8"╰━━━╯ "
	},
	{
		u8" ╭╮  ",
		u8"╭╯┃  ",
		u8"╰╮┃  ",
		u8" ┃┃  ",
		u8"╭╯╰╮ ",
		u8"╰━━╯ "
	},
	{
		u8"╭━━━╮ ",
		u8"┃╭━╮┃ ",
		u8"╰╯╭╯┃ ",
		u8"╭━╯╭╯ ",
		u8"┃ ╰━╮ ",
		u8"╰━━━╯ "
	},
	{
		u8"╭━━━╮ ",
		u8"┃╭━╮┃ ",
		u8"╰╯╭╯┃ ",
		u8"╭╮╰╮┃ ",
		u8"┃╰━╯┃ ",
		u8"╰━━━╯ "
	},
	{
		u8"╭╮ ╭╮ ",
		u8"┃┃ ┃┃ ",
		u8"┃╰━╯┃ ",
		u8"╰━━╮┃ ",
		u8"   ┃┃ ",
		u8"   ╰╯ "
	},
	{
		u8"╭━━━╮ ",
		u8"┃╭━━╯ ",
		u8"┃╰━━╮ ",
		u8"╰━━╮┃ ",
		u8"╭━━╯┃ ",
		u8"╰━━━╯ "
	},
	{
		u8"╭━━━╮ ",
		u8"┃╭━━╯ ",
		u8"┃╰━━╮ ",
		u8"┃╭━╮┃ ",
		u8"┃╰━╯┃ ",
		u8"╰━━━╯ "
	},
	{
		u8"╭━━━╮ ",
		u8"┃╭━╮┃ ",
		u8"╰╯╭╯┃ ",
		u8"  ┃╭╯ ",
		u8"  ┃┃  ",
		u8"  ╰╯  "
	},
	{
		u8"╭━━━╮ ",
		u8"┃╭━╮┃ ",
		u8"┃╰━╯┃ ",
		u8"┃╭━╮┃ ",
		u8"┃╰━╯┃ ",
		u8"╰━━━╯ "
	},
	{
		u8"╭━━━╮ ",
		u8"┃╭━╮┃ ",
		u8"┃╰━╯┃ ",
		u8"╰━━╮┃ ",
		u8"╭━━╯┃ ",
		u8"╰━━━╯ "
	},
	{
		u8"     ",
		u8"     ",
		u8"╭━━╮ ",
		u8"╰━━╯ ",
		u8"     ",
		u8"     "
	},
	{
		u8"╭━╮ ",
		u8"╰━╯ ",
		u8"    ",
		u8"    ",
		u8"    ",
		u8"    "
	}
};

enum windvane
{
	NORTH = 59,
	NORTHNORTHEAST = 154,
	NORTHEAST = 140,
	EASTNORTHEAST = 234,
	EAST = 232,
	EASTSOUTHEAST = 239,
	SOUTHEAST = 209,
	SOUTHSOUTHEAST = 224,
	SOUTH = 184,
	SOUTHSOUTHWEST = 194,
	SOUTHWEST = 98,
	WESTSOUTHWEST = 106,
	WEST = 19,
	WESTNORTHWEST = 49,
	NORTHWEST = 34,
	NORTHNORTHWEST = 80,
	UNDEFINED = 0
};

enum daypart
{
	NIGHT,
	DAY
};

enum clouds
{
	CLEAR,
	MOSTLY_SUNNY,
	PARTIALY_CLOUDY,
	MOSTLY_CLOUDY,
	CLOUDY
};

struct COMPort
{
	HANDLE serialHandle;
	DCB serialParameters;
	COMMTIMEOUTS timeouts;
	char buffer;
	DWORD written;
	DWORD read;
};

struct MCUData
{
	int wind_speed;
	enum windvane wind_direction;
	int rainfall;
	enum clouds cloud_coverage;
	int temperature;
	int humidity;
	int feels_like;
	int UV_index;
	int max;
	int min;
};

struct PCData
{
	enum daypart part_of_day;
	int day_of_year;
	int hour;
	string sunrise;
	string sunset;
	string date;
	string time;
};


class WeatherData
{
private:
	COMPort com2;
	MCUData sensors;
	PCData season;

	void configureCOMPort();
	void resetSensorData();
public:
	WeatherData();
	~WeatherData();
	void readData();
	void calculatePredictions();
	void calculateTimeDate();
	void calculateSunriseSunset();
	void calculateFeelsLike();
	void calculateUVIndex();
	void predictMaxMinTemp();
	int predictHourly(int hour, int temp);
	int predictDaily(int day, int tmp, bool minmax);
	void displayData();
	void modeChange();
	void displayHeader();
	void displayBigInfo();
	void displayTemperature();
	void displayWind();
	void displayHumidity();
	void displayRainfall();
	void displaySunriseSunset();
	void displayPredictions();
	void displayWarnings();
};