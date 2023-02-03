#include <cstddef>
#include <array>
#include <algorithm>
#include <tuple>
#include <compare>
#include <optional>
#include <concepts>
#include <type_traits>

namespace sparse {

template<typename value_t, typename subscript_t = std::size_t>
struct coo_entry {
    subscript_t i;
    subscript_t j;
    value_t value;

    constexpr std::strong_ordering operator<=>(
        const coo_entry& that
    ) const {
        return std::tie(i, j) <=> std::tie(that.i, that.j);
    }
    constexpr bool operator==(
        const coo_entry& that
    ) const {
        return std::tie(i, j) == std::tie(that.i, that.j);
    }
};

constexpr auto sorted(
    const auto& arr
) {
    auto result = arr;
    std::sort(begin(result), end(result));
    return result;
}

template<typename entry_t, std::size_t entry_count>
struct coo_matrix {
    using subscript_t = decltype(entry_t::i);
    using value_t = decltype(entry_t::value);

    std::array<entry_t, entry_count> data;
    value_t fill_value;

    constexpr coo_matrix(
        const std::array<entry_t, entry_count> & data,
        value_t fill_value
    ) :
        data(sorted(data)),
        fill_value(fill_value)
    {}

    /* Retrieve an item from the matrix */
    constexpr value_t get(
        const subscript_t i,
        const subscript_t j
    ) const {
        entry_t key = {i,j,fill_value};
        auto it = std::lower_bound(begin(data), end(data), key);
        if (it >= end(data)) return fill_value;
        if (*it == key) return it->value;
        return fill_value;
    }

    /* Returns the closed interval [i_min, i_max] that contains all extant i values */
    constexpr std::pair<subscript_t,subscript_t> i_range() const {
        return {begin(data)->i, std::prev(end(data))->i}; // already sorted by (i,j) so first element will have lowest i, last will have highest j
    }

    /* Returns the closed interval [j_min, j_max] that contains all extant j values in all rows.*/
    constexpr std::pair<subscript_t,subscript_t> j_range() const {
        auto its = std::minmax_element(begin(data), end(data), [](entry_t a, entry_t b){ return a.j < b.j;});
        // minmax_element is linear time... theoretically this could be done asymtotically faster with a binary search for begin/end of each row
        // but I'd wager it's not actually faster enough in practice to be worth implementing
        return {its.first->j, its.second->j};
    }

    /* Returns the closed interval [j_min, j_max] that contains all extant j values only in the i-th row,
        or nothing if there are no entries in that row. */
    constexpr std::optional<std::pair<subscript_t,subscript_t>> j_range(
        const subscript_t i
    ) const {
        entry_t key = {i,0,fill_value};
        auto its = std::equal_range(begin(data), end(data), key, [](entry_t a, entry_t b){ return a.i < b.i;});
        if (its.first == its.second) return {}; // row does not exist
        return {{its.first->j, std::prev(its.second)->j}};
    }
};

/* use lambda closure trick to make size a constexpr method parameter rather than template parameter */
consteval auto make_coo_matrix_from_gen(
    const auto gen,
    const auto fill_value
) {
    using array_t = typename std::invoke_result<decltype(gen)>::type;
    using entry_t = typename array_t::value_type;
    using value_t = decltype(entry_t::value);
    using fill_t = decltype(fill_value);
    static_assert(std::is_convertible<fill_t, value_t>::value);

    constexpr std::array coo_data = gen();
    constexpr auto entry_count = std::size(coo_data);
    return coo_matrix<entry_t, entry_count>(coo_data, fill_value);
}

/* convert an array of unsorted coo_entry into a sorted coo_matrix */
#define make_coo_matrix(ARR, ...) make_coo_matrix_from_gen([]{return ARR;} __VA_OPT__(,) __VA_ARGS__)
// variadic so you get better compiler errors when there's the wrong number of arguments

} // namespace