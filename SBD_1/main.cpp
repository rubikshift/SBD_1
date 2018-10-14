#include<iostream>
#include "record.h"
#include "page.h"
#include "file.h"

int main(int argc, char* argv[])
{
	auto r = Page();
	r.data[0] = { 2, 3.5 };
	auto f = File("sbd.data");
	f.Write(r);
	auto x = f.Read();
	std::cout << r;
	std::cout << x;
	return 0;
}