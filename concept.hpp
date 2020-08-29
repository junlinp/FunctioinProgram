#ifndef FUNCTION_PROGRAM_CONCEPT_HPP_
#define FUNCTION_PROGRAM_CONCEPT_HPP_

template <class T>
class Dereference {
  using yes = int;
  using no = char;
  template <class C>
  static yes test(decltype(&C::operator*));
  template <class C>
  static no test(...) {
    static_assert(false, "Class or Struct should be dereference");
  };
 public:
  static bool const value = sizeof(test<T>(0)) == sizeof(yes);
};
template <class T>
class Decrementable {
  using yes = int;
  using no = char;
  template <class C>
  static yes test(decltype(&C::operator--));
  template <class C>
  static no test(...) {
    static_assert(false, "Class or Struct should be dereference");
  };
 public:
  static bool const value = sizeof(test<T>(0)) == sizeof(yes);
};
#endif  // FUNCTION_PROGRAM_CONCEPT_HPP_