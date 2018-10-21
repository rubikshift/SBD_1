#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

#include <cstdlib>
#include <ctime>

#include "file.h"

void log(std::string text);
double random();
void distribute(File& source, File tapes[3]);
int sort(File tapes[3]);

int main(int argc, char* argv[])
{
	int n;
	double v, m;
	std::stringstream stream;
	std::string fileName = "sbd1.data";
	Record r;
	File dataFile;

	unsigned int IOcounter = 0;

	File tapes[] = { File("tape1", File::DEFAULT_TAPE_MODE), File("tape2", File::DEFAULT_TAPE_MODE), File("tape3", File::DEFAULT_TAPE_MODE) };

	//srand(time(NULL));

	if (argc != 3)
	{
		if (argc < 3)
			std::cout << "To few arguments" << std::endl;
		else
			std::cout << "To many arguments" << std::endl;
		std::cout << "Help:" << std::endl;
		std::cout << "\tsbd_1.exe f file" << std::endl;
		std::cout << "\t\t reads records from file" << std::endl;
		std::cout << "\tsbd_1.exe r n" << std::endl;
		std::cout << "\t\t randomly generates n records" << std::endl;
		std::cout << "\tsbd_1.exe m n" << std::endl;
		std::cout << "\t\t user has to manualy type n records" << std::endl;
		return 1;
	}
	
	log("START PREPARING");
	stream << argv[2];

	switch (argv[1][0])
	{
		case 'f': 
			stream >> fileName;
			dataFile.Open(fileName);
			break;
		case 'r':
			stream >> n;
			dataFile.Open(fileName, File::DEFAULT_TAPE_MODE);
			for (int i = 0; i < n; i++)
			{
				r = Record(random(), random());
				std::cout << r << std::endl;

				dataFile.WriteNextRecord(r);
			}
			dataFile.Close();
			dataFile.Open(fileName);
			break;
		case 'm': 
			stream >> n;
			dataFile.Open(fileName, File::DEFAULT_TAPE_MODE);
			for (int i = 0; i < n; i++)
			{
				std::cin >> v;
				std::cin >> m;
				r = Record(v, m);
				std::cout << r << std::endl;

				dataFile.WriteNextRecord(r);
			}
			dataFile.Close();
			dataFile.Open(fileName);
			break;
	}
	log("END PREPARING");

	log("START DISTRIBUTION");
	dataFile.ResetPosition();
	distribute(dataFile, tapes);
	log("END DISTRIBUTION");

	log("START SORTING");
	int x = sort(tapes);
	log("END SORTING");
	return 0;
}

void log(std::string stage)
{
	std::cout << std::setw(80) << std::setfill('-') << std::left << "---" + stage << std::endl;
	std::cout << std::setfill(' ');
}

double random()
{
	auto base = rand() % 1000 + 1;
	return (double)base / 100.0;
}

void distribute(File & source, File tapes[3])
{
	unsigned int goal = 1;
	unsigned int currentTape = 0;
	Record r, last;

	while (!source.eof)
	{
		r = source.ReadNextRecord();
		if (last > r && tapes[currentTape].series == goal)
		{
			goal = tapes[0].series + tapes[1].series;
			currentTape++;
			currentTape %= 2;
		}
		last = r;
		std::cout << r << std::endl;
		tapes[currentTape].WriteNextRecord(r);
	}
	tapes[currentTape].dummies = goal - tapes[currentTape].series;
	
	tapes[0].ForceWrite();
	tapes[1].ForceWrite();

	tapes[0].ResetPosition();
	tapes[1].ResetPosition();
	tapes[2].ResetPosition();
}

int sort(File tapes[3])
{
	int a, b, c = 2;
	if (tapes[0].series + tapes[0].dummies > tapes[1].series + tapes[1].dummies)
	{
		a = 0;
		b = 1;
	}
	else
	{
		a = 1;
		b = 0;
	}

	std::cout << tapes[0].size << " | " << tapes[1].size << " | " << tapes[2].size << std::endl;
	std::cout << tapes[0].series << " | " << tapes[1].series << " | " << tapes[2].series << std::endl;
	std::cout << "-----" << std::endl;


	while (tapes[0].series + tapes[1].series + tapes[2].series != 1)
	{
		Merge(tapes[a], tapes[b], tapes[c]);
		a = (a + 1) % 3;
		b = (b + 1) % 3;
		c = (c + 1) % 3;
		std::cout << tapes[0].size << " | " << tapes[1].size << " | " << tapes[2].size << std::endl;
		std::cout << tapes[0].series << " | " << tapes[1].series << " | " << tapes[2].series << std::endl;
		std::cout << "-----" << std::endl;
	}

	if (tapes[a].series == 1)
		return a;
	else if (tapes[b].series == 1)
		return b;
	else
		return c;
}
