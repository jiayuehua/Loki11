//#include <iostream>
//#include <vector>
//#include <condition_variable>
//#include <mutex>
//#include <array>
//
//template<class T, int size>
//struct Queue
//{
//private:
//  std::condition_variable rd_;
//  std::condition_variable wr_;
//  std::mutex m_;
//  std::array<T, size> a_;
//  int head_ = 0;
//  int tail_ = 0;
//  bool isEmpty() const
//  {
//    return head_ == tail_;
//  }
//  bool isFull() const
//  {
//    return (head_ + 1) % size == tail_;
//  }
//
//
//public:
//  T pop()
//  {
//    std::unique_lock l(m_);
//    rd_.wait(l, [this]() {
//      return !isEmpty();
//    });
//    T t = a_[tail_];
//    tail_ = (tail_ + 1) % size;
//    wr_.notify_one();
//    return t;
//  }
//  void push(const T &t)
//  {
//    std::unique_lock l(m_);
//    wr_.wait(l, [this]() {
//      return !isFull();
//    });
//    a_[head_] = t;
//    head_ = (head_ + 1) % size;
//    rd_.notify_one();
//  }
//};