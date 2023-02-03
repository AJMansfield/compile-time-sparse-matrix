#include <cstddef>
#include <array>
#include <algorithm>
#include <tuple>
#include <compare>
#include <stdexcept>
#include <limits>
#include <ranges>
#include <boost/integer.hpp>

#include "coo_matrix.h"

namespace sparse {

/* store a 2D matrix in Compressed Sparse Row format.
For example, take the matrix:
    0 0 1 0
    0 0 0 0
    0 2 0 3
Storing it with fill=0, only the following items actually need to be stored:
    - - 1 -
    - - - -
    - 2 0 3
This is laid out in the data array as: [1] [] [2 0 3]
With an index of the row locations:    0   1  1       4
And each row's starting subscript:     2   -  1
The row location array has an additional entry at the end equal to data_size so the last entry doesn't have to be special-cased.
The offset value for a blank row doesn't matter, but this implementation uses -1.
*/
template<
    typename entry_t,
    std::size_t data_size,
    std::size_t index_size,
    typename index_t = typename boost::uint_value_t<data_size>::least
> struct csr_matrix {
    using subscript_t = decltype(entry_t::i);
    using value_t = decltype(entry_t::value);

    static_assert(data_size <= std::numeric_limits<index_t>::max(), "Too much data to index with this index type.");
    static_assert(std::is_unsigned<index_t>::value, "Only unsigned index types are supported.");
    static_assert(index_size <= std::numeric_limits<subscript_t>::max(), "Too many rows to subscript with this subscript type.");
    static_assert(std::is_unsigned<subscript_t>::value, "Only unsigned subscripts are supported.");

    std::array<value_t, data_size> data; // indexed with index_t
    const value_t fill_value; // field order to allow packing fill_value with data, in case subscript_t has different alignment

    std::array<index_t, index_size+1> row_pos; // indexed with subscript_t
    std::array<index_t, index_size> row_indent;
    /*
    - row_pos[i] is the storage location of the start of the i-th row.
      - making data[row_pos[i]] the first non-empty element of the i-th row.
    - row_indent[i] is the j-subscript of that first non-empty element in the i-th row.
      - That is, M[i, row_indent[i]] = data[row_pos[i]].
    - The size of a row i is implicit from row_pos[i+1] - row_pos[i]
      - Therefore an empty row is represented by having row_pos[i] == row_pos[i+1].
    - row_pos[index_size] is the overall size of the data array
    */

    template<auto entry_count>
    constexpr csr_matrix(
        const coo_matrix<entry_t, entry_count> & mat
    ) :
        fill_value(mat.fill_value)
    {
        std::size_t data_index = 0;

        subscript_t i = 0;
        subscript_t i_max = mat.i_range().second + 1;
        while (i != i_max) { // looping this way to ensure correct wrap behavior
            row_pos[i] = data_index;
            auto jr = mat.j_range(i);
            if (jr) {
                subscript_t j = jr->first;
                subscript_t j_max = jr->second + 1;
                row_indent[i] = j;
                while (j != j_max) {
                    data[data_index++] = mat.get(i,j++);
                }
            } else {
                row_indent[i] = -1;
            }
            ++i;
        }
        row_pos[index_size] = (subscript_t) data_size;
        // assert(data_index == data_size) // except assert isn't constexpr
    }

    constexpr value_t get(
        const subscript_t i,
        const subscript_t j
    ) const {
        if (i >= index_size) return fill_value;

        auto s = row_indent[i];
        if (j < s) return fill_value;

        auto p = row_pos[i];
        auto q = row_pos[i+1];
        // q - p is the length of the row's data
        // s + (q-p) is the j-index one past the end of that row
        if (j >= s + q - p) return fill_value;

        if (p + j - s >= data_size) return fill_value;
        return data[p + j - s];
    }
};

/* determine the size of the data array needed to store this matrix in csr form */
template<typename entry_t, std::size_t entry_count>
constexpr std::size_t csr_data_size_of(
    const coo_matrix<entry_t, entry_count> & mat
) {
    using subscript_t = decltype(entry_t::i);
    std::size_t total = 0;
    subscript_t i = 0;
    subscript_t i_max = mat.i_range().second + 1;
    while (i != i_max) { // looping this way to ensure correct wrap behavior
        auto jr = mat.j_range(i);
        if (jr) {
            total += jr->second - jr->first + 1;
        } 
        ++i;
    }
    return total;
}

/* determine the size of the index arrays needed to store this matrix in csr form */
template<typename entry_t, std::size_t entry_count>
constexpr std::size_t csr_index_size_of(
    const coo_matrix<entry_t, entry_count> & mat
) {
    return mat.i_range().second + 1;
}

/* use lambda closure trick to pass size info as method param rather than template param */
consteval auto make_csr_matrix_from_gen(
    const auto coo_gen
) {
    using coo_mat_t = typename std::invoke_result<decltype(coo_gen)>::type;
    using entry_t = typename decltype(coo_mat_t::data)::value_type;

    constexpr auto coo_mat = coo_gen();
    constexpr auto data_size = csr_data_size_of(coo_mat);
    constexpr auto index_size = csr_index_size_of(coo_mat);
    constexpr auto csr_mat = csr_matrix<entry_t, data_size, index_size>(coo_mat);
    return csr_mat;
}

/* use lambda closure trick to pass size info and fill value as method params rather than template params */
consteval auto make_csr_matrix_from_gen(
    const auto arr_gen,
    const auto fill_gen
) {
    using array_t = typename std::invoke_result<decltype(arr_gen)>::type;
    using entry_t = typename array_t::value_type;
    using value_t = decltype(entry_t::value);
    using fill_t = typename std::invoke_result<decltype(fill_gen)>::type;
    static_assert(std::is_convertible<fill_t, value_t>::value);

    constexpr auto coo_data = arr_gen();
    constexpr auto entry_count = std::size(coo_data);
    constexpr auto fill_value = fill_gen();
    constexpr auto coo_mat = coo_matrix<entry_t, entry_count>(coo_data, fill_value);
    constexpr auto data_size = csr_data_size_of(coo_mat);
    constexpr auto index_size = csr_index_size_of(coo_mat);
    constexpr auto csr_mat = csr_matrix<entry_t, data_size, index_size>(coo_mat);
    return csr_mat;
}


#define GEN(VAL) []{return VAL;}
// variadic so you get marginally better compiler errors when there's the wrong number of arguments
// TODO: variadic GEN argument wrapping
#define make_csr_matrix(ARR, FILL, ...) make_csr_matrix_from_gen(GEN(ARR), GEN(FILL) __VA_OPT__(,) __VA_ARGS__)
#define make_csr_matrix_from_coo(COO, ...) make_csr_matrix_from_gen(GEN(COO) __VA_OPT__(,) __VA_ARGS__)

} // sparse