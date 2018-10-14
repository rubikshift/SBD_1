#include "file.h"

File::File(std::string fileName)
{
	file.open(fileName, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
}

File::~File()
{
	file.close();
}

void File::Write(const Page & page)
{
	auto binaryData = page.GetRawData();
	file.write(binaryData, Page::BYTE_SIZE);
}

Page File::Read()
{
	file.seekg(std::ios_base::beg);
	auto tmp = new char[Page::BYTE_SIZE];
	file.read(tmp, Page::BYTE_SIZE);
	auto page = Page(tmp);
	delete tmp;
	return page;
}
