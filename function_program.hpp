#ifndef FUNCTION_PROGRAM_H_
#define FUNCTION_PROGRAM_H_
namespace fp {
namespace View {

template <class Container>
class Range {
    Range(Container&& container) : container_(std::forward<Container>(container)) {}
    Container container_;
};

template <class Functor>
struct TransformWrap {
    TransformWrap(Functor functor) : functor_(std::forward<Functor>(functor)){}
    using Functor_Type = Functor;
    Functor functor_;
};
template<class Functor>
auto transform(Functor&& functor) -> TransformWrap<Functor>{
    return TransformWrap<Functor>(functor);
}

}
}  // namespace fp
#endif           // FUNCTION_PROGRAM_H_