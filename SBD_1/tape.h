#pragma once
#include "file.h"

class Tape : public File
{
	

	public:
		Tape(const std::string& fileName, unsigned int& counter, unsigned int tapeId, int mode = DEFAULT_TAPE_MODE);
		~Tape();

		void Open(const std::string& fileName, int mode = DEFAULT_TAPE_MODE);

		unsigned int TotalSeries();
		void Print();
		//void WriteNextRecord(const Record& record);

		unsigned int dummies;
		
	private:
		bool ReadPage();
		void WritePage();

		const unsigned int tapeId;
		unsigned int& counter;
		unsigned int series;
};



