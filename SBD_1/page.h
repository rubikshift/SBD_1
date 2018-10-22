#pragma once
#include <memory>
#include <iostream>
#include "record.h"

struct Page
{
	Page();

	std::unique_ptr<char[]> GetRawData() const;
	void ParseRawData(const char* ptr);

	static const unsigned int PAGE_SIZE = 8;							//Page size in records
	static constexpr auto BYTE_SIZE = PAGE_SIZE * Record::RECORD_SIZE;	//Page size in bytes
	Record data[PAGE_SIZE];
};

std::ostream& operator<<(std::ostream& os, const Page& page);