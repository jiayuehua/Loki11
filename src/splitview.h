#pragma once
// https://godbolt.org/z/7GjPzEGo1
#include <ranges>
#include <algorithm>
#include <string>
#include <iostream>
#include <span>

using namespace std::ranges;

template <contiguous_range V, forward_range Pattern>
    requires view<V> && view<Pattern> &&
    std::indirectly_comparable<iterator_t<V>,
                               iterator_t<Pattern>,
                               equal_to>
class contig_split_view
    : public view_interface<contig_split_view<V, Pattern>>
{
public:
    contig_split_view() = default;
    contig_split_view(V base, Pattern pattern)
        : base_(base)
        , pattern_(pattern)
    { }

    template <contiguous_range R>
	    requires std::constructible_from<V, views::all_t<R>>
	        && std::constructible_from<Pattern, single_view<range_value_t<R>>>
	contig_split_view(R&& r, range_value_t<R> elem)
	    : base_(std::views::all(std::forward<R>(r)))
	    , pattern_(std::move(elem))
	{ }

    struct sentinel;
    struct as_sentinel_t { };

    class iterator {
    private:
        using underlying = std::remove_reference_t<
            range_reference_t<V>>;
        friend sentinel;

        contig_split_view* parent = nullptr;
        iterator_t<V> cur = iterator_t<V>();
        iterator_t<V> next = iterator_t<V>();

    public:
        iterator() = default;
        iterator(contig_split_view* p)
            : parent(p)
            , cur(std::ranges::begin(p->base_))
            , next(lookup_next())
        { }
        
        iterator(as_sentinel_t, contig_split_view* p)
            : parent(p)
            , cur(std::ranges::end(p->base_))
            , next()
        { }

        using iterator_category = std::forward_iterator_tag;

        struct reference : std::span<underlying> {
            using std::span<underlying>::span;

            operator std::string_view() const
                requires std::same_as<range_value_t<V>, char>
            {
                return {this->data(), this->size()};
            }
        };

        using value_type = reference;
        using difference_type = std::ptrdiff_t;

        bool operator==(iterator const& rhs) const {
            return cur == rhs.cur;
        }

        auto lookup_next() const -> iterator_t<V> {
            return std::ranges::search(
                subrange(cur, std::ranges::end(parent->base_)),
                parent->pattern_
                ).begin();
        }

        auto operator++() -> iterator& {
            cur = next;
            if (cur != std::ranges::end(parent->base_)) {
                cur += distance(parent->pattern_);
                next = lookup_next();
            }
            return *this;
        }
        auto operator++(int) -> iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        auto operator*() const -> reference {
            return {cur, next};
        }
    };

    struct sentinel {
        bool operator==(iterator const& rhs) const {
            return rhs.cur == sentinel;
        }

        sentinel_t<V> sentinel;
    };


    auto begin() -> iterator {
        if (not cached_begin_) {
            cached_begin_.emplace(this);
        }
        return *cached_begin_;
    }
    auto end() -> sentinel {
        return {std::ranges::end(base_)};
    }

    auto end() -> iterator requires common_range<V> {
        return {as_sentinel_t(), this};
    }

private:
    V base_ = V();
    Pattern pattern_ = Pattern();
    std::optional<iterator> cached_begin_;
};

namespace std::ranges {
    template<contiguous_range V, forward_range Pattern>
    requires view<V> && view<Pattern>
      && indirectly_comparable<iterator_t<V>, iterator_t<Pattern>, equal_to>
    class split_view<V, Pattern> : public contig_split_view<V, Pattern>
    {
        using contig_split_view<V, Pattern>::contig_split_view;
    };
}

struct zstring_sentinel {
    bool operator==(char const* p) const {
        return *p == '\0';
    }
};

struct zstring : view_interface<zstring> {
    char const* p = nullptr;
    zstring() = default;
    zstring(char const* p) : p(p) { }
    auto begin() const { return p; }
    auto end() const { return zstring_sentinel{}; }
};

//int main() {
//    using namespace std::literals;
//    using V = contig_split_view<std::string_view, std::string_view>;
//    using It = V::iterator;
//    using S = V::sentinel;
//    static_assert(std::sentinel_for<S, It>);
//    static_assert(forward_range<V>);
//    static_assert(common_range<V>);
//    static_assert(view<V>);
//    static_assert(std::indirectly_readable<It>);
//    static_assert(std::forward_iterator<It>);
//
//    V v = contig_split_view("127..0..0..1"sv, ".."sv);
//    std::ranges::begin(v);
//
//    for (std::string_view sv : "127..0..0..1"sv | std::views::split(".."sv))
//    {
//        std::cout << sv << '\n';
//    }
//
//    auto ip = "127.0.0.1"s;
//    auto parts = ip | std::views::split('.');
//    auto as_vec = std::vector<std::string>(parts.begin(), parts.end());
//    std::cout << as_vec.size() << '\n';
//
//    char const* words = "A quick brown fox";
//    static_assert(view<zstring>);
//    for (std::string_view sv : zstring{words} | std::views::split(' ')) {
//        std::cout << sv << '\n';
//    }
//    return 0;
//}
