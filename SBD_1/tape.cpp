#include "tape.h"

Tape::Tape(const std::string & fileName, unsigned int& counter, unsigned int tapeId, int mode)
	: File(fileName, mode), counter(counter), tapeId(tapeId)
{
	series = 0;
	dummies = 0;
}

Tape::~Tape()
{
	Close();
}

void Tape::Open(const std::string & fileName, int mode)
{
	File::Open(fileName, mode);
}

unsigned int Tape::TotalSeries()
{
	return series + dummies;
}

void Tape::Print()
{
	std::cout << "Tape" << tapeId << " (S: " << series << ", D: " << dummies << "): ";
	for (unsigned int i = 0; i < dummies; i++)
		std::cout << "D ";

	auto bckTellp = file.tellg();
	auto bckTellg = file.tellg();
	auto bckPage = buffer;
	auto bckOffset = pageOffset;
	
	ResetPosition();
	while (eof); // Bypass counter
		std::cout << File::ReadNextRecord().Energy() << std::endl;

	std::cout << std::endl;

	file.seekg(bckTellg);
	file.seekp(bckTellp);
	buffer = bckPage;
	pageOffset = bckOffset;
}

//void Tape::WriteNextRecord(const Record & record)
//{
//	if (series == 0 || last > record)
//		series++;
//	File::WriteNextRecord(record);
//}

bool Tape::ReadPage()
{
	if (File::ReadPage())
	{
		counter++;
		return true;
	}
	return false;
}

void Tape::WritePage()
{
	counter++;
	std::cout << buffer;
	File::WritePage();
}

