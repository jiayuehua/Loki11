////////////////////////////////////////////////////////////////////////////////
// flex_string
// Copyright (c) 2001 by Andrei Alexandrescu
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id: main.cpp 949 2009-01-26 02:04:40Z rich_sposato $


#ifdef _MSC_VER

#pragma warning (disable : 4786)    // Get rid of browser information too long names
#ifdef _DEBUG
#pragma warning (disable : 4786)
#endif

#endif

#include <stdio.h>
#include <list>

#ifndef __MWERKS__
//#define NO_ALLOCATOR_REBIND
//#define NO_ITERATOR_TRAITS
#endif

#include <loki/flex/flex_string.h>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <boost/swap.hpp>
using namespace std;  
typedef yasli::string String;
typedef yasli::flex_string<
    char,
    std::char_traits<char>,
    std::allocator<char>,
	yasli::AllocatorStringStorage<char, std::allocator<char> >
> my_string;


template <class Integral1, class Integral2>
Integral2 random(Integral1 low, Integral2 up)
{
    Integral2 low2(low);
    assert(up >= low2);
    if (low2 == up) return low;
    Integral2 x = Integral2(low2 + (rand() * (up - low2)) / RAND_MAX);
    assert(x >= low2 && x <= up);
    return x;
}

template <class String>
String RandomString(const String* /* model */, unsigned int maxSize)
{
    String result(random(0, maxSize), '\0');
    size_t i = 0;
    for (; i != result.size(); ++i)
    {
        result[i] = random('a', 'z');
    }
    return result;
}

template <class String, class Integral>
void Num2String(String& str, Integral )
{
    str.resize(10, '\0');
//    ultoa((unsigned long)n, &str[0], 10);
    sprintf(&str[0], "%ul", 10);
    resize(str, strlen(str.c_str()));
}

std::list<char> RandomList(unsigned int maxSize)
{
    std::list<char> lst(random(0u, maxSize));
    std::list<char>::iterator i = lst.begin(); 
    for (; i != lst.end(); ++i)
    {
        *i = random('a', 'z');
    }
    return lst;
}

int currentTest = 0;

template <class String>
String Test(String, unsigned int count, bool avoidAliasing)
{
	system("pause");
    typedef typename String::size_type size_type;
    const size_type maxString = 20;

    String test;
    while (count--)
    {
        test = RandomString(&test, maxString);

        static unsigned int functionSelector = 0;
        currentTest = functionSelector % 97; 
        ++functionSelector;
        std::cout << currentTest <<"\n";
        switch (currentTest)
        {
        case 0:
            // test default constructor 21.3.1
			std::cout<<string()<<std::endl;
            return String();
            break;
        case 1:
            // test copy constructor 21.3.1
            {
                const size_type pos = random(0, test.size());
                String s(test, pos, random(0, static_cast<typename String::size_type>(test.size() - pos)));
                //test = s;
			std::cout<<test<<" "<<s<<" "<<pos<<std::endl;
            }
            break;
        case 2:
            // test constructor 21.3.1
            { 
                const size_type
                    pos = random(0, test.size()),
                    n = random(0, test.size() - pos);
                String s(test.c_str() + pos, n);
                test = s;
			std::cout<<test<<" "<<s<<std::endl;
            }
            break;
        case 3:
            // test constructor 21.3.1
            { 
                const size_type pos = random(0, test.size());
                String s(test.c_str() + pos);
                test = s;
			std::cout<<test<<" "<<s<<std::endl;

            }
            break;
        case 4:
            // test assignment 21.3.1
            {
                String s(random(0, 1000), '\0');
                size_type i = 0;
                for (; i != s.size(); ++i)
                {
                    s[i] = random('a', 'z');
                }
                test = s;
			std::cout<<test<<" "<<s<<std::endl;
            }
            break;
        case 5:
            // test assignment 21.3.1
            {
                String s(random(0, 1000), '\0');
                size_type i = 0;
                for (; i != s.size(); ++i)
                {
                    s[i] = random('a', 'z');
                }
                test = s.c_str();
			std::cout<<test<<" "<<s<<std::endl;
            }
            break;
        case 6:
            // test aliased assignment 21.3.1
            {
                String s = test;
                const size_t pos = random(0, test.size());
                if (avoidAliasing)
                {
                    test = String(test.c_str() + pos);
                }
                else
                {
                    test = test.c_str() + pos;
                }
				std::cout<<pos<<" "<<test<<" "<<s<<std::endl;
            }
            break;
        case 7:
            // test assignment 21.3.1
            test = random('a', 'z');
			std::cout<<test<<std::endl;
            break;
        case 8:
            // exercise iterators 21.3.2
            test.begin();
            test.end();
            test.rbegin();
            test.rend();
            break;
        case 9:
            // exercise capacity 21.3.3
            test.size();
            length(test);
            test.max_size();
            test.capacity();
            break;
        case 10:
            // test resize
            resize(test, random(0, test.size()), random('a', 'z'));
            break;
        case 11:
            // test resize with 1 arg
            resize(test, random(0, test.size()));
            break;
        case 12:
            // test reserve
            test.reserve(random(0, 1000));
            break;
        case 13:
            // test clear
            clear(test); // skip because std::string doesn't support it
            break;
        case 14:
            // exercise empty
            {
                const char* kEmptyString = "empty";
                const char* kNonEmptyString = "not empty";
                if (empty(test)) test = "empty";
                else test = "not empty";
                // the above assignments don't work yet; use iterator assign
                if (empty(test)) test = String(kEmptyString, kEmptyString + strlen(kEmptyString));
                else test = String(kNonEmptyString, kNonEmptyString + strlen(kNonEmptyString));
            }
            break;
        case 15:
            // exercise element access 21.3.4
            if(!empty(test))
            {
                test[random(0, test.size() - 1)];
                at(test, random(0, test.size() - 1));
            }
            break;
        case 16:
            // 21.3.5 modifiers
            test += RandomString(&test, maxString);
            break;
        case 17:
            // aliasing modifiers
            test += test;
            break;
        case 18:
            // 21.3.5 modifiers
            test += RandomString(&test, maxString).c_str();
            break;
        case 19:
            // aliasing modifiers
            if (avoidAliasing)
            {
                test += String(test.c_str() + random(0, test.size()));
            }
            else
            {
                test += test.c_str() + random(0, test.size());
            }
            break;
        case 20:
            // 21.3.5 modifiers
            test += random('a', 'z');
            break;
        case 21:
            // 21.3.5 modifiers
            append(test, RandomString(&test, maxString));
            break;
        case 22:
            // 21.3.5 modifiers
            {
                String s(RandomString(&test, maxString));
                append(test, s, random(0, s.size()), random(0, maxString));
            }
            break;
        case 23:
            // 21.3.5 modifiers
            {
                String s = RandomString(&test, maxString);
                test.append( s.c_str(), random(0, s.size()));
            }
            break;
        case 24:
            // 21.3.5 modifiers
            append(test,RandomString(&test, maxString).c_str());
            break;
        case 25:
            // 21.3.5 modifiers
            append(test,random(0, maxString), random('a', 'z'));
            break;
        case 26:
            {
                std::list<char> lst(RandomList(maxString));
                append(test,lst.begin(), lst.end());
            }
            break;
        case 27:
            // 21.3.5 modifiers
            // skip push_back, Dinkumware doesn't support it
            push_back(test, random('a', 'z'));
            break;
        case 28:
            // 21.3.5 modifiers
            assign(test, RandomString(&test, maxString));
            break;
        case 29:
            // 21.3.5 modifiers
            {
                String str = RandomString(&test, maxString);
                assign(test, str, random(0, str.size()), random(0, maxString));
            }
            break;
        case 30:
            // 21.3.5 modifiers
            {
                String str = RandomString(&test, maxString);
                assign(test, str.c_str(), random(0, str.size()));
            }
            break;
        case 31:
            // 21.3.5 modifiers
            assign(test, RandomString(&test, maxString).c_str());
            break;
        case 32:
            // 21.3.5 modifiers
            assign(test, random(0, maxString), random('a', 'z'));
            break;
        case 33:
            // 21.3.5 modifiers
            {
                // skip, Dinkumware doesn't support it
                std::list<char> lst(RandomList(maxString));
                assign(test, lst.begin(), lst.end());
            }
            break;
        case 34:
            // 21.3.5 modifiers
            insert(test, random(0, test.size()), RandomString(&test, maxString));
            break;
        case 35:
            // 21.3.5 modifiers
            {
                String str = RandomString(&test, maxString);
                insert(test, random(0, test.size()), 
                    str, random(0, str.size()), 
                    random(0, maxString));
            }
            break;
        case 36:
            // 21.3.5 modifiers
            {
                String str = RandomString(&test, maxString);
                insert(test, random(0, test.size()), 
                    str.c_str(), random(0, str.size()));
            }
            break;
        case 37:
            // 21.3.5 modifiers
            insert(test, random(0, test.size()), 
                RandomString(&test, maxString).c_str());
            break;
        case 38:
            // 21.3.5 modifiers
            insert(test, random(0, test.size()), 
                random(0, maxString), random('a', 'z'));
            break;
        case 39:
            // 21.3.5 modifiers
            insert(test, test.begin() + random(0, test.size()), 
                random('a', 'z'));
            break;
        case 40:
            // 21.3.5 modifiers
            {
                std::list<char> lst(RandomList(maxString));
                test.insert(test.begin() + random(0, test.size()), 
                      lst.begin(), lst.end());
            }
            break;
        case 41: 
            // 21.3.5 modifiers
            erase(test, random(0, test.size()), random(0, maxString));
            break;
        case 42:
            // 21.3.5 modifiers
            if(!empty(test))
                erase(test ,test.begin() + random(0, test.size()));
            break;
        case 43:
            // 21.3.5 modifiers
            {
                const typename String::iterator i = test.begin() + random(0, test.size());
                test.erase(i, i + random(0, size_t(test.end() - i)));
            }
            break;
        case 44:
            // 21.3.5 modifiers
            {
                const typename String::size_type pos = random(0, test.size());
                if (avoidAliasing)
                {
                    replace(test, pos, random(0, test.size() - pos), 
                        String(test));
                }
                else
                {
                    replace(test, pos, random(0, test.size() - pos), test);
                }
            }
            break;
        case 45:
            // 21.3.5 modifiers
            {
                const typename String::size_type pos = random(0, test.size());
                replace(test, pos, pos + random(0, test.size() - pos), 
                    RandomString(&test, maxString));
            }
            break;
        case 46:
            // 21.3.5 modifiers
            {
                const size_type 
                    pos1 = random(0, test.size()),
                    pos2 = random(0, test.size());
                if (avoidAliasing)
                {
                    replace(test, pos1, pos1 + random(0, test.size() - pos1), 
                        String(test), 
                        pos2, pos2 + random(0, test.size() - pos2));
                }
                else
                {
                    replace(test, pos1, pos1 + random(0, test.size() - pos1), 
                        test, pos2, pos2 + random(0, test.size() - pos2));
                }
            }
            break;
        case 47:
            // 21.3.5 modifiers
            {
                const size_type pos1 = random(0, test.size());
                String str = RandomString(&test, maxString);
                const size_type pos2 = random(0, str.size());
                replace(test, pos1, pos1 + random(0, test.size() - pos1), 
                    str, pos2, pos2 + random(0, str.size() - pos2));
            }
            break;
        case 48:
            // 21.3.5 modifiers
            {
                const size_type pos = random(0, test.size());
                if (avoidAliasing)
                {
                    replace(test, pos, random(0, test.size() - pos), 
                        String(test).c_str(), test.size());
                }
                else
                {
                    replace(test, pos, random(0, test.size() - pos), 
                        test.c_str(), test.size());
                }
            }
            break;
        case 49:
            // 21.3.5 modifiers
            {
                const size_type pos = random(0, test.size());
                String str = RandomString(&test, maxString);
                replace(test, pos, pos + random(0, test.size() - pos), 
                    str.c_str(), str.size());
            }
            break;
        case 50:
            // 21.3.5 modifiers
            {
                const size_type pos = random(0, test.size());
                String str = RandomString(&test, maxString);
                replace(test, pos, pos + random(0, test.size() - pos), 
                    str.c_str());
            }
            break;
        case 51:
            // 21.3.5 modifiers
            {
                const size_type pos = random(0, test.size());
                replace(test, pos, random(0, test.size() - pos), 
                    random(0, maxString), random('a', 'z'));
            }
            break;
        case 52:
            // 21.3.5 modifiers
            {
                const size_type pos = random(0, test.size());
                if (avoidAliasing)
                {
                    replace(test,
                        test.begin() + pos, 
                        test.begin() + pos + random(0, test.size() - pos), 
                        String(test));
                }
                else
                {
                    replace(test,
                        test.begin() + pos, 
                        test.begin() + pos + random(0, test.size() - pos), 
                        String(test));
                }
            }
            break;
        case 53:
            // 21.3.5 modifiers
            {
                const size_type pos = random(0, test.size());
                if (avoidAliasing)
                {
                    test.replace(
                        test.begin() + pos, 
                        test.begin() + pos + random(0, test.size() - pos), 
                        String(test).c_str(), 
                        test.size() - random(0, test.size()));
                }
                else
                {
                    test.replace(
                        test.begin() + pos, 
                        test.begin() + pos + random(0, test.size() - pos), 
                        test.c_str(), 
                        test.size() - random(0, test.size()));
                }
            }
            break;
        case 54:
            // 21.3.5 modifiers
            {
                const size_type 
                    pos = random(0, test.size()),
                    n = random(0, test.size() - pos);
                typename String::iterator b = test.begin();
                const String str = RandomString(&test, maxString);
                const typename String::value_type* s = str.c_str();
                replace(test,
                    b + pos, 
                    b + pos + n, 
                    s);
            }
            break;
        case 55:
            // 21.3.5 modifiers
            {
                const size_type pos = random(0, test.size());
                test.replace(
                    test.begin() + pos, 
                    test.begin() + pos + random(0, test.size() - pos), 
                    random(0, maxString), random('a', 'z'));
            }
            break;
        case 56:
            // 21.3.5 modifiers
            {
                std::vector<typename String::value_type> 
                    vec(random(0, maxString));
                copy(test,
                    &vec[0], 
                    vec.size(), 
                    random(0, test.size()));
            }
            break;
        case 57:
            // 21.3.5 modifiers
            RandomString(&test, maxString).swap(test);
            break;
        case 58:
            // 21.3.6 string operations
            // exercise c_str() and data()
            assert(test.c_str() == test.data());
            // exercise get_allocator()
            assert(test.get_allocator() == 
                RandomString(&test, maxString).get_allocator());
            break;
        case 59:
            // 21.3.6 string operations
            {
                String str = substr(test,
                    random(0, test.size()), 
                    random(0, test.size()));
                Num2String(test, find(test, str, random(0, test.size())));
            }
            break;
        case 60:
            // 21.3.6 string operations
            {
                String str = substr(test,
                    random(0, test.size()), 
                    random(0, test.size()));
                Num2String(test, find(test, str.c_str(), 
                    random(0, test.size()),
                    random(0, str.size())));
            }
            break;
        case 61:
            // 21.3.6 string operations
            {
                String str = substr(test, 
                    random(0, test.size()), 
                    random(0, test.size()));
                Num2String(test, find(test, str.c_str(), 
                    random(0, test.size())));
            }
            break;
        case 62:
            // 21.3.6 string operations
            Num2String(test, find(test, 
                random('a', 'z'), 
                random(0, test.size())));
            break;
        case 63:
            // 21.3.6 string operations
            {
                String str = substr(test, 
                    random(0, test.size()), 
                    random(0, test.size()));
                Num2String(test, rfind(test, str, random(0, test.size())));
            }
            break;
        case 64:
            // 21.3.6 string operations
            {
                String str = substr(test, 
                    random(0, test.size()), 
                    random(0, test.size()));
                Num2String(test, rfind(test, str.c_str(), 
                    random(0, test.size()),
                    random(0, str.size())));
            }
            break;
        case 65:
            // 21.3.6 string operations
            {
                String str = substr(test, 
                    random(0, test.size()), 
                    random(0, test.size()));
                Num2String(test, rfind(test, str.c_str(), 
                    random(0, test.size())));
            }
            break;
        case 66:
            // 21.3.6 string operations
            Num2String(test, rfind(test, 
                random('a', 'z'), 
                random(0, test.size())));
            break;
        case 67:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_first_of(test, str, 
                    random(0, test.size())));
            }
            break;
        case 68:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_first_of(test, str.c_str(), 
                    random(0, test.size()),
                    random(0, str.size())));
            }
            break;
        case 69:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_first_of(test, str.c_str(), 
                    random(0, test.size())));
            }
            break;
        case 70:
            // 21.3.6 string operations
            Num2String(test, find_first_of(test, 
                random('a', 'z'), 
                random(0, test.size())));
            break;
        case 71:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_last_of(test, str, 
                    random(0, test.size())));
            }
            break;
        case 72:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_last_of(test, str.c_str(), 
                    random(0, test.size()),
                    random(0, str.size())));
            }
            break;
        case 73:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_last_of(test, str.c_str(), 
                    random(0, test.size())));
            }
            break;
        case 74:
            // 21.3.6 string operations
            Num2String(test, find_last_of(test, 
                random('a', 'z'), 
                random(0, test.size())));
            break;
        case 75:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_first_not_of(test, str, 
                    random(0, test.size())));
            }
            break;
        case 76:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_first_not_of(test, str.c_str(), 
                    random(0, test.size()),
                    random(0, str.size())));
            }
            break;
        case 77:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_first_not_of(test, str.c_str(), 
                    random(0, test.size())));
            }
            break;
        case 78:
            // 21.3.6 string operations
            Num2String(test, find_first_not_of(test, 
                random('a', 'z'), 
                random(0, test.size())));
            break;
        case 79:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_last_not_of(test, str, 
                    random(0, test.size())));
            }
            break;
        case 80:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_last_not_of(test, str.c_str(), 
                    random(0, test.size()),
                    random(0, str.size())));
            }
            break;
        case 81:
            // 21.3.6 string operations
            {
                String str = RandomString(&test, maxString);
                Num2String(test, find_last_not_of(test, str.c_str(), 
                    random(0, test.size())));
            }
            break;
        case 82:
            // 21.3.6 string operations
            Num2String(test, find_last_not_of(test, 
                random('a', 'z'), 
                random(0, test.size())));
            break;
        case 83:
            // 21.3.6 string operations
            test = substr(test, random(0, test.size()), random(0, test.size()));
            break;
        case 84:
            {
                int tristate = compare(test, RandomString(&test, maxString));
                if (tristate > 0) tristate = 1;
                else if (tristate < 0) tristate = 2;
                Num2String(test, tristate);
            }
            break;
        case 85:
            {
                int tristate = compare(test, 
                    random(0, test.size()), 
                    random(0, test.size()), 
                    RandomString(&test, maxString));
                if (tristate > 0) tristate = 1;
                else if (tristate < 0) tristate = 2;
                Num2String(test, tristate);
            }
            break;
        case 86:
            {
                String str = RandomString(&test, maxString);
                int tristate = compare(test, 
                    random(0, test.size()), 
                    random(0, test.size()), 
                    str,
                    random(0, str.size()), 
                    random(0, str.size()));
                if (tristate > 0) tristate = 1;
                else if (tristate < 0) tristate = 2;
                Num2String(test, tristate);
            }
            break;
        case 87:
            {
                int tristate = compare(test, 
                    RandomString(&test, maxString).c_str());
                if (tristate > 0) tristate = 1;
                else if (tristate < 0) tristate = 2;
                Num2String(test, tristate);
            }
            break;
        case 88:
            {
                String str = RandomString(&test, maxString);
                int tristate = compare(test, 
                    random(0, test.size()), 
                    random(0, test.size()), 
                    str.c_str(),
                    random(0, str.size()));
                if (tristate > 0) tristate = 1;
                else if (tristate < 0) tristate = 2;
                Num2String(test, tristate);
            }
            break;
        case 89:
            test = RandomString(&test, maxString) + 
                RandomString(&test, maxString);
            break;
        case 90:
            test = RandomString(&test, maxString).c_str() + 
                RandomString(&test, maxString);
            break;
        case 91:
            test = typename String::value_type(random('a', 'z')) + 
                RandomString(&test, maxString);
            break;
        case 92:
            test = RandomString(&test, maxString) + 
                RandomString(&test, maxString).c_str();
            break;
        case 93:
            test = RandomString(&test, maxString) + 
                RandomString(&test, maxString).c_str();
            break;
        case 94:
            test = RandomString(&test, maxString) + 
                typename String::value_type(random('a', 'z'));
            break;
        case 95:
			{
				String s(test);
            test = RandomString(&test, maxString) ; 
				boost::swap(test,s);
            break;
			}
        default:
            //assert(((functionSelector + 1) % 96) == 0);
            break;
        }
    }
    return test;
}

template<class T>
void checkResults(const std::string& reference, const T& tested)
{
    if( (tested.size() != reference.size())||
        (std::string(tested.data(), tested.size()) != reference) )
    {
        std::cout << "\nTest " << currentTest << " failed: \n";
        std::cout << "reference.size() = " << reference.size() << "\n";
        std::cout << "tested.size()    = " << tested.size()    << "\n";
        std::cout << "reference data   = " <<  reference << "\n";
        std::cout << "tested    data   = " <<  tested    << "\n";
    }

    //assert(tested.size() == reference.size());
    //assert(std::string(tested.data(), tested.size()) == reference);
}

void Compare()
{
    unsigned int count = 0;
    srand(10);
    for (;;)
    {
        cout << ++count << '\r';
        
        unsigned long t = rand(); //time(0);

        srand(t);
        //const std::string reference = Test(std::string(), 1, true);

        {
            srand(t);
			typedef yasli::flex_string<
                char,
                std::char_traits<char>,
                std::allocator<char>,
				yasli::SimpleStringStorage<char, std::allocator<char> >
            > my_string;
           /* const my_string tested =*/ Test(my_string(), 1, false);
            //checkResults(reference, tested);
        }

        //{
        //    srand(t);
        //    typedef flex_string<
        //        char,
        //        std::char_traits<char>,
        //        std::allocator<char>,
        //        AllocatorStringStorage<char, std::allocator<char> >
        //    > my_string;
        //    const my_string tested = Test(my_string(), 1, false);
        //    checkResults(reference, tested);
        //}

        //{
        //    srand(t);
        //    typedef flex_string<
        //        char,
        //        std::char_traits<char>,
        //        mallocator<char>,
        //        AllocatorStringStorage<char, mallocator<char> >
        //    > my_string;
        //    const my_string tested = Test(my_string(), 1, false);
        //    checkResults(reference, tested);
        //}

        //{
        //    srand(t);
        //    typedef flex_string<
        //        char,
        //        std::char_traits<char>,
        //        std::allocator<char>,
        //        VectorStringStorage<char, std::allocator<char> >
        //    > my_string;
        //    const my_string tested = Test(my_string(), 1, false);
        //    checkResults(reference, tested);
        //}
        //{
        //    srand(t);
        //    typedef VectorStringStorage<char, std::allocator<char> >
        //        Storage;
        //    typedef flex_string<
        //        char,
        //        std::char_traits<char>,
        //        std::allocator<char>,
        //        SmallStringOpt<Storage, 23>
        //    > my_string;
        //    static my_string sample;
        //    const my_string tested(Test(sample, 1, false));
        //    checkResults(reference, tested);
        //}
        //{
        //    srand(t);
        //    typedef SimpleStringStorage<char, std::allocator<char> >
        //        Storage;
        //    typedef flex_string<
        //        char,
        //        std::char_traits<char>,
        //        std::allocator<char>,
        //        CowStringOpt<Storage>
        //    > my_string;
        //    static my_string sample;
        //    const my_string tested(Test(sample, 1, false));
        //    checkResults(reference, tested);
        //}
        //{
        //    srand(t);
        //    typedef AllocatorStringStorage<char, std::allocator<char> >
        //        Storage;
        //    typedef flex_string<
        //        char,
        //        std::char_traits<char>,
        //        std::allocator<char>,
        //        CowStringOpt<Storage>
        //    > my_string;
        //    static my_string sample;
        //    const my_string tested(Test(sample, 1, false));
        //    checkResults(reference, tested);
        //}
/*
        {    // SimpleStringStorage with UTF16 Encoding
            srand(t);
            typedef SimpleStringStorage<unicode::UTF16Char>
                Storage;
            typedef flex_string<
                unicode::UTF16Char,
                std::char_traits<unicode::UTF16Char>,
                std::allocator<unicode::UTF16Char>,
                UTF16Encoding<Storage>
            > my_string;
            static my_string sample;
            const my_string tested(Test(sample, 1, false));
            assert(tested.size() == reference.size());
            //assert(std::string(tested.data(), tested.size()) == reference);
       }
       */
    }
}

/// This function tests out a bug found by Jean-Francois Bastien in the find function.
void TestBug2536490( void )
{

    my_string bug;
    std::cout << "Index of '6' in \"" << bug.c_str() << "\": " << find(bug,'6') << std::endl;
    bug = "12345";
    std::cout << "Index of '6' in \"" << bug.c_str() << "\": " << find(bug,'6') << std::endl;
    bug = "123456";
    std::cout << "Index of '6' in \"" << bug.c_str() << "\": " << find(bug,'6') << std::endl;
    bug = "12345";
    std::cout << "Index of '6' in \"" << bug.c_str() << "\": " << find(bug,'6') << std::endl;

    bug = "12345";
    std::cout << "Index of '123' in \"" << bug.c_str() << "\": " << find(bug,"123") << std::endl;
    bug = "12345";
    std::cout << "Index of '12345' in \"" << bug.c_str() << "\": " << find(bug,"12345") << std::endl;
    bug = "12345";
    std::cout << "Index of '345' in \"" << bug.c_str() << "\": " << find(bug,"345") << std::endl;
    bug = "123456";
    std::cout << "Index of '456' in \"" << bug.c_str() << "\": " << find(bug,"456") << std::endl;
    bug = "12345";
    std::cout << "Index of '456' in \"" << bug.c_str() << "\": " << find(bug,"456") << std::endl;
}

/*
#include <string>
#include <limits>
#include <cassert>
//#include "flex_string.h"

int main()
{
   typedef wchar_t char_type;
   typedef std::basic_string<char_type> std_string_t;

   typedef flex_string<
    char_type,
    std::char_traits<char_type>,
    std::allocator<char_type>,
    SimpleStringStorage<char_type, std::allocator<char_type> > > flex_string_t;

   flex_string_t s1(L"A flex string");
   std_string_t s2(L"A std string");

   assert(std::numeric_limits<char_type>::is_specialized);
 
    assert(std::numeric_limits<std_string_t::iterator::value_type>::is_specialized); 
    s1.replace<std_string_t::iterator>(s1.begin(), s1.begin(), 
    s2.begin(),s2.end());
    return 0;
}
*/



int main_2()
{
	//std::binary_search()
 //   TestBug2536490();
 //   srand(unsigned(time(0)));
 //   //Compare();
	//my_string str("worldsoccer");
	//std::cout<<str<<std::endl;
	//replace(str, str.begin(),str.end()-6, "hello,");
	//std::cout<<str<<std::endl;
	////std::cout<<(find_first_of(str,"ec") - str.begin());
	//std::cout<<substr(str,  find_first_of(str,"ec"),length(str)-find_first_of(str,"ec"));
	//std::cout<<endl;
	//str += "nihao;";
	//std::cout<<str<<std::endl;
	//for (my_string::iterator i = str.begin(); i!= str.end(); ++i)
	//{
	//	std::cout<<*i<<" ";
	//}
	//std::cout<<std::endl;
	yasli::string str2("worldsoccer");
	std::cin>>str2;
	//std::cout<<str<<" --1:2-- "<<str2<<std::endl;
	//std::cout<<compare(str,str2)<<std::endl;
	//swap(str,str2);
	//std::cout<<str<<" --1:2-- "<<str2<<std::endl;
	//std::cout<<compare(str,str2)<<std::endl;
	//std::cout<<substr(str, rfind(str,"o"),length(str)-rfind(str,"o"));
	//std::cout<<endl;
	//std::cout<<substr(str, find_last_of(str,"dco"),length(str)-find_last_of(str,"dco"))<<std::endl;
	//std::cout<<str+'c'<<std::endl;
	//std::cout<<erase(str, str.begin()+2);
	//std::cout<<endl;
	//std::cout<<erase(str, 0,2);
	//std::cout<<endl;
	//std::cout<<(str<str2)<<std::endl;
	//std::cout<<(str2<str)<<std::endl;
	//assign(str, str2.begin(), str2.end());
	//std::cout<<str<<std::endl;
	//		std::cout<<endl;
	//		std::cout<<endl;
	//		std::cout<<endl;
	{
		int i = 0;
		std::ifstream ifs("world.txt");
		while (yasli::getline(ifs,str2))
		{
			std::cout<<i<<" ";
			std::cout<<str2;
			std::cout<<endl;
			++i;
		}
		ifs.close();
	}
	
	//{
	//	int i = 0;
	//	std::string s;
	//	std::ifstream ifs("world.txt");
	//	std::cin>>s;
	//	{
	//		std::cout<<"i"<<i<<" ";
	//		std::cout<<s;
	//		std::cout<<endl;
	//		++i;
	//	}
	//}
    return 0;
}
