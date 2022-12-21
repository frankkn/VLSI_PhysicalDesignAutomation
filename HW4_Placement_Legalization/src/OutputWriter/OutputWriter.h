#pragma once
#include "../Module/Module.h"

class OutputWriter
{
	Input *input;

public:
	OutputWriter(Input *input):input(input) {}
	void WriteResult(char argv[]);
};
