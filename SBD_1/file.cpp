#include <iomanip>
#include "file.h"

File::File()
{
}

File::File(const std::string& fileName, int mode, unsigned int* counter) : counter(counter)
{
	Open(fileName, mode);
}

File::~File()
{
	Close();
}

void File::Open(const std::string & fileName, int mode)
{
	this->fileName = fileName;
	file.open(fileName, mode);
	series = 0;
	dummies = 0;
	ResetPosition();
}

void File::Close()
{
	ForceWrite();
	file.close();
}

void File::WriteNextRecord(const Record& record, bool benchamark)
{
	if (last > record || series == 0)
		series++;

	buffer.data[pageOffset] = record;
	last = record;

	IncrementOffset();

	if (pageOffset == 0)
		WritePage(benchamark);
}

void File::ForceWrite(bool benchamark)
{
	if (pageOffset != 0)
		WritePage(benchamark);
}

Record File::ReadRecord(bool benchamark)
{
	if (lastPageId < currentPageId)
		ReadPage(benchamark);

	return buffer.data[pageOffset];
}

Record File::ReadNextRecord(bool benchamark)
{
	auto r = ReadRecord(benchamark);

	IncrementOffset();
	
	if (!buffer.data[pageOffset].isInitialized())
		eof = true;

	return r;
}

void File::SwitchToReadMode()
{
	ForceWrite();
	file.close();
	file.open(fileName, DEFAULT_INPUT_MODE);
	ResetPosition();
}

void File::SwitchToWriteMode()
{
	file.close();
	file.open(fileName, DEFAULT_OUTPUT_MODE);
	series = 0;
	ResetPosition();
}

void File::ResetPosition()
{
	lastPageId = 0;
	currentPageId = 1;
	pageOffset = 0;
	eof = false;
	last = { Record::UNINIT, Record::UNINIT };
}

void File::IncrementOffset()
{
	pageOffset++;
	pageOffset %= Page::PAGE_SIZE;
	if (pageOffset == 0)
		currentPageId++;
}

void File::ClearBuffer()
{
	for (int i = 0; i < Page::PAGE_SIZE; i++)
		buffer.data[i] = { Record::UNINIT, Record::UNINIT };
}

void File::WritePage(bool benchamark)
{
	if (benchamark && counter != nullptr)
		(*counter)++;
	auto binaryData = buffer.GetRawData();
	file.write(binaryData.get(), Page::BYTE_SIZE);

	ClearBuffer();
}


bool File::ReadPage(bool benchamark)
{
	char tmp[Page::BYTE_SIZE];	//Page::BYTE_SIZE is constexpr => valid memory alocation
	if(!eof && file.read(tmp, Page::BYTE_SIZE))
		buffer.ParseRawData(tmp);
	else
	{
		ClearBuffer();
		eof = true;
		return false;
	}
	if (benchamark && counter != nullptr)
		(*counter)++;
	lastPageId++;
	return true;
}

void Merge(File& tape1, File& tape2, File& result) // assumption: tape1.TotalSeries() >= tape2.TotalSeries()
{
	Record r1, r2, last1, last2;
	MergeDummies(tape1, tape2, result); //if tape1.dummies == 0 does not have any effect
	unsigned int i = 0;
	auto n = tape2.series; //assing series counter to n, because MergeDummies() affects series counter
	while (i < n)
	{
		r1 = tape1.ReadRecord();
		r2 = tape2.ReadRecord();

		if (last1 > r1 || tape1.eof || !r1.isInitialized())		//end of actual series on tape1 or end of tape1
		{
			//rewrite rest of actual series from tape2
			//std::cout << tape1.fileName << ": end of series " << r1 << std::endl;
			tape1.dummies++;
			tape1.series--;
			last1 = { Record::UNINIT, Record::UNINIT };
			last2 = { Record::UNINIT, Record::UNINIT };
			MergeDummies(tape1, tape2, result);
			i++;
		}
		else if (last2 > r2 || tape2.eof || !r2.isInitialized())	//end of actual series on tape2 or end of tape2
		{
			//rewrite rest of actual series from tape1
			//std::cout << tape2.fileName << ": end of series " << r2 << std::endl;
			tape2.dummies++;
			tape2.series--;
			last1 = { Record::UNINIT, Record::UNINIT };
			last2 = { Record::UNINIT, Record::UNINIT };
			MergeDummies(tape2, tape1, result);
			i++;
		}
		else if (r2 > r1)					//write r1 to result
		{
			result.WriteNextRecord(r1);
			last1 = r1;
			last2 = r2;
			tape1.ReadNextRecord();
		}
		else								//write r2 to result
		{
			result.WriteNextRecord(r2);
			last1 = r1;
			last2 = r2;
			tape2.ReadNextRecord();
		}

	}
	result.SwitchToReadMode();
	tape2.SwitchToWriteMode();
}

void MergeDummies(File& tape1, File& tape2, File& result)
{
	Record current, last;
	unsigned int i = 0;
	while (i < tape1.dummies)
	{
		current = tape2.ReadRecord();
		if (last > current || tape2.eof)
		{
			i++;
			last = { Record::UNINIT, Record::UNINIT };
			continue;
		}
		last = current;
		result.WriteNextRecord(current);
		tape2.ReadNextRecord();
	}

	tape2.series -= i;
	tape1.dummies -= i;
}

std::ostream & operator<<(std::ostream & os, File & tape)
{
	os << tape.fileName << " S: " << tape.series << " D: " << tape.dummies << "\n";
	os << "\tPage:Offset " << tape.currentPageId << ":" << tape.pageOffset << " eof " << tape.eof << "\n";

	unsigned int i = 0;
	Record r;
	auto bckPageOffset = tape.pageOffset;
	auto bckPageId = tape.currentPageId;

	if (tape.series == 0)
		return os;

	while (!tape.eof)
	{
		r = tape.ReadNextRecord(false);
		if (!r.isInitialized())
			break;
		os << "\t" << std::setw(3) << std::right << i++ << "." << r << "\n";
	}
	tape.file.clear();
	tape.file.seekg(0, std::ios::beg);
	tape.ResetPosition();
	while (tape.currentPageId < bckPageId)
		tape.ReadNextRecord(false);
	while (tape.pageOffset < bckPageOffset)
		tape.ReadNextRecord(false);

	return os;
}

