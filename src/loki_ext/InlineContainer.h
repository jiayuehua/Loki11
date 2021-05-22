#ifndef __INLINE_CONTAINER_H__
#define __INLINE_CONTAINER_H__
     
#include <vector>
#include <list>
#include <deque>
     
template < class T, class container = std::vector<T> > 
class inline_container : public container
{
public:
    inline_container()      
    {
    }
    inline_container(inline_container &v) 
    {
        this->swap(v);
    }
    explicit inline_container(const T &a) 
        : container(1, a)
    {           
    }
    inline_container &operator()(const T &a) 
    {
        this->push_back(a);
        return *this;
    }
};
     
template <class T>
inline inline_container<T> make_vector(const T &a) 
{
    return inline_container<T>(a);
}
     
template <class T>
inline inline_container< T, std::list<T> > make_list(const T &a) 
{
    return inline_container< T, std::list<T> >(a);
}
     
template <class T>
inline inline_container< T, std::deque<T> > make_deque(const T &a)
{
    return inline_container< T, std::deque<T> >(a);
}
     
template <class container> 
inline const container::value_type min(const container &a) 
{
    return *std::min_element(a.begin(), a.end());
}
     
template <class container> 
inline const container::value_type max(const container &a) 
{
    return *std::max_element(a.begin(), a.end());
}
     
#endif //__INLINE_CONTAINER_H__
     
/* End of File */
     
