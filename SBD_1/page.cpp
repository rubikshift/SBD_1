#include <iomanip>
#include "page.h"

Page::Page()
{
}

Page::Page(const char* ptr)
{
	auto tmp = (const double*)ptr;
	for (int i = 0; i < PAGE_SIZE; i++)
	{
		data[i].v = tmp[2 * i];
		data[i].m = tmp[2 * i + 1];
	}
}

const char* Page::GetRawData() const
{
	auto ptr = new char[BYTE_SIZE];
	
	auto tmp = (double*)ptr;
	for (int i = 0; i < PAGE_SIZE; i++)
	{
		tmp[2 * i] = data[i].v;
		tmp[2 * i + 1] = data[i].m;
	}

	return (const char*)ptr;
}

std::ostream & operator<<(std::ostream & os, const Page & page)
{
	for (int i = 0; i < Page::PAGE_SIZE; i++)
		os << std::setw(5) << std::left << i << page.data[i] << std::endl;
	return os;
}
