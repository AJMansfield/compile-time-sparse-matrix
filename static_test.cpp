#include "sparse/csr_matrix.h"

consteval void test_normal_matrix() {
    using entry_t = sparse::coo_entry<char, unsigned char>;
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
}

consteval void test_coo_factory() {
    using entry_t = sparse::coo_entry<char, unsigned char>;
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
    constexpr auto csr_mat = sparse::make_csr_matrix_from_coo(coo_mat);
}

consteval void test_at_limits_of_uchar() {
    using entry_t = sparse::coo_entry<unsigned char, unsigned char>;

    constexpr auto csr_mat_1 = sparse::make_csr_matrix(std::to_array<entry_t>({{0,255,1}}), -1);
    static_assert(csr_mat_1.get(0,255) == 1);
    static_assert(std::is_same<decltype(csr_mat_1.row_pos)::value_type, unsigned char>::value);

    constexpr auto csr_mat_2 = sparse::make_csr_matrix(std::to_array<entry_t>({{254,0,1}}), -1);
    static_assert(csr_mat_2.get(254,0) == 1);
    static_assert(std::is_same<decltype(csr_mat_2.row_pos)::value_type, unsigned char>::value);

    constexpr auto csr_mat_3 = sparse::make_csr_matrix(std::to_array<entry_t>({{254,255,1}}), -1);
    static_assert(csr_mat_3.get(254,255) == 1);
    static_assert(std::is_same<decltype(csr_mat_3.row_pos)::value_type, unsigned char>::value);
}

consteval void test_too_big_for_uchar() {
    using entry_t = sparse::coo_entry<unsigned char, unsigned int>;
    constexpr auto csr_mat = sparse::make_csr_matrix(std::to_array<entry_t>({{0,0,0xff},{0,260,0xff}}), 0);
    static_assert(std::is_same<decltype(csr_mat.row_pos)::value_type, unsigned short>::value);
}