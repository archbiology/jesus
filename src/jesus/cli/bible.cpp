#include <algorithm> // for std::unique
#include <stdexcept>
#include <sstream>
#include <cstdint>

#include "bible.hpp"
#include "../understanding/scripture/book_aliases.hpp"
#include <bible/compressed_bible.hpp>

namespace BibleCLI
{
    uint32_t getVerseId(uint16_t book, uint16_t chapter, uint16_t verse)
    {
        uint32_t bookStart = BOOK_OFFSETS[book];
        uint32_t chapterIndex = BOOK_CHAPTER_INDEX[book] + (chapter - 1);
        uint32_t chapterStart = CHAPTER_OFFSETS[chapterIndex];

        return bookStart + chapterStart + (verse - 1);
    }

    bool chapterExists(uint16_t book, uint16_t chapter)
    {
        return chapter >= 1 && chapter <= CHAPTER_COUNT[book];
    }

    bool verseExists(uint16_t book, uint16_t chapter, uint16_t verse)
    {
        if (chapter < 1 || chapter > CHAPTER_COUNT[book])
            return false;

        if (verse < 1)
            return false;

        uint32_t startId = getVerseId(book, chapter, 1);

        // compute endId safely
        uint32_t endId;

        if (chapter < CHAPTER_COUNT[book])
        {
            endId = getVerseId(book, chapter + 1, 1);
        }
        else
        {
            // next book
            if (book + 1 < sizeof(BOOK_OFFSETS) / sizeof(uint32_t))
                endId = BOOK_OFFSETS[book + 1];
            else
                endId = VERSE_COUNT;
        }

        uint32_t verseCount = endId - startId;

        return verse <= verseCount;
    }

    std::string getWordFromDictionary(uint16_t id)
    {
        uint32_t start = DICTIONARY_OFFSETS[id];

        std::string word;
        for (uint32_t i = start; DICTIONARY_DATA[i] != '\0'; i++)
            word += DICTIONARY_DATA[i];

        return word;
    }

    uint16_t readBibleToken(const uint8_t *data, uint32_t index, uint8_t bits)
    {
        uint64_t bitPos = (uint64_t)index * bits;
        uint64_t bytePos = bitPos / 8;
        uint8_t offset = bitPos % 8;

        uint64_t chunk = 0;

        // read up to 8 bytes safely
        for (int i = 0; i < 8; i++)
        {
            chunk |= ((uint64_t)data[bytePos + i]) << (8 * i);
        }

        chunk >>= offset;

        uint64_t mask = (1ULL << bits) - 1;

        return chunk & mask;
    }

    std::string getVerseById(uint32_t verseID)
    {
        uint32_t total_tokens = TOTAL_TOKENS;

        uint32_t start = VERSE_OFFSETS[verseID];

        uint32_t end =
            (verseID + 1 < VERSE_COUNT)
                ? VERSE_OFFSETS[verseID + 1]
                : total_tokens;

        std::string result;

        for (uint32_t i = start; i < end; i++)
        {
            uint16_t token = readBibleToken(TOKEN_STREAM, i, TOKEN_BITS);
            std::string word = getWordFromDictionary(token);
            result += word;
        }

        // 🔥 remove leading space (only at verse start)
        if (!result.empty() && result[0] == ' ')
            result.erase(0, 1);

        return result;
    }
    std::string normalizeBookToken(std::string s)
    {
        // lowercase
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);

        // replace separators with space
        for (char &c : s)
        {
            if (c == '-' || c == '_' || c == '.')
                c = ' ';
        }

        // collapse spaces
        s.erase(std::unique(s.begin(), s.end(),
                            [](char a, char b)
                            { return a == ' ' && b == ' '; }),
                s.end());

        return s;
    }

    bool parseBook(
        const std::vector<std::string> &tokens,
        HolyBible::BookId &outId,
        size_t &consumed)
    {
        for (int len = std::min(3, (int)tokens.size()); len >= 1; --len)
        {
            std::string combined;
            for (int i = 0; i < len; ++i)
            {
                if (i)
                    combined += " ";
                combined += tokens[i];
            }

            auto key = normalizeBookToken(combined);
            auto it = HolyBible::BOOK_ALIASES.find(key);
            if (it != HolyBible::BOOK_ALIASES.end())
            {
                outId = it->second;
                consumed = len;
                return true;
            }
        }
        return false;
    }

    std::vector<std::string> splitTokens(const std::string &input)
    {
        std::istringstream iss(input);
        std::vector<std::string> tokens;
        std::string t;
        while (iss >> t)
            tokens.push_back(t);
        return tokens;
    }

    ChapterReference parseChapterToken(const std::string &token)
    {
        ChapterReference ref{};

        auto colon = token.find(':');
        if (colon == std::string::npos)
        {
            // chapter or chapter range
            auto dash = token.find('-');
            if (dash == std::string::npos)
            {
                ref.chapter = std::stoi(token);
                return ref;
            }
            else
            {
                throw std::runtime_error("Chapter ranges handled outside");
            }
        }

        ref.chapter = std::stoi(token.substr(0, colon));
        std::string verses = token.substr(colon + 1);

        size_t pos = 0;
        while (pos < verses.size())
        {
            size_t next = verses.find(',', pos);
            auto part = verses.substr(pos, next - pos);

            auto dash = part.find('-');
            if (dash == std::string::npos)
            {
                int v = std::stoi(part);
                ref.verses.push_back({v, v});
            }
            else
            {
                int a = std::stoi(part.substr(0, dash));
                int b = std::stoi(part.substr(dash + 1));
                ref.verses.push_back({a, b});
            }

            if (next == std::string::npos)
                break;
            pos = next + 1;
        }

        return ref;
    }

    ScriptureReference parseScripture(const std::string &input)
    {
        auto tokens = splitTokens(input);

        HolyBible::BookId bookId;
        size_t consumed = 0;

        if (!parseBook(tokens, bookId, consumed))
        {
            throw std::runtime_error("Unknown Bible book");
        }

        ScriptureReference ref{};
        ref.bookId = bookId;

        for (size_t i = consumed; i < tokens.size(); ++i)
        {
            ref.chapters.push_back(parseChapterToken(tokens[i]));
        }

        return ref;
    }

    std::string readScripture(const ScriptureReference &ref)
    {
        std::ostringstream out;

        uint16_t book = (uint16_t)ref.bookId;

        for (const auto &chRef : ref.chapters)
        {
            int chapter = chRef.chapter;

            if (!chapterExists(book, chapter))
                continue; // skip invalid chapter entirely

            // If no verses specified → whole chapter
            if (chRef.verses.empty())
            {
                // We must iterate until next chapter
                uint32_t startId = getVerseId(book, chapter, 1);

                // find how many verses exist
                uint32_t nextChapterStart;

                if (chapter < CHAPTER_COUNT[book])
                {
                    nextChapterStart = getVerseId(book, chapter + 1, 1);
                }
                else
                {
                    // next book
                    if (book + 1 < sizeof(BOOK_OFFSETS) / sizeof(uint32_t))
                        nextChapterStart = BOOK_OFFSETS[book + 1];
                    else
                        nextChapterStart = VERSE_COUNT;
                }

                for (uint32_t id = startId; id < nextChapterStart; ++id)
                {
                    out << getVerseById(id) << "\n";
                }
            }
            else
            {
                for (const auto &vr : chRef.verses)
                {
                    for (int v = vr.start; v <= vr.end; ++v)
                    {
                        if (verseExists(book, chapter, v))
                        {
                            uint32_t id = getVerseId(book, chapter, v);
                            out << getVerseById(id) << "\n";
                        }
                    }
                }
            }
        }

        return out.str();
    }

};
