#include <algorithm>
#include <climits>
#include "module.h"

void HardBlock::updateInfo(int& new_width, int& new_height, int& new_x, int& new_y)
{
  downleft_x = new_x;
  downleft_y = new_y;
  if(width == new_width && height == new_height)
  {
    rotated = false;
  }  
  else
  {
    rotated = true;
  }
  center_pin->x_cor = new_x + new_width/2;
  center_pin->y_cor = new_y + new_height/2;
}

int net::calHPWL()
{
  int min_x = INT_MAX, max_x = INT_MIN;
  int min_y = INT_MAX, max_y = INT_MIN;
  for(const auto& pin: pins)
  {
    if(pin->x_cor < min_x)  min_x = pin->x_cor;
    if(pin->x_cor > max_x)  max_x = pin->x_cor;
    if(pin->y_cor < min_y)  min_y = pin->y_cor;
    if(pin->y_cor > max_y)  max_y = pin->y_cor;
  }
  for(const auto& hb: hardblocks)
  {
    if(hb->center_pin->x_cor < min_x) min_x = hb->center_pin->x_cor;
    if(hb->center_pin->x_cor > max_x) max_x = hb->center_pin->x_cor;
    if(hb->center_pin->y_cor < min_y) min_y = hb->center_pin->y_cor;
    if(hb->center_pin->y_cor > max_y) max_y = hb->center_pin->y_cor;
  }
  return (max_x - min_x) + (max_y - min_y);
}

void TreeNode::updateShape()
{
  decltype(shape)().swap(shape);
  if(type == -1)
  {
    sort(lchild->shape.begin(), lchild->shape.end(), [&](auto& a, auto& b) {return get<1>(a) > get<1>(b);});
    sort(rchild->shape.begin(), rchild->shape.end(), [&](auto& a, auto& b) {return get<1>(a) > get<1>(b);});
    int i = 0, j = 0;
    while(i < lchild->shape.size() && j < rchild->shape.size())
    {
      auto cur_shape = make_tuple(get<0>(lchild->shape[i])+get<0>(rchild->shape[j]), max(get<1>(lchild->shape[i]), get<1>(rchild->shape[j])),make_pair(i,j));
      shape.emplace_back(cur_shape);
      if(get<1>(lchild->shape[i]) > get<1>(rchild->shape[j]))
      {
        ++i;
      }
      else if(get<1>(lchild->shape[i]) < get<1>(rchild->shape[j]))
      {
        ++j;
      }
      else
      {
        ++i;++j;
      }
    }
  }
  else
  {
    sort(lchild->shape.begin(), lchild->shape.end(), [&](auto& a, auto& b) {return get<0>(a) > get<0>(b);});
    sort(rchild->shape.begin(), rchild->shape.end(), [&](auto& a, auto& b) {return get<0>(a) > get<0>(b);});
    int i = 0, j = 0;
    while(i < lchild->shape.size() && j < rchild->shape.size())
    {
      auto cur_shape = make_tuple(max(get<0>(lchild->shape[i]),get<0>(rchild->shape[j])), get<1>(lchild->shape[i])+get<1>(rchild->shape[j]), make_pair(i,j));
      shape.emplace_back(cur_shape);
      if(get<0>(lchild->shape[i]) > get<0>(rchild->shape[j]))
      {
        ++i;
      }
      else if(get<0>(lchild->shape[i]) < get<0>(rchild->shape[j]))
      {
        ++j;
      }
      else
      {
        ++i;++j;
      }
    }
  }
}