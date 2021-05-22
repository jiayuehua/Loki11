#include "yasli_find.h"
#include "yasli_sort.h"
#include <loki/yasli/yasli_vector.h>
#include <iostream>
int main()
{
	int a [] = {1,2,4,5,6,8,11,22};
	std::cout<<*yasli::binary_find<sizeof(a)/sizeof(int)>(a,6);
	std::cout<<*yasli::binary_find<sizeof(a)/sizeof(int)>(a,7);
	std::cout<<*yasli::binary_find<sizeof(a)/sizeof(int)>(a,8);
	int a1[] = {5,6,7,8,9,0,1,2,3,4};
	yasli::sort(a1, a1+ sizeof(a1)/sizeof(int));
	yasli::vector<int> a2(100);
	int len = a2.size();
	for (int i = 0; i<len; ++i)
	{
		std::cout<<" "<<a2[i];
	}
	std::cout<<std::endl;
}