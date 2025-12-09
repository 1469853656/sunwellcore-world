

#ifndef TRINITY_CONTAINERS_H
#define TRINITY_CONTAINERS_H

#include "Define.h"
#include "Utilities/Util.h"

#include <list>
#include <sparsehash/dense_hash_map>

//! Because circular includes are bad
extern uint32 urand(uint32 min, uint32 max);

namespace Trinity {
template<typename K, typename V>
using LookupMap = google::dense_hash_map<K, V>;

namespace Containers {
    // replace with std::size in C++17
    template<class C>
    constexpr inline std::size_t Size(C const& container)
    {
        return container.size();
    }

    template<class T, std::size_t size>
    constexpr inline std::size_t Size(T const (&)[size]) noexcept
    {
        return size;
    }

    // resizes <container> to have at most <requestedSize> elements
    // if it has more than <requestedSize> elements, the elements to keep are selected randomly
    template<class C>
    void RandomResize(C& container, std::size_t requestedSize)
    {
        static_assert(std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<typename C::iterator>::iterator_category>::value, "Invalid container passed to Trinity::Containers::RandomResize");
        if (Size(container) <= requestedSize)
            return;
        auto keepIt = std::begin(container), curIt = std::begin(container);
        uint32 elementsToKeep = requestedSize, elementsToProcess = Size(container);
        while (elementsToProcess)
        {
            // this element has chance (elementsToKeep / elementsToProcess) of being kept
            if (urand(1, elementsToProcess) <= elementsToKeep)
            {
                *keepIt = std::move(*curIt);
                ++keepIt;
                --elementsToKeep;
            }
            ++curIt;
            --elementsToProcess;
        }
        container.erase(keepIt, std::end(container));
    }

    template<class C, class Predicate>
    void RandomResize(C& container, Predicate&& predicate, std::size_t requestedSize)
    {
        //! First use predicate filter
        C containerCopy;
        std::copy_if(std::begin(container), std::end(container), std::inserter(containerCopy, std::end(containerCopy)), predicate);

        if (requestedSize)
            RandomResize(containerCopy, requestedSize);

        container = std::move(containerCopy);
    }

    /*
        * Select a random element from a container.
        *
        * Note: container cannot be empty
        */
    template<class C>
    inline auto SelectRandomContainerElement(C const& container) -> typename std::add_const<decltype(*std::begin(container))>::type&
    {
        auto it = std::begin(container);
        std::advance(it, urand(0, uint32(Size(container)) - 1));
        return *it;
    }

    template<typename Container, typename Predicate>
    std::enable_if_t<!std::is_move_assignable_v<decltype(*std::declval<Container>().begin())>, void> EraseIf(Container& c, Predicate p)
    {
        for (auto it = c.begin(); it != c.end();)
        {
            if (p(*it))
                it = c.erase(it);
            else
                ++it;
        }
    }

    template<typename Container, typename Predicate>
    std::enable_if_t<std::is_move_assignable_v<decltype(*std::declval<Container>().begin())>, void> EraseIf(Container& c, Predicate p)
    {
        auto wpos = c.begin();
        for (auto rpos = c.begin(), end = c.end(); rpos != end; ++rpos)
        {
            if (!p(*rpos))
            {
                if (rpos != wpos)
                    std::swap(*rpos, *wpos);
                ++wpos;
            }
        }

        c.erase(wpos, c.end());
    }

    // Checks if container has chosen element
    template<typename Container>
    bool Contains(Container& container, typename Container::value_type const& value)
    {
        if (container.empty())
            return false;

        return std::find(std::begin(container), std::end(container), value) != std::end(container);
    }

    /**
         * @fn void Trinity::Containers::RandomShuffle(C& container)
         *
         * @brief Reorder the elements of the container randomly.
         *
         * @param container Container to reorder
         */
    template<class C>
    void RandomShuffle(C& container)
    {
        std::shuffle(container.begin(), container.end(), GetGenerator());
    }

    // Get element from container with the most occurences
    template<class Iterator, class T = typename std::iterator_traits<Iterator>::value_type>
    T GetMostCommonElement(Iterator begin, Iterator end)
    {
        std::map<T, int> counts;
        for (Iterator it = begin; it != end; ++it)
        {
            if (counts.find(*it) != counts.end())
                ++counts[*it];
            else
                counts[*it] = 1;
        }
        return std::max_element(counts.begin(), counts.end(), [](const std::pair<T, int>& pair1, const std::pair<T, int>& pair2) { return pair1.second < pair2.second; })->first;
    }

    template<class iterator>
    class IteratorPair
    {
    public:
        IteratorPair()
            : _iterators()
        {
        }
        IteratorPair(iterator first, iterator second)
            : _iterators(first, second)
        {
        }
        IteratorPair(std::pair<iterator, iterator> iterators)
            : _iterators(iterators)
        {
        }

        iterator begin() const
        {
            return _iterators.first;
        }
        iterator end() const
        {
            return _iterators.second;
        }

    private:
        std::pair<iterator, iterator> _iterators;
    };

    template<class M>
    inline auto MapEqualRange(M& map, typename M::key_type const& key) -> IteratorPair<decltype(map.begin())>
    {
        return { map.equal_range(key) };
    }
}
//! namespace Containers
}
//! namespace Trinity

#endif //! #ifdef TRINITY_CONTAINERS_H
