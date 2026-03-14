#pragma once
#include <bible/book.hpp>
#include "book_id.hpp"
#include "language.hpp"

namespace HolyBible
{
    class BookRegistry
    {
    public:
        static const HolyBible::Book *resolve(Language lang, HolyBible::BookId id)
        {
            return books[(int)lang][(int)id];
        }

    private:
        static const HolyBible::Book *books[2][66];
    };
}
