#pragma once
#include <iostream>

struct Record
{
	Record(double v = UNINIT, double m = UNINIT); // v == UNINIT or m == UNINIT => uninitialized record
	bool isInitialized() const;
	double Energy() const;

	static constexpr double UNINIT = -1;
	static const unsigned int RECORD_SIZE = 2 * sizeof(double);

	double v, m;
};

std::ostream& operator<<(std::ostream& os, const Record& record);