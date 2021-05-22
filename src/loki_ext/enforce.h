//-*-c++-*-
// This code is stolen by Markus Werle from an article by Andrei Alexandrescou
// http://www.cuj.com/documents/s=8250/cujcexp2106alexandr/
// There was no copyright header in this file, so I asked Andrei Alexandrescou 
// via mail about his opinion. This is what he wrote:

// "The license (which I forgot to include in the file) is almost identical to
// that in Loki. Just remove Addison Wesley, add Petru Marginean, and remove
// the reference to Modern C++ Design.
//
// John Torjo is working on putting Enforce into Boost."

// Many thanks to Andrei!


// modified by Markus Werle

#ifndef ENFORCE_H_
#define ENFORCE_H_

#include <string>
#include <sstream>
#include <stdexcept>

struct DefaultPredicate
{
  template <class T>
  static bool Wrong(const T& obj)
  {
    return !obj;
  }
};

struct DefaultRaiser
{
    template <class T>
    static void Throw(const T&, const std::string& message, const char* locus)
    {
        throw std::runtime_error(message + '\n' + locus);
    }
};

template<typename Ref, typename P, typename R>
class Enforcer
{
public:
    Enforcer(Ref t, const char* locus) : t_(t), locus_(P::Wrong(t) ? locus : 0)
    {
    }

    Ref operator*() const
    {
        if (locus_) R::Throw(t_, msg_, locus_);
        return t_;
    }

    template <class MsgType>
    Enforcer& operator()(const MsgType& msg)
    {
        if (locus_) 
        {
            // Here we have time; an exception will be thrown
            std::ostringstream ss;
            ss << msg;
            msg_ += ss.str();
        }
        return *this;
    }

private:
    Ref t_;
    std::string msg_;
    const char* const locus_;
};

template <class P, class R, typename T>
inline Enforcer<const T&, P, R> 
MakeEnforcer(const T& t, const char* locus)
{
    return Enforcer<const T&, P, R>(t, locus);
}

template <class P, class R, typename T>
inline Enforcer<T&, P, R> 
MakeEnforcer(T& t, const char* locus)
{
    return Enforcer<T&, P, R>(t, locus);
}


struct DummyNoActionEnforcer
{
  template <class MsgType>
  DummyNoActionEnforcer& operator()(const MsgType& msg) 
  {
    return *this;
  }
};



#define STRINGIZE(expr) STRINGIZE_HELPER(expr)
#define STRINGIZE_HELPER(expr) #expr

#define ENFORCE(exp) \
    *MakeEnforcer<DefaultPredicate, DefaultRaiser>((exp), "Expression '" #exp "' failed in '" \
    __FILE__ "', line: " STRINGIZE(__LINE__))


#ifndef NDEBUG

#  define DEBUG_ENFORCE(exp) \
    *MakeEnforcer<DefaultPredicate, DefaultRaiser>((exp), "Expression '" #exp "' failed in '" \
    __FILE__ "', line: " STRINGIZE(__LINE__))

#else

#  define DEBUG_ENFORCE(exp) DummyNoActionEnforcer()

#endif // NDEBUG


#endif