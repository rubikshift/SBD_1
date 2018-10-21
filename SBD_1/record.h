#pragma once
#include <iostream>

struct Record
{
	Record(double v = UNINIT, double m = UNINIT); // v == UNINIT or m == UNINIT => uninitialized record
	bool isInitialized() const;
	double Energy() const;

	bool operator==(const Record& record) const;
	bool operator>(const Record& record) const;
	bool operator<(const Record& record) const;
	bool operator>=(const Record& record) const;
	bool operator<=(const Record& record) const;

	double v, m;

	static constexpr double UNINIT = -1;
	static const unsigned int RECORD_SIZE = sizeof(v) + sizeof(m);

};

std::ostream& operator<<(std::ostream& os, const Record& record);