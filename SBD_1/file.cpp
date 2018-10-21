#include "file.h"

File::File()
{
}

File::File(const std::string& fileName, int mode)
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
	size = 0;
	ResetPosition();
}

void File::Close()
{
	ForceWrite();
	file.close();
}

void File::WriteNextRecord(const Record& record)
{
	if (last > record || series == 0)
		series++;
	size++;
	buffer.data[pageOffset] = record;
	last = record;

	IncrementOffset();

	if (pageOffset == 0)
		WritePage();
}

void File::ForceWrite()
{
	if (pageOffset != 0)
		WritePage();
	pageOffset = 0;
}

Record File::ReadRecord()
{
	if (lastPageId < currentPageId)
		ReadPage();

	auto r = buffer.data[pageOffset];
	last = r;
	return r;
}

Record File::ReadNextRecord()
{
	size--;
	auto r = ReadRecord();

	IncrementOffset();
	
	if (!buffer.data[pageOffset].isInitialized())
		eof = true;

	return r;
}

void File::ResetPosition()
{
	file.seekg(std::ios::beg);
	file.seekp(std::ios::beg);
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

void File::PrintTape()
{
	std::cout << fileName << " S: " << series << " D: " << dummies << std::endl;
	ResetPosition();
	while (!eof && series != 0)
		std::cout << "\t" << ReadNextRecord() << std::endl;
}

void File::ClearBuffer()
{
	for (int i = 0; i < Page::PAGE_SIZE; i++)
		buffer.data[i] = { Record::UNINIT, Record::UNINIT };
}

void File::WritePage()
{
	auto binaryData = buffer.GetRawData();
	file.write(binaryData, Page::BYTE_SIZE);

	ClearBuffer();
}


bool File::ReadPage()
{
	ClearBuffer();

	if (file.eof() || eof)
		return eof = false;

	lastPageId++;
	char tmp[Page::BYTE_SIZE];	//Page::BYTE_SIZE is constexpr => valid memory alocation
	file.read(tmp, Page::BYTE_SIZE);
	buffer.ParseRawData(tmp);

	return true;
}

void Merge(File& tape1, File& tape2, File& result) // assumption: tape1.TotalSeries() >= tape2.TotalSeries()
{
	Record r1, r2, last1, last2;
	tape1.ResetPosition();
	result.ResetPosition();

	MergeDummies(tape1, tape2, result); //if tape1.dummies == 0 does not have any effect
	unsigned int i = 0;
	auto n = tape2.series; //assing series counter to n, because MergeDummies() affects series counter
	while (i < n)
	{
		r1 = tape1.ReadRecord();
		if (last1 > r1 || tape1.eof) //End of series on tape1, write rest of actual series from tape 2
		{
			tape1.dummies++;
			tape1.series--;
			last1 = { Record::UNINIT, Record::UNINIT };
			MergeDummies(tape1, tape2, result);
			i++;
			continue;
		}

		r2 = tape2.ReadRecord();
		if (last2 > r2 || tape2.eof)//End of series on tape2, write rest of actual series from tape1
		{
			tape2.dummies++;
			tape2.series--;
			last2 = { Record::UNINIT, Record::UNINIT };
			MergeDummies(tape2, tape1, result);
			i++;
			continue;
		}
		last1 = r1;
		last2 = r2;

		if (r2 > r1) //write r1 to result
		{
			result.WriteNextRecord(r1);
			tape1.ReadNextRecord();
		}
		else //write r2 to result
		{
			result.WriteNextRecord(r2);
			tape2.ReadNextRecord();
		}
	}
	result.ForceWrite();
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

