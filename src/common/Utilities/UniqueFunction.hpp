#ifndef UNIQUE_FUNCTION_H
#define UNIQUE_FUNCTION_H

#include <functional>
#include <type_traits>

namespace Detail
{
    template<typename Callable, typename IsCopyable = void>
    struct Wrapper;

    template<typename Callable>
    struct Wrapper<Callable, std::enable_if_t< std::is_copy_constructible_v<Callable> > >
    {
        Callable callable;

        template<typename... Args>
        auto operator()( Args &&... args )
        {
            return callable( std::forward<Args>( args )... );
        }
    };

    template<typename Callable>
    struct Wrapper<Callable, std::enable_if_t< !std::is_copy_constructible_v<Callable> && std::is_move_constructible_v<Callable>> >
    {
        Callable callable;

        Wrapper( Callable && callable )
            : callable( std::forward<Callable>( callable ) )
        {
        }

        Wrapper( Wrapper && ) = default;
        Wrapper & operator=( Wrapper && ) = default;

        Wrapper( const Wrapper & rhs )
            : callable( const_cast< Callable && >( rhs.callable ) )
        {
            throw 0;
        }

        Wrapper & operator=( Wrapper & )
        {
            throw 0;
        }

        template<typename... Args>
        auto operator()( Args &&... args )
        {
            return callable( std::forward<Args>( args )... );
        }
    };
}

template<typename T>
class UniqueFunction : public std::function<T>
{
    using Base = std::function<T>;

public:
    UniqueFunction() noexcept = default;
    UniqueFunction( std::nullptr_t ) noexcept
        : Base( nullptr )
    {
    }

    template<typename Callable>
    UniqueFunction( Callable && f )
        : Base( Detail::Wrapper<Callable>{ std::forward<Callable>( f ) } )
    {
    }

    UniqueFunction( UniqueFunction && ) = default;
    UniqueFunction & operator=( UniqueFunction && ) = default;

    UniqueFunction & operator=( std::nullptr_t )
    {
        Base::operator=( nullptr );
        return *this;
    }

    template<typename Callable>
    UniqueFunction & operator=( Callable && f )
    {
        Base::operator=( Detail::Wrapper<Callable>{ std::forward<Callable>( f ) } );
        return *this;
    }

    using Base::operator();
};
#endif
