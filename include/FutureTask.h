// -*- mode: c++ -*-
#ifndef _TPOOL_FUTURE_TASK_H_
#define _TPOOL_FUTURE_TASK_H_

#include "TaskBase.h"
#include "ConditionVariable.h"
#include <boost/bind.hpp>
#include <functional>

namespace tpool {
  template <typename T>
  class FutureTask : public TaskBase {
  public:
    FutureTask();
    virtual ~FutureTask();

    T GetResult();
    
  private:
    virtual void DoRun();
    virtual T Call() = 0;
    bool IsResultReturned() const;

    T m_result;
    bool m_isResultReturned;
    sync::MutexConditionVariable m_resultCondition;
  };

  // Partial specialization for void, which forbids
  // users from defining a method returning void.
  template <>
  class FutureTask<void>;


  // Implementation
  template <typename T>
  FutureTask<T>::FutureTask()
    : m_isResultReturned(false)
  {}

  template <typename T>
  FutureTask<T>::~FutureTask()
  {}

  template <typename T>
  T FutureTask<T>::GetResult()
  {
    using std::not1;
    using std::mem_fun;
    using boost::bind;
    
    sync::ConditionWaitLocker l(m_resultCondition,
				bind(not1(mem_fun(&FutureTask::IsResultReturned)),
				     this));
    return m_result;
  }

  template <typename T>
  void FutureTask<T>::DoRun()
  {
    m_result = Call();
  }

  template <typename T>
  bool FutureTask<T>::IsResultReturned() const
  {
    return m_isResultReturned;
  }
}

#endif