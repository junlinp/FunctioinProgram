#ifndef FUNCTION_PROGRAM_H_
#define FUNCTION_PROGRAM_H_
namespace fp {
namespace View {

template <class Contaner>
class Range {
    Range(Container&& container) : container_(std::forward<Container>(container)) {}
    Contaner& container_;
};

}
}  // namespace fp
#endif           // FUNCTION_PROGRAM_H_