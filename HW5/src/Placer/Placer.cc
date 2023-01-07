#include <cmath>
#include "Placer.h"
using namespace std;

void Placer::createDieBoundary()
{
	input->die->design_name = 'CS_APR';
	input->die->x1 = input->die->y1 = 0;
	int tmp = sqrt(input->numCS);
	input->die->x2 = input->GP->CS_WIDTH * (tmp*2) + input->GP->M3_SPACING * ((tmp+1)*(tmp*2-1)+tmp) + input->GP->M3_WIDTH * (tmp*(tmp*2));
	input->die->y2 = input->GP->CS_HEIGHT * (tmp*2) + input->GP->M4_SPACING * ((tmp/2+1)*(tmp*2-1)+tmp/2) + input->GP->M4_WIDTH * ((tmp/2)*(tmp*2)); 
}

void Placer::createCSPlacement()
{
	int n = input->numCS;
	auto GP = input->GP;
	int Dx = GP->CS_WIDTH + GP->M3_SPACING*3 + GP->M3_WIDTH*2;
	int Dy = GP->CS_HEIGHT + GP->M4_SPACING*2 + GP->M4_WIDTH;
	for(int i = 0; i < n; ++i)
	{
		for(int j = 0; j < n; ++j)
		{
			auto cur_cs = input->cs_array[i][j];
			cur_cs->lib_name = input->GP->CS_LIB_NAME;
			cur_cs->inst_name = "Transistor" + to_string(i*4+j);
			cur_cs->x = i * Dx;
			cur_cs->y = j * Dy + (GP->M4_SPACING+GP->M4_WIDTH);
		}
	}
}

OutputWriter *Placer::run()
{
	createDieBoundary();
	createCSPlacement();
	return new OutputWriter(input);
}