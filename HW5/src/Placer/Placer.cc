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

OutputWriter *Placer::run()
{
	createDieBoundary();
	return new OutputWriter(input);
}