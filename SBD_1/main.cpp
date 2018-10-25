#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include "file.h"

const unsigned int TAPES_COUNT = 3;

void log(std::string text);
double random();
unsigned int distribute(File& source, File tapes[TAPES_COUNT]);
unsigned int sort(File tapes[TAPES_COUNT]);

int main(int argc, char* argv[])
{
	int n;
	int v, m;
	std::stringstream stream;
	std::string fileName = "sbd1.data";
	Record r;
	File dataFile;

	unsigned int IOcounter = 0;

	File tapes[TAPES_COUNT] = { 
		File("tape1", File::DEFAULT_TAPE_MODE, &IOcounter), 
		File("tape2", File::DEFAULT_TAPE_MODE, &IOcounter),
		File("tape3", File::DEFAULT_TAPE_MODE, &IOcounter) 
	};

	//std::srand(std::time(nullptr));

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
				//std::cout << r << std::endl;

				dataFile.WriteNextRecord(r);
			}
			dataFile.Close();
			dataFile.Open(fileName);
			break;
	}
	log("END PREPARING");

	log("START DISTRIBUTION");
	dataFile.ResetPosition();
	auto series = distribute(dataFile, tapes);
	std::cout << "\tSeries counter: " << series << std::endl;
	std::cout << std::endl;
	log("END DISTRIBUTION");

	log("START SORTING");
	auto phases = sort(tapes);
	double expectedPhases = 1.04 * log(series) / log(2);
	double diffPhases = abs((double)phases - expectedPhases);
	diffPhases *= 100;
	diffPhases /= expectedPhases;
	std::cout << "\nPhases: " << phases << " | Expected: " << expectedPhases << " | Diff [%] " << diffPhases << std::endl;
	std::cout << std::endl;
	log("END SORTING");
	
	for (unsigned int i = 0; i < TAPES_COUNT; i++)
		if(tapes[i].series == 1)
			tapes[i].PrintTape();

	double expected = ((1.04 * log(series) / log(2)) + 1.0);
	expected *= 2;
	expected /= Page::PAGE_SIZE;
	expected *= n;
	double diff = abs((double)IOcounter - expected);
	diff *= 100;
	diff /= expected;
	std::cout << "\nIOCounter: " << IOcounter << " | Expected: " << expected << " | Diff [%] " <<  diff << std::endl;

	return 0;
}

void log(std::string stage)
{
	std::cout << std::setw(80) << std::setfill('-') << std::left << "---" + stage << std::endl;
	std::cout << std::setfill(' ');
}

double random()
{
	auto base = std::rand() % 10 + 1;
	return (double)base / 1.0;
}

unsigned int distribute(File & source, File tapes[TAPES_COUNT])
{
	unsigned int goal = 1;
	unsigned int currentTape = 0;
	unsigned int series = 0;
	Record r, last;
	while (!source.eof)
	{
		r = source.ReadNextRecord();
		if (!r.isInitialized()) // equal to eof
			break;
		if (last > r) 
		{
			series++;
			if ((tapes[currentTape].series == goal))
			{
				goal = tapes[0].series + tapes[1].series;
				currentTape++;
				currentTape %= (TAPES_COUNT - 1);
			}
		}
		last = r;
		tapes[currentTape].WriteNextRecord(r);
	}
	tapes[currentTape].dummies = goal - tapes[currentTape].series;
	
	tapes[0].ForceWrite();
	tapes[1].ForceWrite();

	for(unsigned int i = 0; i < TAPES_COUNT; i++)
		tapes[i].ResetPosition();

	return series;
}

void printPhase(File tapes[TAPES_COUNT], unsigned int phase)
{
	std::cout << "\tPhase " << phase << ": \t";
	for (unsigned int i = 0; i < TAPES_COUNT; i++)
	{
		std::cout << tapes[i].series;
		if (tapes[i].dummies > 0)
			std::cout << " (" << tapes[i].dummies << ")";
		if (i != TAPES_COUNT - 1)
			std::cout << " | ";
	}
	std::cout << std::endl;
	for (unsigned int i = 0; i < TAPES_COUNT; i++)
		tapes[i].PrintTape();
}

unsigned int sort(File tapes[TAPES_COUNT])
{
	int a, b, c = 2, tmp;
	
	if (tapes[0].series + tapes[0].dummies > tapes[1].series + tapes[1].dummies)
	{
		a = 0;	b = 1;
	}
	else
	{
		a = 1; b = 0;
	}

	unsigned int phases = 0;
	printPhase(tapes, phases);

	while (tapes[0].series + tapes[1].series + tapes[2].series != 1)
	{
		Merge(tapes[a], tapes[b], tapes[c]);

		phases++;
		printPhase(tapes, phases);

		tmp = c;
		c = b; 
		b = a;
		a = tmp;
	}
	return phases;
}
