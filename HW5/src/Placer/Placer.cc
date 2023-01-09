#include <cmath>
#include "Placer.h"
using namespace std;

void Placer::createDieBoundary()
{
	input->die->design_name = "CS_APR";
	input->die->x1 = input->die->y1 = 0;
	int tmp = sqrt(input->numCS);
	input->die->x2 = input->GP->CS_WIDTH * (tmp*2) + input->GP->M3_SPACING * ((tmp+1)*(tmp*2-1)+tmp) + input->GP->M3_WIDTH * (tmp*(tmp*2));
	input->die->y2 = input->GP->CS_HEIGHT * (tmp*2) + input->GP->M4_SPACING * ((tmp/2+1)*(tmp*2-1)+tmp/2) + input->GP->M4_WIDTH * ((tmp/2)*(tmp*2)); 
}

void Placer::createCSPlacement()
{
	int n = input->numCS;
	int tmp = sqrt(n);
	auto GP = input->GP;
	int Dx = GP->CS_WIDTH + GP->M3_SPACING*(tmp+1) + GP->M3_WIDTH*(tmp);
	int Dy = GP->CS_HEIGHT + GP->M4_SPACING*(tmp/2+1) + GP->M4_WIDTH*(tmp/2);
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

void Placer::createVerticalME3()
{
	int tmp = sqrt(input->numCS);
	auto GP = input->GP;
	int Dx2 = GP->CS_WIDTH + GP->M3_SPACING;
	for(int i = 0; i < tmp*2; ++i)
	{
		for(int j = 0; j < tmp; ++j)
		{
			auto cur_ME3 = input->ME3_specialnet[i][j];
			cur_ME3->inst_name = "Matal3_" + to_string(i*2+j);
			cur_ME3->layer = "ME3";
			cur_ME3->x1 = input->cs_array[i][0]->x + Dx2 + j * (GP->M3_WIDTH + GP->M3_SPACING);
			cur_ME3->x2 = cur_ME3->x1 + GP->M3_WIDTH;
			cur_ME3->y1 = 0;
			cur_ME3->y2 = input->die->y2;
		}
	}
}

void Placer::createME4Drain()
{
	int tmp = sqrt(input->numCS);
	auto GP = input->GP;
	for(int i = 0; i < tmp; ++i)
	{
		for(int j = 0; j < tmp; ++j)
		{
			// left bottom corner units
			auto cur_ME4Drain = input->ME4_specialnet_drain[i][j];
			cur_ME4Drain->layer = "ME4";
			cur_ME4Drain->inst_name = "Metal4_drain" + (i * tmp + j + 0 * input->numCS);
			cur_ME4Drain->x1 = input->cs_array[i][j]->x + GP->CS_X1_TO_DRAIN;
			cur_ME4Drain->x2 = input->ME3_specialnet[i][j]->x2;
			cur_ME4Drain->y1 = input->cs_array[i][j]->y + GP->CS_Y1_TO_DRAIN;
			cur_ME4Drain->y2 = cur_ME4Drain->y1 + GP->M4_WIDTH;
			// right bottom corner units
			cur_ME4Drain = input->ME4_specialnet_drain[(tmp*2-1)-i][j];
			cur_ME4Drain->layer = "ME4";
			cur_ME4Drain->inst_name = "Metal4_drain" + (i * tmp + j + 1 * input->numCS);
			cur_ME4Drain->x1 = input->cs_array[(tmp*2-1)-i][j]->x + GP->CS_X1_TO_DRAIN;
			cur_ME4Drain->x2 = input->ME3_specialnet[(tmp*2-1)-i][j]->x2;
			cur_ME4Drain->y1 = input->cs_array[(tmp*2-1)-i][j]->y + GP->CS_Y1_TO_DRAIN;
			cur_ME4Drain->y2 = cur_ME4Drain->y1 + GP->M4_WIDTH;
			// left top corner units
			cur_ME4Drain = input->ME4_specialnet_drain[i][(tmp*2-1)-j];
			cur_ME4Drain->layer = "ME4";
			cur_ME4Drain->inst_name = "Metal4_drain" + (i * tmp + j + 2 * input->numCS);
			cur_ME4Drain->x1 = input->cs_array[i][(tmp*2-1)-j]->x + GP->CS_X1_TO_DRAIN;
			cur_ME4Drain->x2 = input->ME3_specialnet[i][(tmp*2-1)-j]->x2;
			cur_ME4Drain->y1 = input->cs_array[i][(tmp*2-1)-j]->y + GP->CS_Y1_TO_DRAIN;
			cur_ME4Drain->y2 = cur_ME4Drain->y1 + GP->M4_WIDTH;
			// right top corner units
			cur_ME4Drain = input->ME4_specialnet_drain[(tmp*2-1)-i][(tmp*2-1)-j];
			cur_ME4Drain->layer = "ME4";
			cur_ME4Drain->inst_name = "Metal4_drain" + (i * tmp + j + 3 * input->numCS);
			cur_ME4Drain->x1 = input->cs_array[(tmp*2-1)-i][(tmp*2-1)-j]->x + GP->CS_X1_TO_DRAIN;
			cur_ME4Drain->x2 = input->ME3_specialnet[(tmp*2-1)-i][j]->x2;
			cur_ME4Drain->y1 = input->cs_array[(tmp*2-1)-i][(tmp*2-1)-j]->y + GP->CS_Y1_TO_DRAIN;
			cur_ME4Drain->y2 = cur_ME4Drain->y1 + GP->M4_WIDTH;
		}
	}
}

void Placer::createME4Port()
{
	int tmp = sqrt(input->numCS);
	auto GP = input->GP;
	int Dy = GP->CS_HEIGHT + GP->M4_SPACING*(tmp/2+1) + GP->M4_WIDTH*(tmp/2);
	for(int i = 0; i < (tmp*2); ++i)
	{
		for(int j = 0; j < tmp/2; ++j)
		{
			auto cur_ME4Port = input->ME4_specialnet_port[i][j];
			cur_ME4Port->inst_name = "Metal4_port_" + to_string(i*2+j);
			cur_ME4Port->layer = "ME4";
			cur_ME4Port->x1 = 0;
			cur_ME4Port->x2 = input->die->x2;
			cur_ME4Port->y1 = i * Dy + j * (GP->M4_WIDTH + GP->M4_SPACING);
			cur_ME4Port->y2 = cur_ME4Port->y1 + GP->M4_WIDTH;
		}
	}
}

OutputWriter *Placer::run()
{
	createDieBoundary();
	createCSPlacement();
	createVerticalME3();
	createME4Drain();
	createME4Port();
  // createVia34_drain2ME3();
  // createVia34_port2ME3();
	return new OutputWriter(input);
}