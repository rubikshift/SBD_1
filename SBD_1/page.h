#pragma once
#include <memory>
#include <iostream>
#include "record.h"

struct Page
{
	Page();
	Page(const char* ptr);
	const char* GetRawData() const;

	static const unsigned int PAGE_SIZE = 16; //Page size == number of records
	static constexpr auto BYTE_SIZE = PAGE_SIZE * Record::RECORD_SIZE;
	Record data[PAGE_SIZE];
};

std::ostream& operator<<(std::ostream& os, const Page& page);