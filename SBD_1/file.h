#pragma once
#include <string>
#include <fstream>
#include <exception>

#include "page.h"

class EndOfSeriesException : std::exception
{
};

class File
{
	friend void Merge(File& tape1, File& tape2, File& result);
	friend void MergeDummies(File& tape1, File& tape2, File& result);

	public:
		File();
		File(const std::string& fileName, int mode = DEFAULT_FILE_MODE);
		~File();

		void Open(const std::string& fileName, int mode = DEFAULT_FILE_MODE);
		void Close();
		bool eof;
		
		void WriteNextRecord(const Record& record);
		void ForceWrite();
		Record ReadRecord();
		Record ReadNextRecord();		

		void ResetPosition();
		void IncrementOffset();

		void PrintTape();

		unsigned int dummies;
		unsigned int series;
		int size;


		static const auto DEFAULT_FILE_MODE = std::ios::in | std::ios::out | std::ios::binary | std::ios::app;
		static const auto DEFAULT_TAPE_MODE = std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc;

	protected:
		std::fstream file;
		std::string fileName;
		Page buffer;
		Record last;


		unsigned int currentPageId, lastPageId;
		unsigned int pageOffset;

		void ClearBuffer();
		bool ReadPage();
		void WritePage();
};

void Merge(File& tape1, File& tape2, File& result);
void MergeDummies(File& tape1, File& tape2, File& result);