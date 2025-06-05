#include "WeatherData.h"

WeatherData::WeatherData()
{
	this->sensors.wind_speed = 0;
	this->sensors.wind_direction = UNDEFINED;
	this->sensors.rainfall = 0;
	this->sensors.cloud_coverage = CLEAR;
	this->season.part_of_day = NIGHT;
	this->sensors.temperature = 0;
	this->sensors.humidity = 0;
	this->sensors.feels_like = 0;
	this->sensors.UV_index = 0;
	this->sensors.max = 0;
	this->sensors.min = 0;
	this->season.day_of_year = 1;
	this->season.hour = 0;
	this->season.sunrise = "00:00";
	this->season.sunset = "00:00";
	this->season.date = "01.01.0001";
	this->season.time = "00:00";
	this->configureCOMPort();
	system("cls");
	cout << "LOADING & MEASURING DATA. PLEASE WAIT...";
	SetConsoleOutputCP(CP_UTF8);
}

WeatherData::~WeatherData()
{
	this->com2.buffer = '0';
	WriteFile(this->com2.serialHandle, &(this->com2.buffer), 1, &(this->com2.written), NULL);
	CloseHandle(this->com2.serialHandle);
}

void WeatherData::configureCOMPort()
{
	this->com2.serialHandle = CreateFile(L"COM2", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	this->com2.serialParameters = { 0 };
	this->com2.serialParameters.DCBlength = sizeof(this->com2.serialParameters);
	this->com2.serialParameters.BaudRate = CBR_4800;
	this->com2.serialParameters.ByteSize = 8;
	this->com2.serialParameters.StopBits = ONESTOPBIT;
	this->com2.serialParameters.Parity = NOPARITY;
	SetCommState(this->com2.serialHandle, &(this->com2.serialParameters));

	this->com2.timeouts = { 0 };
	this->com2.timeouts.ReadIntervalTimeout = MAXDWORD;
	this->com2.timeouts.ReadTotalTimeoutConstant = 0;
	this->com2.timeouts.ReadTotalTimeoutMultiplier = 0;
	this->com2.timeouts.WriteTotalTimeoutConstant = 50;
	this->com2.timeouts.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts(this->com2.serialHandle, &(this->com2.timeouts));

	this->com2.buffer = '1';
	WriteFile(this->com2.serialHandle, &(this->com2.buffer), 1, &(this->com2.written), NULL);
	this->com2.buffer = '0';
}

void WeatherData::resetSensorData()
{
	this->sensors.wind_speed = 0;
	this->sensors.wind_direction = UNDEFINED;
	this->sensors.rainfall = 0;
	this->sensors.cloud_coverage = CLEAR;
	this->sensors.temperature = 0;
	this->sensors.humidity = 0;
}

void WeatherData::readData()
{
	string weather_data = "";
	while (this->com2.buffer != '\n')
	{
		if (ReadFile(this->com2.serialHandle, &(this->com2.buffer), 1, &(this->com2.read), NULL) && this->com2.read > 0)
		{
			weather_data = weather_data + this->com2.buffer;
		}
	}
	int i = 0;
	this->resetSensorData();
	bool negative = false;
	while (weather_data[i] != ' ')
	{
		if (weather_data[i] == '-')
		{
			negative = true;
		}
		else
		{ 
			this->sensors.temperature = this->sensors.temperature * 10 + weather_data[i] - '0';
		}
		i++;
	}
	if (negative)
	{
		this->sensors.temperature = this->sensors.temperature * (-1);
	}
	i++;
	while (weather_data[i] != ' ')
	{
		this->sensors.wind_speed = this->sensors.wind_speed * 10 + weather_data[i] - '0';
		i++;
	}
	i++;
	while (weather_data[i] != ' ')
	{
		this->sensors.wind_direction = (windvane)(this->sensors.wind_direction * 10 + weather_data[i] - '0');
		i++;
	}
	i++;
	while (weather_data[i] != ' ')
	{
		this->sensors.humidity = this->sensors.humidity * 10 + weather_data[i] - '0';
		i++;
	}
	i++;
	while (weather_data[i] != ' ')
	{
		this->sensors.rainfall = this->sensors.rainfall * 10 + weather_data[i] - '0';
		i++;
	}
	i++;
	while (weather_data[i] != '\n')
	{
		this->sensors.cloud_coverage = (clouds)(this->sensors.cloud_coverage * 10 + weather_data[i] - '0');
		i++;
	}
	this->com2.buffer = '0';
	this->calculatePredictions();
}

void WeatherData::calculatePredictions()
{
	this->calculateTimeDate();
	this->calculateFeelsLike();
	this->calculateUVIndex();
	this->calculateSunriseSunset();
	this->predictMaxMinTemp();
}

void WeatherData::calculateFeelsLike()
{
	if (this->sensors.temperature <= 10 && this->sensors.wind_speed >= 5)
	{
		int wind = pow(this->sensors.wind_speed, 0.16);
		this->sensors.feels_like = 13.12 + 0.62 * this->sensors.temperature - 11.37 * wind + 0.4 * wind;
	}
	else
	{
		if (this->sensors.temperature >= 27 && this->sensors.humidity >= 40)
		{
			this->sensors.feels_like = this->sensors.temperature + 0.1 * this->sensors.humidity - 0.7;
		}
		else
		{
			this->sensors.feels_like = this->sensors.temperature - (this->sensors.wind_speed >> 3) + 2 - this->sensors.cloud_coverage - (this->sensors.rainfall >> 3);
		}
	}

}

void WeatherData::calculateUVIndex()
{
	float seasonal = sin(3.14 / 2 * cos((this->season.day_of_year - 173) * 3.14 * 2 / 365));
	float dayp = exp(-pow((this->season.hour - 13), 2) / 18);
	float cloud = 1 - 0.15 * this->sensors.cloud_coverage;
	this->sensors.UV_index = 9 * seasonal * dayp * cloud;
}

void WeatherData::calculateTimeDate()
{
	time_t now = time(nullptr);
	tm* localTime = localtime(&now);
	this->season.day_of_year = localTime->tm_yday + 1;
	this->season.hour = localTime->tm_hour;
	if (localTime->tm_mday < 10)
	{
		this->season.date = "0";
	}
	this->season.date = this->season.date + to_string(localTime->tm_mday) + ".";
	if ((localTime->tm_mon + 1) < 10)
	{
		this->season.date = this->season.date + "0";
	} 
	this->season.date = this->season.date + to_string(localTime->tm_mon + 1) + "." + to_string(1900 + localTime->tm_year) + ".";
	if (localTime->tm_hour < 10)
	{
		this->season.time = "0";
	}
	this->season.time = to_string(localTime->tm_hour) + ":";
	if (localTime->tm_min < 10)
	{
		this->season.time = this->season.time + "0";
	} 
	this->season.time = this->season.time + to_string(localTime->tm_min);
}

void WeatherData::calculateSunriseSunset()
{
	float decl = -23.44 * cosf((2 * 3.14 / 365.0) * (this->season.day_of_year + 10));
	float latRad = NIS_LATITUDE * 3.14 / 180.0;
	float declRad = decl * 3.14 / 180.0;
	float hourAngle = acosf(-tanf(latRad) * tanf(declRad));
	float daylightHours = (2.0f * hourAngle * 180.0 / 3.14) / 15.0; 
	float solarNoon = 12.0;
	float sunriseHour = solarNoon - daylightHours / 2.0;
	float sunsetHour = solarNoon + daylightHours / 2.0;
	this->season.sunrise = "0" + to_string((int)sunriseHour) + ":" + to_string((int)((sunriseHour - (int)sunriseHour) * 60));
	this->season.sunset = to_string((int)sunsetHour) + ":" + to_string((int)((sunsetHour - (int)sunsetHour) * 60));
	if (this->season.time > this->season.sunrise && this->season.time < this->season.sunset)
	{
		this->season.part_of_day = DAY;
	}
	else
	{
		this->season.part_of_day = NIGHT;
	}
}

void WeatherData::predictMaxMinTemp()
{
	float indexCC = 0, indexH = 0, indexWS = 0;
	int sunrise = stoi(this->season.sunrise.substr(0, 2));
	int sunset = stoi(this->season.sunset.substr(0, 2));
	if (this->sensors.cloud_coverage == MOSTLY_SUNNY)
	{
		indexCC = -0.1;
	}
	if (this->sensors.cloud_coverage == PARTIALY_CLOUDY)
	{
		indexCC = -0.2;
	}
	if (this->sensors.cloud_coverage == MOSTLY_CLOUDY)
	{
		indexCC = -0.4;
	}
	if (this->sensors.cloud_coverage == CLOUDY)
	{
		indexCC = -0.6;
	}
	if (this->sensors.humidity < 30)
	{
		indexH = 0.2;
	}
	if (this->sensors.humidity > 60)
	{
		indexH = -0.15;
	}
	if (this->sensors.wind_speed > 4 && this->sensors.wind_speed < 7)
	{
		indexWS = -0.2;
	}
	if (this->sensors.wind_speed > 7)
	{
		indexWS = -0.35;
	}
	if (this->season.time < this->season.sunrise)
	{
		this->sensors.max = this->sensors.temperature + (-1.0 + indexH + indexWS) * (sunrise - this->season.hour) + (1.2 + indexCC + indexH) * (12 - sunrise) + (0.3 + indexCC + indexH) * 3;
		this->sensors.min = this->sensors.temperature + (-1.0 + indexH + indexWS) * (sunrise - this->season.hour);
	}
	else
	{
		if (this->season.hour < 12)
		{
			this->sensors.max = this->sensors.temperature + (1.2 + indexCC + indexH) * (12 - this->season.hour) + (0.3 + indexCC + indexH) * 3;
			this->sensors.min = this->sensors.temperature - (1.2 + indexCC + indexH) * (this->season.hour - sunrise);
		}
		else
		{
			if (this->season.hour < 15)
			{
				this->sensors.max = this->sensors.temperature + (0.3 + indexCC + indexH) * (15 - this->season.hour);
				this->sensors.min = this->sensors.temperature -(0.3 + indexCC + indexH) * (this->season.hour - 12) - (1.2 + indexCC + indexH) * (12 - sunrise);
			}
			else
			{
				if (this->season.time < this->season.sunset)
				{
					this->sensors.max = this->sensors.temperature - (-0.7 + indexWS + indexH) * (this->season.hour - 15);
					this->sensors.min = this->sensors.temperature - (-0.7 + indexWS + indexH) * (this->season.hour - 15) - (0.3 + indexCC + indexH) * 3 - (1.2 + indexCC + indexH) * (12 - sunrise);
				}
				else
				{
					this->sensors.max = this->sensors.temperature - (-0.7 + indexWS + indexH) * (sunset - 15) - (-1.0 + indexWS + indexH) * (this->season.hour - sunset);
					this->sensors.min = this->sensors.temperature - (-1.0 + indexWS + indexH) * (this->season.hour - sunset) - (-0.7 + indexWS + indexH) * (sunset - 15) - (0.3 + indexCC + indexH) * 3 - (1.2 + indexCC + indexH) * (12 - sunrise);
				}
			}
		}
	}
}

int WeatherData::predictHourly(int hour, int temp)
{ 
	float indexCC = 0, indexH = 0, indexWS = 0;
	int sunrise = stoi(this->season.sunrise.substr(0, 2));
	int sunset = stoi(this->season.sunset.substr(0, 2));
	if (this->sensors.cloud_coverage == MOSTLY_SUNNY)
	{
		indexCC = -0.1;
	}
	if (this->sensors.cloud_coverage == PARTIALY_CLOUDY)
	{
		indexCC = -0.2;
	}
	if (this->sensors.cloud_coverage == MOSTLY_CLOUDY)
	{
		indexCC = -0.4;
	}
	if (this->sensors.cloud_coverage == CLOUDY)
	{
		indexCC = -0.6;
	}
	if (this->sensors.humidity < 30)
	{
		indexH = 0.2;
	}
	if (this->sensors.humidity > 60)
	{
		indexH = -0.15;
	}
	if (this->sensors.wind_speed > 4 && this->sensors.wind_speed < 7)
	{
		indexWS = -0.2;
	}
	if (this->sensors.wind_speed > 7)
	{
		indexWS = -0.35;
	}
	if (hour < sunrise)
	{
		return temp - 1.0 + indexH + indexWS;
	}
	if (hour < 12)
	{
		return temp + 1.2 + indexCC + indexH;
	}
	if (hour < 15)
	{
		return temp + 0.3 + indexCC + indexH;
	}
	if (hour < sunset)
	{
		return temp - 0.7 + indexWS + indexH;
	}
	return temp - 1.0 + indexWS + indexH;
}

int WeatherData::predictDaily(int day, int tmp, bool minmax)
{
	float indexCCMin = 0, indexHMin = 0, indexWSMin = 0, indexCCMax = 0, indexHMax = 0, indexWSMax = 0;
	if (this->sensors.cloud_coverage == CLEAR)
	{
		indexCCMin = -0.4;
	}
	if (this->sensors.cloud_coverage == MOSTLY_SUNNY)
	{
		indexCCMin = -0.2;
		indexCCMax = -0.2;
	}
	if (this->sensors.cloud_coverage == PARTIALY_CLOUDY)
	{
		indexCCMin = -0.1;
		indexCCMax = -0.5;
	}
	if (this->sensors.cloud_coverage == MOSTLY_CLOUDY)
	{
		indexCCMin = 0.2;
		indexCCMax = -1.0;
	}
	if (this->sensors.cloud_coverage == CLOUDY)
	{
		indexCCMin = 0.5;
		indexCCMax = -1.5;
	}
	if (this->sensors.humidity < 30)
	{
		indexHMin = -0.3;
		indexHMax = 0.4;
	}
	if (this->sensors.humidity > 60)
	{
		indexHMin = 0.3;
		indexHMax = -0.3;
	}
	if (this->sensors.wind_speed < 4)
	{
		indexWSMin = -0.3;
		indexWSMax = 0.2;
	}
	if (this->sensors.wind_speed > 4 && this->sensors.wind_speed < 7)
	{
		indexWSMin = -0.1;
		indexWSMax = -0.2;
	}
	if (this->sensors.wind_speed > 7)
	{
		indexWSMin = -0.3;
		indexWSMax = -0.5;
	}
	if (minmax == 0)
	{
		return 0.5 * (tmp + indexCCMin + indexHMin + indexWSMin) + 1.5 * (8.55 + 9.65 * sin(2 * 3.14 * (day - 200) / 365));
	}
	return 0.5 * (tmp + indexCCMax + indexHMax + indexWSMax) + 1.5 * (19.95 + 13.65 * sin(2 * 3.14 * (day - 200) / 365));
}

void WeatherData::displayData()
{
	system("cls");
	this->modeChange();
	this->displayHeader();
	this->displayBigInfo();
	this->displayTemperature();
	this->displayWind();
	this->displayHumidity();
	this->displayRainfall();
	this->displaySunriseSunset();
	this->displayPredictions();
	this->displayWarnings();
}

void WeatherData::modeChange()
{
	if (this->season.hour > stoi(this->season.sunrise.substr(0, 2)) && this->season.hour < stoi(this->season.sunset.substr(0, 2)))
	{
		if (this->sensors.rainfall > 0)
		{
			if (this->sensors.temperature < 0)
			{
				system("color 8F");
			}
			else
			{
				system("color 79");
			}
		}
		else
		{
			if (this->sensors.cloud_coverage == CLEAR || this->sensors.cloud_coverage == MOSTLY_SUNNY || this->sensors.cloud_coverage == PARTIALY_CLOUDY)
			{
				system("color 3E");
			}
			else
			{
				system("color 87");
			}
		}
	}
	else
	{
		if (this->season.hour == stoi(this->season.sunrise.substr(0, 2)) || this->season.hour == stoi(this->season.sunset.substr(0, 2)))
		{
			if (this->sensors.rainfall > 0)
			{
				if (this->sensors.temperature < 0)
				{
					system("color 5F");
				}
				else
				{
					system("color 53");
				}
			}
			else
			{
				if (this->sensors.cloud_coverage == CLEAR || this->sensors.cloud_coverage == MOSTLY_SUNNY || this->sensors.cloud_coverage == PARTIALY_CLOUDY)
				{
					system("color DE");
				}
				else
				{
					system("color 75");
				}
			}
		}
		else
		{
			if (this->sensors.rainfall > 0)
			{
				if (this->sensors.temperature < 0)
				{
					system("color 0F");
				}
				else
				{
					system("color 09");
				}
			}
			else
			{
				system("color 0E");
			}
		}
	}
}

void WeatherData::displayHeader()
{
	cout << u8"████████████████████████████████████████████████████████████████████████████████████████\n█▄─█▀▀▀█─▄█▄─▄▄─██▀▄─██─▄─▄─█─█─█▄─▄▄─█▄─▄▄▀███─▄▄▄▄█─▄─▄─██▀▄─██─▄─▄─█▄─▄█─▄▄─█▄─▀█▄─▄█\n██─█─█─█─███─▄█▀██─▀─████─███─▄─██─▄█▀██─▄─▄███▄▄▄▄─███─████─▀─████─████─██─██─██─█▄▀─██\n▀▀▄▄▄▀▄▄▄▀▀▄▄▄▄▄▀▄▄▀▄▄▀▀▄▄▄▀▀▄▀▄▀▄▄▄▄▄▀▄▄▀▄▄▀▀▀▄▄▄▄▄▀▀▄▄▄▀▀▄▄▀▄▄▀▀▄▄▄▀▀▄▄▄▀▄▄▄▄▀▄▄▄▀▀▄▄▀" << endl;
	cout << this->season.date << "     Today in Nis     " << this->season.time << endl << endl << endl;
}

void WeatherData::displayBigInfo()
{
	for (int i = 0; i < 6; i++)
	{
		if (this->sensors.temperature < 0)
		{
			cout << DIGITS[10][i];
		}
		if (this->sensors.temperature > 9 || this->sensors.temperature < -9)
		{
			cout << DIGITS[this->sensors.temperature / 10][i];
		}
		cout << DIGITS[abs(this->sensors.temperature) % 10][i];
		cout << DIGITS[11][i];
		cout << endl;
	}
	if (this->sensors.rainfall > 0)
	{
		if (this->sensors.temperature < 0)
		{
			cout << "Snowy";
		}
		else
		{
			cout << "Rainy";
		}
	}
	else
	{
		if (this->sensors.wind_speed > 30)
		{
			cout << "Windy";
		}
		else
		{
			if (this->sensors.temperature > 35)
			{
				cout << "Hot";
			}
			if (this->sensors.temperature < -5)
			{
				cout << "Cold";
			}
			if (this->sensors.temperature > -6 && this->sensors.temperature < 36)
			{
				if (this->sensors.cloud_coverage == CLEAR)
				{
					cout << "Sunny";
				}
				if (this->sensors.cloud_coverage == MOSTLY_SUNNY)
				{
					cout << "Mostly sunny";
				}
				if (this->sensors.cloud_coverage == PARTIALY_CLOUDY)
				{
					cout << "Partialy cloudy";
				}
				if (this->sensors.cloud_coverage == MOSTLY_CLOUDY)
				{
					cout << "Mostly cloudy";
				}
				if (this->sensors.cloud_coverage == CLOUDY)
				{
					cout << "Cloudy";
				}
			}
		}
	}
	cout << endl;
}

void WeatherData::displayTemperature()
{
	cout << "Temperature: " << this->sensors.temperature << u8"°" << endl;
	cout << "Feels like: " << this->sensors.feels_like << u8"°" << endl;
	cout << "UV index: " << this->sensors.UV_index << endl;
}

void WeatherData::displayWind()
{
	cout << "Wind speed: " << this->sensors.wind_speed << "km/h ";
	if (this->sensors.wind_direction == NORTH)
	{
		cout << u8"↑ N" << endl;
	}
	if (this->sensors.wind_direction == NORTHNORTHEAST)
	{
		cout << "NNE" << endl;
	}
	if (this->sensors.wind_direction == NORTHEAST)
	{
		cout << "NE" << endl;
	}
	if (this->sensors.wind_direction == EASTNORTHEAST)
	{
		cout << "ENE" << endl;
	}
	if (this->sensors.wind_direction == EAST)
	{
		cout << u8"→ E" << endl;
	}
	if (this->sensors.wind_direction == EASTSOUTHEAST)
	{
		cout << "ESE" << endl;
	}
	if (this->sensors.wind_direction == SOUTHEAST)
	{
		cout << "SE" << endl;
	}
	if (this->sensors.wind_direction == SOUTHSOUTHEAST)
	{
		cout << "SSE" << endl;
	}
	if (this->sensors.wind_direction == SOUTH)
	{
		cout << u8"↓ S" << endl;
	}
	if (this->sensors.wind_direction == SOUTHSOUTHWEST)
	{
		cout << "SSW" << endl;
	}
	if (this->sensors.wind_direction == SOUTHWEST)
	{
		cout << "SW" << endl;
	}
	if (this->sensors.wind_direction ==  WESTSOUTHWEST)
	{
		cout << "WSW" << endl;
	}
	if (this->sensors.wind_direction == WEST)
	{
		cout << u8"← W" << endl;
	}
	if (this->sensors.wind_direction == WESTNORTHWEST)
	{
		cout << "WNW" << endl;
	}
	if (this->sensors.wind_direction == NORTHWEST)
	{
		cout << "NW" << endl;
	}
	if (this->sensors.wind_direction == NORTHNORTHWEST)
	{
		cout << "NNW" << endl;
	}
}
void WeatherData::displayHumidity()
{
	cout << "Humidity: " << this->sensors.humidity << "%" << endl;
}

void WeatherData::displayRainfall()
{
	if (this->sensors.temperature < 0)
	{
		cout << "Snowfall: ";
	}
	else
	{
		cout << "Rainfall: ";
	}
	cout << this->sensors.rainfall << "mm/hr" << endl;
}

void WeatherData::displaySunriseSunset()
{
	int daytimehr = stoi(this->season.sunset.substr(0, 2)) - stoi(this->season.sunrise.substr(0, 2)) + 1;
	int sunpos = this->season.hour - stoi(this->season.sunrise.substr(0, 2));
	cout << "     ";
	for (int i = 0; i < daytimehr; i++)
	{
		if (i == sunpos)
		{
			cout << u8"☼";
		}
		else
		{
			cout << "-";
		}
	}
	cout << endl;
	cout << this->season.sunrise << string(daytimehr, ' ') << this->season.sunset << endl;
}

void WeatherData::displayPredictions()
{
	cout << "Max temperature today: " << this->sensors.max << endl;
	cout << "Min temperature today: " << this->sensors.min << endl;
	int hr;
	cout << left << setw(5) << "HOUR";
	for (int i = this->season.hour + 1; i < this->season.hour + 18; i++)
	{
		hr = i % 24;
		cout << left << setw(5) << hr;
	}
	cout << endl;
	int t = this->sensors.temperature;
	cout << left << setw(5) << "TEMP";
	for (int i = this->season.hour + 1; i < this->season.hour + 18; i++)
	{
		hr = i % 24;
		t = this->predictHourly(hr - 1, t);
		cout << left << setw(5) << t;
	}
	cout << endl << endl;
	int mo = stoi(this->season.date.substr(3, 2)), dy = stoi(this->season.date.substr(0, 2));
	cout << left << setw(7) << "DAY";
	for (int i = this->season.day_of_year + 1; i < this->season.day_of_year + 13; i++)
	{
		dy++;
		if (dy == 29 && mo == 2)
		{
			mo++;
			dy = 1;
		}
		if (dy == 31 && (mo == 4 || mo == 6 || mo == 9 || mo == 11))
		{
			mo++;
			dy = 1;
		}
		if (dy == 32 && (mo == 1 || mo == 3 || mo == 5 || mo == 7 || mo == 8 || mo == 10 || mo == 12))
		{
			mo++;
			dy = 1;
			if (mo == 13)
			{
				mo = 1;
			}
		}
		string pr = to_string(dy) + "." + to_string(mo);
		cout << left << setw(7) << pr;
	}
	cout << endl;
	int mx = this->sensors.max;
	int mn = this->sensors.min;
	cout << left << setw(7) << "MAX";
	for (int i = this->season.day_of_year + 1; i < this->season.day_of_year + 13; i++)
	{
		 mx = this->predictDaily(i, mx, 1);
		cout << left << setw(7) << mx;
	}
	cout << endl;
	cout << left << setw(7) << "MIN";
	for (int i = this->season.day_of_year + 1; i < this->season.day_of_year + 13; i++)
	{
		mn = this->predictDaily(i, mn, 0);
		cout << left << setw(7) << mn;
	}
	cout << endl;
}

void WeatherData::displayWarnings()
{
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	WORD originalAttributes = consoleInfo.wAttributes;
	WORD background = originalAttributes & 0xF0;

	string w = "RAINFALL";
	if (this->sensors.temperature > 40)
	{
		SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED);
		cout << "! EXTREME HEAT WARNING !" << endl;
	}
	else
	{
		if (this->sensors.temperature > 35)
		{
			SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED | FOREGROUND_INTENSITY);
			cout << "! EXTREME HEAT WARNING !" << endl;
		}
		else
		{
			if (this->sensors.temperature > 30)
			{
				SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED | FOREGROUND_GREEN);
				cout << "! EXTREME HEAT WARNING !" << endl;
			}
		}
	}
	if (this->sensors.temperature < 0)
	{
		SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED);
		cout << "! EXTREME COLD WARNING !" << endl;
		w = "SNOWFALL";
	}
	else
	{
		if (this->sensors.temperature < 5)
		{
			SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED | FOREGROUND_INTENSITY);
			cout << "! EXTREME COLD WARNING !" << endl;
		}
		else
		{
			if (this->sensors.temperature < 10)
			{
				SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED | FOREGROUND_GREEN);
				cout << "! EXTREME COLD WARNING !" << endl;
			}
		}
	}
	if (this->sensors.UV_index > 8)
	{
		SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED);
		cout << "! EXTREME UV INDEX WARNING !" << endl;
	}
	else
	{
		if (this->sensors.UV_index > 6)
		{
			SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED | FOREGROUND_INTENSITY);
			cout << "! EXTREME UV INDEX WARNING !" << endl;
		}
		else
		{
			if (this->sensors.UV_index > 3)
			{
				SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED | FOREGROUND_GREEN);
				cout << "! EXTREME UV INDEX WARNING !" << endl;
			}
		}
	}

	if (this->sensors.rainfall > 50)
	{
		SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED);
		cout << "! EXTREME " << w << " WARNING !" << endl;
	}
	else
	{
		if (this->sensors.rainfall > 8)
		{
			SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED | FOREGROUND_INTENSITY);
			cout << "! EXTREME " << w << " WARNING !" << endl;
		}
		else
		{
			if (this->sensors.rainfall > 3)
			{
				SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED | FOREGROUND_GREEN);
				cout << "! EXTREME " << w << " WARNING !" << endl;
			}
		}
	}
	if (this->sensors.wind_speed > 60)
	{
		SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED);
		cout << "! EXTREME WIND WARNING !" << endl;
	}
	else
	{
		if (this->sensors.wind_speed > 40)
		{
			SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED | FOREGROUND_INTENSITY);
			cout << "! EXTREME WIND WARNING !" << endl;
		}
		else
		{
			if (this->sensors.wind_speed > 20)
			{
				SetConsoleTextAttribute(hConsole, background | FOREGROUND_RED | FOREGROUND_GREEN);
				cout << "! EXTREME WIND WARNING !" << endl;
			}
		}
	}
	SetConsoleTextAttribute(hConsole, originalAttributes);
}