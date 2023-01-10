#include <cmath>
#include <iostream>
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
	int tmp = sqrt(input->numCS);
	auto GP = input->GP;
	int Dx = GP->CS_WIDTH + GP->M3_SPACING*(tmp+1) + GP->M3_WIDTH*(tmp);
	int Dy = GP->CS_HEIGHT + GP->M4_SPACING*(tmp/2+1) + GP->M4_WIDTH*(tmp/2);
	for(int i = 0; i < tmp*2; ++i)
	{
		for(int j = 0; j < tmp*2; ++j)
		{
			// The following 5 commented lines are INCORRECT! 
			// Only nullptr in cs_array's constructor
			// auto cur_cs = input->cs_array[i][j];
			// cur_cs->lib_name = input->GP->CS_LIB_NAME;
			// cur_cs->inst_name = "Transistor" + to_string(i*4+j);
			// cur_cs->x = i * Dx;
			// cur_cs->y = j * Dy + (GP->M4_SPACING+GP->M4_WIDTH);

			string lib_name = input->GP->CS_LIB_NAME;
			string inst_name = "Transistor" + to_string(i*4+j);
			int x = i * Dx;
			int y = j * Dy + (GP->M4_SPACING+GP->M4_WIDTH);
			input->cs_array[i][j] = new Component(lib_name, inst_name, x, y);
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
			string inst_name = "Matal3_" + to_string(i*2+j);
			string layer = "ME3";
			int x1 = input->cs_array[i][0]->x + Dx2 + j * (GP->M3_WIDTH + GP->M3_SPACING);
			int x2 = x1 + GP->M3_WIDTH;
			int y1 = 0;
			int y2 = input->die->y2;
			input->ME3_specialnet[i][j] = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
		}
	}
}

void Placer::createME4Drain()
{
	int tmp = sqrt(input->numCS);
	auto GP = input->GP;
	string layer = "ME4";
	for(int i = 0; i < tmp; ++i)
	{
		for(int j = 0; j < tmp; ++j)
		{
			// left bottom corner units
			string inst_name = "Metal4_drain" + (i * tmp + j + 0 * input->numCS);
			int x1 = input->cs_array[i][j]->x + GP->CS_X1_TO_DRAIN;
			int x2 = input->ME3_specialnet[i][j]->x2;
			int y1 = input->cs_array[i][j]->y + GP->CS_Y1_TO_DRAIN;
			int y2 = y1 + GP->M4_WIDTH;
			input->ME4_specialnet_drain[i][j] = new SpecialNet(inst_name, layer, x1, x2, y1, y2);
			// right bottom corner units
			inst_name = "Metal4_drain" + (i * tmp + j + 1 * input->numCS);
			x1 = input->cs_array[(tmp*2-1)-i][j]->x + GP->CS_X1_TO_DRAIN;
			x2 = input->ME3_specialnet[(tmp*2-1)-i][j]->x2;
			y1 = input->cs_array[(tmp*2-1)-i][j]->y + GP->CS_Y1_TO_DRAIN;
			y2 = y1 + GP->M4_WIDTH;
			input->ME4_specialnet_drain[(tmp*2-1)-i][j] = new SpecialNet(inst_name, layer, x1, x2, y1, y2);
			// left top corner units
			inst_name = "Metal4_drain" + (i * tmp + j + 2 * input->numCS);
			x1 = input->cs_array[i][(tmp*2-1)-j]->x + GP->CS_X1_TO_DRAIN;
			x2 = input->ME3_specialnet[i][j]->x2;
			y1 = input->cs_array[i][(tmp*2-1)-j]->y + GP->CS_Y1_TO_DRAIN;
			y2 = y1 + GP->M4_WIDTH;
			input->ME4_specialnet_drain[i][(tmp*2-1)-j] = new SpecialNet(inst_name, layer, x1, x2, y1, y2);
			// right top corner units
			inst_name = "Metal4_drain" + (i * tmp + j + 3 * input->numCS);
			x1 = input->cs_array[(tmp*2-1)-i][(tmp*2-1)-j]->x + GP->CS_X1_TO_DRAIN;
			x2 = input->ME3_specialnet[(tmp*2-1)-i][j]->x2;
			y1 = input->cs_array[(tmp*2-1)-i][(tmp*2-1)-j]->y + GP->CS_Y1_TO_DRAIN;
			y2 = y1 + GP->M4_WIDTH;
			input->ME4_specialnet_drain[(tmp*2-1)-i][(tmp*2-1)-j] = new SpecialNet(inst_name, layer, x1, x2, y1, y2);
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
			string inst_name = "Metal4_port_" + to_string(i*2+j);
			string layer = "ME4";
			int x1 = 0;
			int x2 = input->die->x2;
			int y1 = i * Dy + j * (GP->M4_WIDTH + GP->M4_SPACING);
			int y2 = y1 + GP->M4_WIDTH;
			input->ME4_specialnet_port[i][j] = new SpecialNet(inst_name, layer, x1, x2, y1, y2);
		}
	}
}

void Placer::createVia34_drain2ME3()
{
	int tmp = sqrt(input->numCS);
	string lib_name = "VIA34_LIB_NAME";
	for(int i = 0; i < tmp; ++i)
	{
		for(int j = 0; j < tmp; ++j)
		{
			// left bottom corner units
			string inst_name = "Via34_drain2ME3_" + to_string(i * tmp + j + 0 * input->numCS);
			int x = input->ME3_specialnet[i][j]->x1;
			int y = input->cs_array[i][j]->y + input->GP->CS_Y1_TO_DRAIN;
			input->Via34_drain2ME3[i][j] = new Component(lib_name, inst_name, x, y);
			// right bottom corner units
			inst_name = "Via34_drain2ME3_" + to_string(i * tmp + j + 1 * input->numCS);
			x = input->ME3_specialnet[(tmp*2-1)-i][j]->x1;
			y = input->cs_array[(tmp*2-1)-i][j]->y + input->GP->CS_Y1_TO_DRAIN;
			input->Via34_drain2ME3[(tmp*2-1)-i][j] = new Component(lib_name, inst_name, x, y);
			// left top corner units
			inst_name = "Via34_drain2ME3_" + to_string(i * tmp + j + 2 * input->numCS);
			x = input->ME3_specialnet[i][j]->x1;
			y = input->cs_array[i][(tmp*2-1)-j]->y + input->GP->CS_Y1_TO_DRAIN;
			input->Via34_drain2ME3[i][(tmp*2-1)-j] = new Component(lib_name, inst_name, x, y);
			// right top corner units
			inst_name = "Via34_drain2ME3_" + to_string(i * tmp + j + 3 * input->numCS);
			x = input->ME3_specialnet[(tmp*2-1)-i][j]->x1;
			y = input->cs_array[(tmp*2-1)-i][(tmp*2-1)-j]->y + input->GP->CS_Y1_TO_DRAIN;
			input->Via34_drain2ME3[(tmp*2-1)-i][(tmp*2-1)-j] = new Component(lib_name, inst_name, x, y);
		}
	}
}

void Placer::createVia34_port2ME3()
{
	int tmp = sqrt(input->numCS);
	string lib_name = "VIA34_LIB_NAME";
	for(int i = 0; i < tmp*2; ++i)
	{
		for(int j = 0; j < tmp/2; ++j)
		{
			// left one 
			// string inst_name = "Via34_port2ME3" + to_string(i*tmp + j*2);
			// int x = input->Via34_drain2ME3[i/2][i%2*tmp/2+j]->x; 
			// int y = input->ME4_specialnet_port[i][j]->y1;
			// input->Via34_port2ME3[i*tmp/2+j][0] = new Component(lib_name, inst_name, x, y);
			// right one
			// inst_name = "Via34_port2ME3" + to_string(i*tmp + j*2 + 1);
			// x = input->Via34_drain2ME3[tmp*2-i][i%2*tmp/2+j]->x;
			// y = input->ME4_specialnet_port[i][j]->y1;
			// input->Via34_port2ME3[i*tmp/2+j][1]  = new Component(lib_name, inst_name, x, y);
		}
	}
}


OutputWriter *Placer::run()
{
	createDieBoundary();
	createCSPlacement();
	createVerticalME3();
	// createME4Drain();
	// createME4Port();
  // createVia34_drain2ME3();
  // createVia34_port2ME3();
	return new OutputWriter(input);
}