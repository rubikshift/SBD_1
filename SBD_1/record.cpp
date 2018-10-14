#include <iomanip>
#include "record.h"

Record::Record(double v, double m) : v(v), m(m)
{
}

bool Record::isInitialized() const
{
	return v != UNINIT && m != UNINIT;
}

double Record::Energy() const
{
	if (this->isInitialized())
		return m * v * v / 2;
	else
		return UNINIT;
}

std::ostream & operator<<(std::ostream & os, const Record & record)
{
	os << "Ek: " << std::setw(10) << std::left << std::setprecision(5) << record.Energy();
	os << "v: " << std::setw(10) << std::left << std::setprecision(5) << record.v;
	os << "m: " << std::setw(10) << std::left << std::setprecision(5) << record.m;
	return os;
}
