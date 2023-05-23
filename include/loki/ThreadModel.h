
#include <mutex>
namespace Loki {

template<class Host, class MutexPolicy = std::mutex>
class ObjectLevelLockable
{
  mutable MutexPolicy mtx_;

public:
  ObjectLevelLockable() : mtx_() {}

  ObjectLevelLockable(const ObjectLevelLockable &) : mtx_() {}

  ~ObjectLevelLockable() {}

  class Lock;
  friend class Lock;

  ///  \struct Lock
  ///  Lock class to lock on object level
  class Lock
  {
  public:
    /// Lock object
    explicit Lock(const ObjectLevelLockable &host) : host_(host)
    {
      host_.mtx_.lock();
    }

    /// Lock object
    explicit Lock(const ObjectLevelLockable *host) : host_(*host)
    {
      host_.mtx_.lock();
    }

    /// Unlock object
    ~Lock()
    {
      host_.mtx_.unlock();
    }

  private:
    /// private by design of the object level threading
    Lock();
    Lock(const Lock &);
    Lock &operator=(const Lock &);
    const ObjectLevelLockable &host_;
  };
};
template<class Host, class MutexPolicy = std::mutex>
class ClassLevelLockable
{
  struct Initializer
  {
    bool init_;
    MutexPolicy mtx_;

    Initializer() : init_(false)
    {
      init_ = true;
    }

    ~Initializer()
    {
    }
  };

  static Initializer initializer_;

public:
  class Lock;
  friend class Lock;

  ///  \struct Lock
  ///  Lock class to lock on class level
  class Lock
  {
  public:
    /// Lock class
    Lock()
    {
      initializer_.mtx_.lock();
    }

    /// Lock class
    explicit Lock(const ClassLevelLockable &)
    {
      initializer_.mtx_.lock();
    }

    /// Lock class
    explicit Lock(const ClassLevelLockable *)
    {
      initializer_.mtx_.lock();
    }

    /// Unlock class
    ~Lock()
    {
      initializer_.mtx_.unlock();
    }

  private:
    Lock(const Lock &) = delete;
    Lock &operator=(const Lock &) = delete;
  };
};

template<class Host, class MutexPolicy>
typename ClassLevelLockable<Host, MutexPolicy>::Initializer
  ClassLevelLockable<Host, MutexPolicy>::initializer_;

}// namespace Loki
