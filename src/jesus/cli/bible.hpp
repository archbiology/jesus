#pragma once

#include <vector>
#include <string>

#include <bible/book.hpp>
#include "../understanding/scripture/book_id.hpp"
#include "../understanding/scripture/language.hpp"

namespace BibleCLI
{

    struct VerseRange
    {
        int start;
        int end; // inclusive
    };

    struct ChapterReference
    {
        int chapter;
        std::vector<VerseRange> verses; // empty = whole chapter
    };

    struct ScriptureReference
    {
        const HolyBible::Book *book;
        std::vector<ChapterReference> chapters;
    };

    std::string normalizeBookToken(std::string s);

    bool parseBook(const std::vector<std::string> &tokens, HolyBible::BookId &outId, size_t &consumed);

    ChapterReference parseChapterToken(const std::string &token);

    std::vector<std::string> splitTokens(const std::string &input);

    ScriptureReference parseScripture(const std::string &input, HolyBible::Language language);

    std::string readScripture(const ScriptureReference &ref);

} // namespace BibleCLI