#include "Abacus/Abacus.h"
#include "ArgParser/ArgParser.h"
#include "Clock/Clock.h"
#include "Module/Module.h"

int main(int argc, char **argv)
{
	// Clock clock(10*60 - 3);
	// clock.StartClock("Total time");

	// clock.StartClock("Input time");
	Parser parser;
	auto input = parser.parse(argv[1]);
	// clock.EndClock("Input time");
  
  // clock.StartClock("Abacus time");
	AbacusLegalizer AL(input);
	OutputWriter* OW = AL.run();
	// clock.EndClock("Abacus time");
	
	// clock.StartClock("Output time");
	OW->WriteResult(argv);
	// clock.EndClock("Output time");

	// clock.EndClock("Total time");

	// clock.PrintDuration("Input time");
	// clock.PrintDuration("Abacus time");
	// clock.PrintDuration("Output time");
	// clock.PrintDuration("Total time");
	return 0;
}
