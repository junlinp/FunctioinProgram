#ifndef FUNCTION_PROGRAM_H_
#define FUNCTION_PROGRAM_H_
#include <numeric>
#include "thread_pool.hpp"
namespace fp {
namespace View {

template <class Container>
class Range {
  Range(Container&& container)
      : container_(std::forward<Container>(container)) {}
  Container container_;
};

template <class Functor>
struct TransformWrap {
  TransformWrap(Functor&& functor) : functor_(std::forward<Functor>(functor)) {}
  using Functor_Type = Functor;
  Functor functor_;
};

template <class Functor>
auto transform(Functor&& functor) -> TransformWrap<Functor> {
  return TransformWrap<Functor>(std::forward<Functor>(functor));
}

template <class Container, class Functor>
class TransformRange {
 public:
  TransformRange(Container&& container, Functor&& functor)
      : container_(std::forward<Container>(container)),
        functor_(std::forward<Functor>(functor)) {}

  using value_type = typename std::result_of_t<std::remove_reference_t<Functor>(
      typename std::remove_reference_t<Container>::value_type)>;

  template <class Iterator>
  class TransformRangeConstIterator
      : public std::iterator<std::forward_iterator_tag, const value_type> {
   public:
    TransformRangeConstIterator(Iterator iterator,const Functor& functor)
        : input_iterator_(iterator), functor_(functor) {}
    TransformRangeConstIterator(const TransformRangeConstIterator& other)
        : input_iterator_(other.input_iterator_), functor_(other.functor_) {}
    TransformRangeConstIterator& operator++() {
      ++input_iterator_;
      return *this;
    }
    bool operator!=(const TransformRangeConstIterator& rhs) {
      return input_iterator_ != rhs.input_iterator_;
    }
    const value_type operator*() const { return functor_(*input_iterator_); }

   private:
    Iterator input_iterator_;
    const Functor& functor_;
  };
  using const_iterator = TransformRangeConstIterator<
      typename std::remove_reference_t<Container>::const_iterator>;

  const_iterator begin() const noexcept {
    return const_iterator(container_.begin(), functor_);
  }
  const_iterator cbegin() const noexcept {
    return const_iterator(container_.cbegin(), functor_);
  }

  const_iterator end() const noexcept {
    return const_iterator(container_.end(), functor_);
  }
  const_iterator cend() const noexcept {
    return const_iterator(container_.cend(), functor_);
  }

 private:
  Container container_;
  Functor functor_;
};

template <class Container, class Functor>
TransformRange<Container, Functor> operator|(
    Container&& container, TransformWrap<Functor>&& transform_wrap) {
  return TransformRange<Container, Functor>(
      std::forward<Container>(container),
      std::forward<Functor>(transform_wrap.functor_));
}

struct GetKey {};
struct GetValue {};

template <class Container>
auto operator|(Container&& container, GetKey) {
  using Container_T = std::remove_reference_t<Container>;
  using Functor = std::function<typename Container_T::key_type(
      const typename Container_T::value_type&)>;
  Functor functor = [](const typename Container_T::value_type& element) {
    return element.first;
  };
  return TransformRange<Container, Functor>(std::forward<Container>(container),
                                            std::move(functor));
}
template <class Container>
auto operator|(Container&& container, GetValue) {
  using Container_T = std::remove_reference_t<Container>;
  using Functor = std::function<typename Container_T::mapped_type(
      const typename Container_T::value_type&)>;
  Functor functor = [](const typename Container_T::value_type& element) {
    return element.second;
  };
  return TransformRange<Container, Functor>(std::forward<Container>(container),
                                            std::move(functor));
}

template <class T>
struct ReduceIndicator {
  constexpr ReduceIndicator(T&& value) : value_(std::forward<T>(value)) {}
  T value_;
};

template <class T>
ReduceIndicator<T> reduce(T&& initial_value) {
  return ReduceIndicator<T>(std::forward<T>(initial_value));
}

template <class Container, class T>
T operator|(Container&& container, ReduceIndicator<T>&& indicator) {
  return std::accumulate(container.cbegin(), container.cend(),
                         indicator.value_);
}

template <class T, class BinaryFunctor>
struct ReduceFunctorIndicator {
  ReduceFunctorIndicator(T&& initial_value, BinaryFunctor&& plus_op)
      : value_(std::forward<T>(initial_value)),
        plus_op_(std::forward<BinaryFunctor>(plus_op)) {}

  T value_;
  BinaryFunctor plus_op_;
};

/**
 *
 *  binary_functor will be:
 *  Ret binary_functor(const Type1& a, const Type2& b)
 *  The signature does not need to have const &.
 *  The type Type1 must be such that an object of type T can be implicitly
 * converted to Type1. The type Type2 must be such that an object of type
 * InputIt can be dereferenced and then implicitly converted to Type2. The type
 * Ret must be such that an object of type T can be assigned a value of type
 * Ret.​
 */
template <class T, class BinaryFunctor>
auto reduce(T&& initial_value, BinaryFunctor&& binary_functor) {
  return ReduceFunctorIndicator<T, BinaryFunctor>(
      std::forward<T>(initial_value),
      std::forward<BinaryFunctor>(binary_functor));
}
template <class Container, class T, class BinaryFunctor>
auto operator|(Container&& container,
               ReduceFunctorIndicator<T, BinaryFunctor>&& indicator) {
  return std::accumulate(container.cbegin(), container.cend(), indicator.value_,
                         indicator.plus_op_);
}

template <class Functor>
struct FilterIndicator {
  FilterIndicator(Functor&& unary_functor)
      : functor_(std::forward<Functor>(unary_functor)) {}
  Functor functor_;
};
template <class Functor>
FilterIndicator<Functor> filter(Functor&& unary_functor) {
  return FilterIndicator<Functor>(std::forward<Functor>(unary_functor));
}

template <class Container, class Functor>
class FilterRange {
 private:
  Container container_;
  Functor functor_;
};

template <class Container, class Functor>
auto operator|(Container&& container, FilterIndicator<Functor>&& indicator) {
  return FilterRange<Container, Functor>(
      std::forward<Container>(container),
      std::forward<Functor>(indicator.functor_));
}

}  // namespace View

namespace Action {
template <class Functor>
struct ParallelTransformIndicator {
  ParallelTransformIndicator(Functor&& functor)
      : functor_(std::forward<Functor>(functor)) {}
  Functor functor_;
};

template <class Functor>
ParallelTransformIndicator<Functor> ParallelTransform(Functor&& functor) {
  return ParallelTransformIndicator<Functor>(std::forward<Functor>(functor));
}

template <class Container, class Functor,
  class ResultType = std::result_of_t<Functor(typename Container::value_type)>>
std::vector<ResultType>
operator|(Container&& container,
          ParallelTransformIndicator<Functor> indicator) {
  std::vector<ResultType> res;
  std::vector<std::future<ResultType>> future_res;
  {
    ThreadPool thread_pool;
    std::transform(container.cbegin(), container.cend(),
                   std::back_inserter(future_res),
                   [&thread_pool, &indicator](const auto& element) {
                     return thread_pool.Enqueue(
                         std::forward<Functor>(indicator.functor_), element);
                   });
  }

  std::transform(future_res.cbegin(), future_res.cend(), std::back_inserter(res),[](const auto& element){return element.get();});
  return res;
}

}  // namespace Action

}  // namespace fp
#endif  // FUNCTION_PROGRAM_H_