#include "sparse/csr_matrix.h"
#include "dbg.h"

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
constexpr auto csr_mat = sparse::make_csr_matrix(coo_data, -1);

int main() {
    dbg_v(sizeof(coo_data));
    dbg_th(&coo_data);
    
    dbg_v(sizeof(coo_mat));
    dbg_th(&coo_mat);
    dbg_th(&(coo_mat.data));

    dbg_csr_mat(csr_mat);

    return 0;
}