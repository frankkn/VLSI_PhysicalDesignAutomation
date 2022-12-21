void Legalizer::addCell(Cluster* cluster, Cell* cell, double optimalX)
{
  cluster->member.emplace_back(cell);
  cluster->weight += cell->weight;
  cluster->q += cell->weight * (optimalX - cluster->width);
  cluster->width += cell->width; 
}

void Legalizer::addCluster(Cluster* prevCluster, Cluster* cluster)
{
  prevCluster->member.insert(prevCluster->member.end(), cluster->member.begin(), cluster->member.end());
  prevCluster->weight += cluster->weight;
  prevCluster->q += cluster->q - cluster->weight * prevCluster->width; 
  prevCluster->width += cluster->width;
	delete cluster;
	cluster = prevCluster;
}

void Legalizer::collapse(Cluster* cluster, SubRow* subRow)
{
  cluster->x = cluster->q / cluster->weight;

  if(cluster->x < subRow->minX)
  {
    cluster->x = subRow->minX;
  }
  if(cluster->x > subRow->maxX - cluster->width)
  {
    cluster->x = subRow->maxX - cluster->width;
  }

  auto prevCluster = cluster->predecessor;
  if(prevCluster != nullptr && prevCluster->x + prevCluster->width > cluster->x)
  {
    addCluster(prevCluster, cluster);
    collapse(prevCluster, subRow);
  }
}

void Legalizer::placeRowFinal(int const &rowIdx, int const &subRowIdx, Cell *cell)
{
	auto subRow = input->rows[rowIdx]->subRows[subRowIdx];
	subRow->freeWidth -= cell->width;

	double optimalX = cell->x;
	if (cell->x < subRow->minX)
		optimalX = subRow->minX;
	else if (cell->x > subRow->maxX - cell->width)
		optimalX = subRow->maxX - cell->width;

	auto cluster = subRow->lastCluster;
	if (cluster == nullptr || cluster->x + cluster->width <= optimalX)
	{
		cluster = new Cluster(optimalX,
													subRow->lastCluster,
													0,
													0.0,
													0);
		addCell(cluster, cell, optimalX);
		subRow->lastCluster = cluster;
	}
	else
	{
		addCell(cluster, cell, optimalX);
		collapse(cluster, subRow);
		subRow->lastCluster = cluster;
	}
}