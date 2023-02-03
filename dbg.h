#include <iostream>
#include <cstdint>
#include <typeinfo>
#include <boost/core/demangle.hpp>

#define dbg_v(arg) cout << #arg << " = " << std::dec << +arg << "\n"
#define dbg_t(arg) cout << #arg << " = (" << boost::core::demangle(typeid(arg).name()) << ")\n"
#define dbg_tv(arg) cout << #arg << " = " << std::dec << +arg << " (" << boost::core::demangle(typeid(arg).name()) << ")\n"
#define dbg_h(arg) cout << #arg << " = 0x" << std::hex << +arg << "\n"
#define dbg_th(arg) cout << #arg << " = 0x" << std::hex << +arg << " (" << boost::core::demangle(typeid(arg).name()) << ")\n"
#define dbg_mat(arg, ...) dbg_mat_impl(#arg, arg  __VA_OPT__(,) __VA_ARGS__)
#define dbg_arr(arg, ...) dbg_arr_impl(#arg, arg  __VA_OPT__(,) __VA_ARGS__)
#define dbg_get(arg, ...) dbg_get_impl(#arg, [](auto i){return arg(i);}  __VA_OPT__(,) __VA_ARGS__)

#define dbg_csr_mat(arg) \
    dbg_v(sizeof(arg)); \
    dbg_th(&arg); \
    dbg_mat(arg); \
    dbg_th(&(arg.data)); \
    dbg_arr(arg.data); \
    dbg_th(&(arg.row_pos)); \
    dbg_arr(arg.row_pos); \
    dbg_th(&(arg.row_indent)); \
    dbg_arr(arg.row_indent);


void dbg_mat_impl(
        const auto& name,
        const auto& arg,
        const std::size_t imax = 10, 
        const std::size_t jmax = 10,
        decltype(std::hex) conv = std::dec) {
    std::cout << name << " = [\n"; 
    for (size_t i = 0; i < imax; ++i) {
        std::cout << " [ ";
        for (size_t j = 0; j < jmax; ++j) {
            auto value = arg.get(i,j);
            std::cout << conv << +value << " ";
        }
        std::cout << "]\n";
    }
    std::cout << "] (" << boost::core::demangle(typeid(arg).name()) << ") of (" << boost::core::demangle(typeid(arg.get(0,0)).name()) << ")\n";
}
void dbg_arr_impl(
        const auto& name,
        const auto& arg,
        decltype(std::hex) conv = std::dec) {
    // using value_t = decltype(arg.get(0,0));
    // using cast_t = typename std::conditional<std::is_signed<value_t>::value, signed long, unsigned long>::type;
    std::cout << name << " = [ "; 
    for (const auto & value : arg) {
        std::cout << conv << +value << " ";
    }
    std::cout << "] (" << boost::core::demangle(typeid(arg).name()) << ") of (" << boost::core::demangle(typeid(arg[0]).name()) << ")\n";
}
void dbg_get_impl(
        const auto& name,
        const auto& arg,
        const std::size_t imax = 10,
        decltype(std::hex) conv = std::dec) {
    // using value_t = decltype(arg.get(0,0));
    // using cast_t = typename std::conditional<std::is_signed<value_t>::value, signed long, unsigned long>::type;
    std::cout << name << " = [ "; 
    for (size_t i = 0; i < imax; ++i) {
        auto value = arg(i);
        std::cout << conv << +value << " ";
    }
    std::cout << " ... ] (getter) of (" << boost::core::demangle(typeid(arg(0)).name()) << ")\n";
}