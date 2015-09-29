# Sorter adapters

Sorter adapters are the main reason for using sorter function objects instead
of regular functions. A *sorter adapter* is a class template that takes another
`Sorter` template parameter and alters its behavior. The resulting class can be
used as a regular sorter, and be adapted in turn. It is possible to include all
the available adapters at once with the following directive:

```cpp
#include <cpp-sort/adapters.h>
```

In this documentation, we will call *adapted sorters* the sorters passed to the
adapters and *resulting sorter* the sorter class that results from the adaption
of a sorter by an adapter.

## Available sorter adapters

The following sorter adapters are available in the library:

### `counting_adapter`

```cpp
#include <cpp-sort/adapters/counting_adapter.h>
```

Unlike regular sorters, `counting_adapter::operator()` does not return `void` but
the number of comparisons that have been needed to sort the iterable. It will
adapt the comparison functor so that it can count the number of comparisons
made by any other sorter with a reasonable implementation. The actual number of
comparisons needed to sort an interable can be used as a heuristic in hybrid sorts
and may constitute an interesting information nevertheless.

The actual counter type can be configured with the template parameter `CountType`,
which defaults to `std::size_t` if not specified.

```cpp
template<
    typename ComparisonSorter,
    typename CountType = std::size_t
>
struct counting_adapter;
```

The iterator category and the stability of the *resulting sorter* are those of the
*adapted sorter*. Note that this adapter only works with sorters that satisfy the
`ComparisonSorter` concept since it needs to adapt a comparison function.

### `hybrid_adapter`

```cpp
#include <cpp-sort/adapters/hybrid_adapter.h>
```

The goal of this sorter adapter is to aggregate several sorters into one unique
sorter. The new sorter will call the appropriate sorting algorithm based on the
iterator category of the iterable to sort. If several of the aggregated sorters
have the same iterator category, the first to appear in the template parameter
list will be the chosen one, unless some SFINAE condition prevents it from being
used. As long as the iterator categories are different, the order of the sorters
in the parameter pack does not matter.

For example, the following sorter will call a pattern-defeating quicksort to sort
a random-access iterable, an insertion sort to sort a bidirectional iterable and
a bubble sort to sort a forward iterable:

```cpp
using general_purpose_sorter = hybrid_adapter<
    bubble_sorter,
    insertion_sorter,
    pdq_sorter
>;
```

This adapter uses `cppsort::iterator_category` to check the iterator category of
the sorters to aggregate. Therefore, if you write a sorter, you will need to
specialize `cppsort::sorter_traits` if you want it to be usable with this adapter.

The stability of the *resulting sorter* is `true` if and only if the stability
of every *adapter sorter* is `true`. The iterator category of the *resulting
sorter* is the most permissive iterator category among the the *adapted sorters*.

### `self_sort_adapter`

```cpp
#include <cpp-sort/adapters/self_sort_adapter.h>
```

This adapter takes a sorter and, if the object to be sorted has a `sort` method,
it is used to sort the object. Otherwise, the *adapted sorter* is used instead to
sort the iterable.

This sorter adapter allows to support out-of-the-box sorting for `std::list` and
`std::forward_list` as well as other user-defined classes that implement a `sort`
method.

```cpp
template<typename Sorter>
struct self_sort_adapter;
```

Since it is impossible to guarantee the stability of the `sort` method of a
given iterable, the stability of the *resulting sorter* is always `false`.
The iterator category of the *resulting sorter* is that of the *adapted sorter*.

### `small_array_adapter`

```cpp
#include <cpp-sort/adapters/small_array_adapter.h>
```

This sorter adapter comes into two flavors:

```cpp
template<typename Sorter>
struct small_array_adapter<Sorter>;
```

This specialization takes a sorter and uses it to sort the collections it
receives. However, if the collection is an `std::array` or a fixed-size C
array, it replaces the `Sorter` sort by special algorithms designed to sort
small arrays of fixed size.

The specific sorting algorithms used by `small_array_adapter` mostly correspond
to [sorting networks](https://en.wikipedia.org/wiki/Sorting_network) of a given
size. There are specialized algorithms for the sizes 0 to 32. The following table
documents the number of comparisons and the number of swaps made by every
algorithm:

Size | comparisons | swaps
---- | ----------- | -----
0 | 0 | 0
1 | 0 | 0
2 | 1 | ≤ 1
3 | 2-3 | < 2
4 | 5 | ≤ 5
5 | 9 | ≤ 9
6 | 12 | ≤ 12
7 | 16 | ≤ 16
8 | 19 | ≤ 19
9 | 25 | ≤ 25
10 | 29 | ≤ 29
11 | 35 | ≤ 35
12 | 39 | ≤ 39
13 | 45 | ≤ 45
14 | 51 | ≤ 51
15 | 56 | ≤ 56
16 | 60 | ≤ 60
17 | 74 | ≤ 74
18 | 82 | ≤ 82
19 | 91 | ≤ 91
20 | 97 | ≤ 97
21 | 107 | ≤ 107
22 | 114 | ≤ 114
23 | 122 | ≤ 122
24 | 127 | ≤ 127
25 | 138 | ≤ 138
26 | 146 | ≤ 146
27 | 155 | ≤ 155
28 | 161 | ≤ 161
29 | 171 | ≤ 171
30 | 178 | ≤ 178
31 | 186 | ≤ 186
32 | 191 | ≤ 191

The code for most of these algorithms has been generated using the `SWAP` macros
generated by http://pages.ripco.net/~jgamble/nw.html with the "Best" algorithm (as
described on the site) for inputs inferior or equal to 16, and Batcher's Merge-Exchange
algorithm otherwise.

There is another specialization of `small_array_adapter` which takes a `Sorter` and
an `std::index_sequence`:

```cpp
template<
    typename Sorter,
    std::size_t... Indices
>
struct small_array_adapter<Sorter, std::index_sequence<Indices...>>;
```

This version of the sorter adapter only uses the specialized sorting algorithms
for fixed-size arrays if the size of the array to sort is contained in `Indices`,
and falls back to `Sorter` in every other case. The main advantage is that it allows
to use the class [`std::make_index_sequence`](http://en.cppreference.com/w/cpp/utility/integer_sequence)
to generate the indices and pick the specialized algorithms for the smallest values
of N, which tend to be the most optimized ones.