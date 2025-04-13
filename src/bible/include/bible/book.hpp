#ifndef HOLYBIBLE_BOOK_HPP
#define HOLYBIBLE_BOOK_HPP

#include <string>
#include <map>

namespace HolyBible
{

    using Verses = std::map<int, std::string>;
    using Chapters = std::map<int, Verses>;

    class Book
    {
    public:
        const int number;
        const std::string name;
        const Chapters chapters;

        explicit Book(int number, const std::string &name, const Chapters &chapters)
            : number(number), name(name), chapters(chapters) {}
    };

} // namespace word

#endif // WORD_BOOK_HPP
