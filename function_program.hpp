#ifndef FUNCTION_PROGRAM_H_
#define FUNCTION_PROGRAM_H_
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
  TransformRange(Container container, Functor functor)
      : container_(std::forward<Container>(container)),
        functor_(std::forward<Functor>(functor)) {}

  using value_type = typename std::result_of_t<std::remove_reference_t<Functor>(
      typename std::remove_reference_t<Container>::value_type)>;

  template <class Iterator>
  class TransformRangeConstIterator
      : std::iterator<std::forward_iterator_tag, const value_type> {
   public:
    TransformRangeConstIterator(Iterator iterator, Functor functor)
        : input_iterator_(iterator), functor_(std::forward<Functor>(functor)) {}

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
    Functor functor_;
  };
  using const_iterator = TransformRangeConstIterator<
      typename std::remove_reference_t<Container>::const_iterator>;

  const_iterator begin() const noexcept {
    return const_iterator(container_.begin(), functor_);
  }

  const_iterator end() const noexcept {
    return const_iterator(container_.end(), functor_);
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

/*
template < class T>
ReduceWrap reduce(T&& initial_value) {

}

tmeplate <class T, class BinaryFunctor>
ReduceWrap reduce(T&& initial_value, BinaryFunctor&& binary_functor) {

}
*/
}  // namespace View
}  // namespace fp
#endif  // FUNCTION_PROGRAM_H_