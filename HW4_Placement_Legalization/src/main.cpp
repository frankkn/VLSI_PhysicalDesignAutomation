#include <iostream>
#include "Abacus/Abacus.h"
#include "ArgParser/ArgParser.h"
#include "Module/Module.h"
// #include "GlobalTimer/GlobalTimer.h"

int main(int argc, char **argv)
{
	Parser parser;
	auto input = parser.parse(argv[1]);

	Legalizer legalizer(input);
	auto result = legalizer.solve();

	result->write(argv);

	return 0;
}
