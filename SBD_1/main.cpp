#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include "record.h"
#include "page.h"
#include "file.h"

unsigned int SERIES = 0;

unsigned int fib(unsigned int sum);
void log(std::string text);

int main(int argc, char* argv[])
{
	int n;
	Record recordBCK, record;  //tmp value, used in calculating series
	std::stringstream stream;
	std::string fileName;
	std::ifstream inputFile;

	if (argc != 3)
	{
		if (argc < 3)
			std::cout << "To few arguments" << std::endl;
		else
			std::cout << "To many arguments" << std::endl;
		std::cout << "Help:" << std::endl;
		std::cout << "\tsbd_1.exe f file" << std::endl;
		std::cout << "\t\t reads records from file" << std::endl;
		std::cout << "\tsbd_1.exe r n" << std::endl;
		std::cout << "\t\t randomly generates n records" << std::endl;
		std::cout << "\tsbd_1.exe m n" << std::endl;
		std::cout << "\t\t user has to manualy type n records" << std::endl;
		return 1;
	}
	
	log("START PREPARING");
	stream << argv[2];

	switch (argv[1][0])
	{
		case 'f': 
			stream >> fileName;
			inputFile.open(fileName);
			while (!inputFile.eof())
			{
				inputFile >> record.v >> record.m;
				if (recordBCK > record)
					SERIES++;
				recordBCK = record;
			}
			break;
		case 'r':
			stream >> n;
			break;
		case 'm': 
			stream >> n;
			break;
	}
	log("END PREPARING");

	return 0;
}

unsigned int fib(unsigned int sum)
{
	unsigned int a = 0, b = 1, tmp;

	while (sum > a + b)
	{
		tmp = b;
		b = a + b;
		a = tmp;
	}

	return a; //returns number of series on smaller tape
}

void log(std::string stage)
{
	std::cout << std::setw(80) << std::setfill('-') << std::left << "---" + stage << std::endl;
}
