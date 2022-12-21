#pragma once
#include "../Module/Module.h"

class ResultWriter
{
	Input *input;

public:
	ResultWriter(Input *input) : input(input) {}
	void write(char *argv[]);
};
