/*
 * Copyright (c) 2015-2021 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_DETAIL_LOW_COMPARISONS_SORT8_H_
#define CPPSORT_DETAIL_LOW_COMPARISONS_SORT8_H_

namespace cppsort
{
namespace detail
{
    template<>
    struct low_comparisons_sorter_impl<8u>
    {
        template<
            typename RandomAccessIterator,
            typename Compare = std::less<>,
            typename Projection = utility::identity,
            typename = detail::enable_if_t<is_projection_iterator_v<
                Projection, RandomAccessIterator, Compare
            >>
        >
        auto operator()(RandomAccessIterator first, RandomAccessIterator,
                        Compare compare={}, Projection projection={}) const
            -> void
        {
            low_comparisons_sorter<7u>{}(first+1u, first+8u, compare, projection);
            front_insert<8u>(std::move(first), std::move(compare), std::move(projection));
        }
    };
}}

#endif // CPPSORT_DETAIL_LOW_COMPARISONS_SORT8_H_
