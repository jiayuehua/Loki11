#ifndef __COMPOUND_ITERATOR_H__
#define __COMPOUND_ITERATOR_H__
     
#include <utility>
     
template <class T, 
    class outer_iter = T**, 
    class offset_type = ptrdiff_t>
class compound_iterator : 
    public std::iterator<std::random_access_iterator_tag, T>
{
public:
    typedef T *pointer;
    typedef T &reference;
    compound_iterator()
    {
    }
    compound_iterator(const compound_iterator &a)
        : State(a.State)
    {
    }
    compound_iterator(outer_iter i, offset_type d)
        : State(i, d)
    {
    }
    compound_iterator &operator++()
    {
        ++State.first;
        return *this;
    }
    compound_iterator &operator--()
    {
        --State.first;
        return *this;
    }
    compound_iterator operator++(int)
    {
        compound_iterator old(*this);
        ++*this;
        return old;
    }
    compound_iterator operator--(int)
    {
        compound_iterator old(*this);
        --*this;
        return old;
    }
    pointer operator->()
    {
        return &State.first[0][State.second];
    }
    reference operator*()
    {
        return *operator->();
    }
    compound_iterator &operator+=(int n) 
    {
        State.first += n;
        return *this;
    }
    compound_iterator operator+(int n)
    {
        return compound_iterator(State.first + n, State.second);
    }
    compound_iterator &operator-=(int n) 
    {
        State.first -= n;
        return *this;
    }
    compound_iterator operator-(int n)
    {
        return compound_iterator(State.first - n, State.second);
    }
    reference operator[](int n)
    {
        return State.first[n][State.second];
    }
    bool operator==(compound_iterator &a) 
    {
        return State == a.State;
    }
    bool operator!=(compound_iterator &a) 
    {
        return State != a.State;
    }
    bool operator<(compound_iterator &a) 
    {
        return State < a.State;
    }
protected:
    std::pair<outer_iter, offset_type> State;
};
     
#endif //__COMPOUND_ITERATOR_H__

