#include <iostream>
#include <cstdint>
#include <typeinfo>
#include <boost/core/demangle.hpp>
#include "sparse/csr_matrix.h"
#include "dbg.h"

using std::cout, std::size_t;
using value_t = signed char;
using subscript_t = unsigned char;
using index_t = unsigned char;
using entry_t = sparse::coo_entry<value_t, subscript_t>;
constexpr std::array coo_data = std::to_array<entry_t>({
    {1,0,10},
    {1,1,11},
    {1,2,12},
    {4,4,44},
    {5,3,53},
    {2,2,22},
    {2,6,26},
    {2,4,24},
    {2,3,23},
    {4,5,45},
    {0,4,94},
    {0,0,99},
});

constexpr auto coo_mat = sparse::make_coo_matrix(coo_data, -1);
constexpr auto csr_mat_1 = sparse::make_csr_matrix_from_coo(index_t, coo_mat);

constexpr auto csr_mat_2 = sparse::make_csr_matrix(index_t, coo_data, -1);

constexpr auto csr_mat_3 = sparse::make_csr_matrix(index_t, std::to_array<entry_t>({{0,255,1}}), -1);
constexpr auto csr_mat_4 = sparse::make_csr_matrix(index_t, std::to_array<entry_t>({{254,0,1}}), -1);
constexpr auto csr_mat_5 = sparse::make_csr_matrix(index_t, std::to_array<entry_t>({{254,255,1}}), -1);

int main() {
    dbg_v(sizeof(coo_data));
    dbg_th(&coo_data);
    
    dbg_v(sizeof(coo_mat));
    dbg_th(&coo_mat);
    dbg_th(&(coo_mat.data));

    dbg_csr_mat(csr_mat_1);
    dbg_csr_mat(csr_mat_2);
    dbg_csr_mat(csr_mat_3);
    dbg_csr_mat(csr_mat_4);
    dbg_csr_mat(csr_mat_5);

    return 0;
}