/*	Copyright (C) 2004 Garrett A. Kajmowicz
This file is part of the uClibc++ Library.
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef YASLI_SORT_HPP
#define YASLI_SORT_HPP


#include "cstdlib"
#include "iterator"
#include "utility"
#include "functional"
#include <assert.h>


//Elliminate any previously defined macro

namespace yasli{
	namespace SortPrivate{
		using std::pair;
		template <class Iter, unsigned int size,class Compare> struct SelectionSorter;

		template <class Iter,class Compare> struct SelectionSorter<Iter, 1, Compare>
		{
			static Iter MinElement(Iter b,Compare)
			{
				return b;
			}

			static pair<Iter, Iter> MinMaxElement(Iter b,Compare)
			{
				return make_pair(b, b);
			}

			static void Run(Iter,Compare)
			{
				// Nothing to do
			}

			static void Run(Iter, size_t,Compare)
			{
				assert(false);
			}
		};

		template <class Iter,class Compare> struct SelectionSorter<Iter, 0, Compare>
		{
			static void Run(Iter,Compare)
			{
				// Nothing to do
			}

			static void Run(Iter, size_t,Compare)
			{
				assert(false);
			}
		};

		template <class Iter, unsigned int size,class Compare> struct SelectionSorter
		{
			static Iter MinElement(Iter b,Compare comp)
			{
				const Iter candidate = 
					SelectionSorter<Iter, size - 1,Compare>::MinElement(b + 1,comp);
				return comp(*candidate,*b) ? candidate : b;
			}

			static pair<Iter, Iter> MinMaxElement(Iter b,Compare comp)
			{
				pair<Iter, Iter> candidate = 
					SelectionSorter<Iter, size - 1,Compare>::MinMaxElement(b + 1, comp);
				if (!(comp(*candidate.first, *b))) candidate.first = b;
				else if (comp(*candidate.second , *b)) candidate.second = b;
				return candidate;
			}

			static void Run(Iter b,Compare comp)
			{
				typedef SelectionSorter<Iter, size - 1, Compare> Reduced;
				Iter afterB = b; ++afterB;
				const Iter candidate = Reduced::MinElement(afterB,comp);
				if (comp(*candidate,*b)) std::iter_swap(b, candidate);
				Reduced::Run(afterB,comp);
				/*
				// Alternate algorithm (proved to be slower in practice)
				const pair<Iter, Iter> i = MinMaxElement(b);
				if (b != i.first) std::iter_swap(b, i.first);
				std::iter_swap(b + (size - 1), 
				b == i.second ? i.first : i.second);
				SelectionSorter<Iter, size - 2>::Run(b + 1);
				*/
			}

			static void Run(Iter b, size_t length,Compare comp)
			{
				assert(length <= size);
				if (length == size) Run(b,comp);
				else SelectionSorter<Iter, size - 1,Compare>::Run(b, length,comp);
			}
		};

		template <class Iter,class Compare> std::pair<Iter, Iter> Partition(Iter b, Iter e, Iter pivot, Compare comp)
		{
			using std::iter_swap;
			using std::make_pair;

			assert(e - b > 1);
			assert(b <= pivot && pivot < e);

			for (;;)
			{
				const Iter bOld = b;

				// Find the first element strictly greater than the pivot, from left
				while (!(comp(*pivot , *b))) 
				{
					if (++b != e) continue; 

					// *pivot is the largest element
					--b;
					// move the pivot to the end of the array
					iter_swap(pivot, b);
					return make_pair(b, e);
				}

				assert(comp(*pivot , *b)); // found one element greater than the pivot
				assert(pivot != b); // this fires => badly implemented operator<

				// Find the first element strictly smaller than the pivot, from right
				for (--e; !(comp(*e , *pivot)); --e)
				{
					if (bOld != e) continue;

					// *pivot is the smallest element
					// return the range between the beginning and the first
					// element strictly larger than the pivot
					iter_swap(pivot, bOld);
					if (b == bOld) ++b;
					return make_pair(bOld, b);
				}

				assert(comp(*e , *pivot)); // found one element greater than the pivot
				assert(pivot != e); // this fires => badly implemented operator<
				assert(b != e); // this fires => badly implemented operator<

				// If they crossed, end of story
				if (e < b)
				{
					if (b == ++e)
					{
						// hoist the pivot in between b and e
						if (pivot > b) 
						{
							iter_swap(pivot, b);
							return make_pair(b, b + 1);
						}
						--e;
						assert(pivot < e);
						iter_swap(e, pivot);
					}
					return make_pair(e, b);
				}

				// Swap the elements and go on
				iter_swap(b, e);
				assert(pivot != b);
				assert(pivot != e);

				// Always keep the pivot in between b and e
				if (pivot > e)
				{
					iter_swap(pivot, e);
					pivot = e;
					++e;
					++b;
				}
				else if (pivot < b)
				{
					iter_swap(pivot, b);
					pivot = b;
				}
				else
				{
					++b;
				}

				if (b == e) 
				{
					assert(comp(*pivot , *b));
					if (pivot < b) 
					{
						iter_swap(pivot, --b);
						return make_pair(b, e);
					}
					assert(pivot > e);
					iter_swap(pivot, b);
					return make_pair(b, b + 1);
				}
			}
		}

		static inline unsigned short rand2_private(void)
		{
			static unsigned int a = 18000;   
			static unsigned int g_PrivateRandom2AlgoSeed = 521288629;
			return (unsigned short)(g_PrivateRandom2AlgoSeed = 
				a * (g_PrivateRandom2AlgoSeed & 65535) + 
				(g_PrivateRandom2AlgoSeed >> 16));
		}

		template <class Iter,class Compare> Iter SelectPivot(Iter b, Iter e,Compare comp)
		{
			using std::iter_swap;
			assert(b < e);

			typedef typename std::iterator_traits<Iter>::difference_type difference_type; 
			const size_t size = size_t(e - b);

			assert(size >= 3);
			const size_t innerSize = size - 2;
			const size_t offset = (rand2_private() * innerSize) >> 16u;
			assert(offset < innerSize);
			const Iter m = b + offset + 1;
			assert(b < m && m < e);

			// Sort in-place b, m, and e
			--e;
			if (comp(*m , *b))
			{
				if (comp(*e , *m))
				{
					// *e < *m < *b
					iter_swap(b, e);
				}
				else
				{
					if (comp(*e , *b))
					{
						// *m <= *e < *b
						iter_swap(b, m);
						iter_swap(m, e);
					}
					else
					{
						// *m < *b <= *e
						iter_swap(b, m);
					}
				}
			}
			else
			{
				if (comp(*e , *b))
				{
					// *e < *b <= *m
					iter_swap(b, e);
					iter_swap(m, e);
				}
				else
				{
					if (comp(*e , *m))
					{
						// *b <= *e < *m
						iter_swap(m, e);
					}
					else
					{
						// *b <= *m <= *e
						// nothing to do
					}
				}
			}

			assert(!(comp(*m , *b)));
			assert(!(comp(*e , *m)));
			return m;
		}
		template <class Iter, class Compare> void unstable_sort_private(Iter b, Iter e,Compare comp)
		{
			assert(e >= b);
			// The second branch of Quicksort's recursion is implemented through 
			//      iteration, hence the following loop
			size_t length; 
			while ((length = static_cast<size_t>(e - b)) > 1)
			{
				enum { threshold = 8 };
				if (length <= threshold)
				{
					 //InsertionSorter<Iter, threshold, 1>::Run(b, length);
					SortPrivate::SelectionSorter<Iter, threshold,Compare>::Run(b, length, comp);
				    return;
				}
				const pair<Iter, Iter> midRange = SortPrivate::Partition(b, e, SortPrivate::SelectPivot(b, e, comp), comp);
				assert(midRange.second - midRange.first > 0);
				assert(b <= midRange.first);
				assert(midRange.second <= e);
				if (midRange.first - b < e - midRange.second)
				{
					unstable_sort_private(b, midRange.first,comp);
					b = midRange.second;
				}
				else
				{
				    unstable_sort_private(midRange.second, e,comp);
					e = midRange.first;
				}
			}
		}
	}


	template<class RandomAccessIterator, class Compare> 
	void sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		SortPrivate::unstable_sort_private(first, last, comp);
	}
	template<class RandomAccessIterator> 
	void sort(RandomAccessIterator first, RandomAccessIterator last)
	{
		std::less<typename std::iterator_traits<RandomAccessIterator>::value_type> c;
		yasli::sort(first, last, c);
	}

template<class InputIterator, class RandomAccessIterator, class Compare> 
		RandomAccessIterator
		partial_sort_copy(InputIterator first, InputIterator last,
		RandomAccessIterator result_first, RandomAccessIterator result_last, Compare comp)
	{
		size_t output_size = last-first;
		size_t temp_size = result_last - result_first;
		if(temp_size < output_size){
			output_size = temp_size;
		}

		RandomAccessIterator middle = result_first + output_size;
		RandomAccessIterator temp = result_first;

		while(temp != middle){
			*temp = *first;
			++temp;
			++first;
		}
		yasli::sort(result_first, middle, comp);

		while( first != last){
			if( comp( *first, *(middle-1) ) ){
				*(middle-1) = *first;
				yasli::sort(result_first, middle, comp);
			}
			++first;
		}

		return middle;
	}
	template<class InputIterator, class RandomAccessIterator> 
	RandomAccessIterator
		partial_sort_copy(InputIterator first, InputIterator last,
		RandomAccessIterator result_first, RandomAccessIterator result_last)
	{
		less<typename iterator_traits<RandomAccessIterator>::value_type> c;
		return yasli::partial_sort_copy(first, last, result_first, result_last, c);
	}
	
		}


#endif 