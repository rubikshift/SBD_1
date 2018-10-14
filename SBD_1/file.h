#pragma once
#include <string>
#include <fstream>
#include "page.h"

class File
{
	public:
		File(std::string fileName);
		~File();

		void Write(const Page& page);
		void Write(const Record& record);
		Page Read();
		Record Read(unsigned int pageOffset);

	private:
		std::fstream file;

};