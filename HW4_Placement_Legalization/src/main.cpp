#include "Abacus/Abacus.h"
#include "ArgParser/ArgParser.h"
#include "Module/Module.h"

int main(int argc, char **argv)
{
	Parser parser;
	auto input = parser.parse(argv[1]);

	AbacusLegalizer AL(input);
	
	OutputWriter* OW = AL.run();
	OW->WriteResult(argv[2]);

	return 0;
}
