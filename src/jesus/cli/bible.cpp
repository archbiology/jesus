#include <algorithm> // for std::unique
#include <stdexcept>
#include <sstream>

#include "bible.hpp"
#include "../understanding/scripture/book_aliases.hpp"
#include "../understanding/scripture/book_registry.hpp"

namespace BibleCLI
{
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

    ScriptureReference parseScripture(const std::string &input, HolyBible::Language language)
    {
        auto tokens = splitTokens(input);

        HolyBible::BookId bookId;
        size_t consumed = 0;

        if (!parseBook(tokens, bookId, consumed))
        {
            throw std::runtime_error("Unknown Bible book");
        }

        ScriptureReference ref{};
        // ref.book = resolveBook(bookId);
        ref.book = HolyBible::BookRegistry::resolve(language, bookId);

        if (!ref.book)
            throw std::runtime_error("Book not available");

        for (size_t i = consumed; i < tokens.size(); ++i)
        {
            ref.chapters.push_back(parseChapterToken(tokens[i]));
        }

        return ref;
    }

    std::string readScripture(const ScriptureReference &ref)
    {
        if (!ref.book)
            return "";

        const auto &book = *ref.book;
        std::ostringstream out;

        for (const auto &chRef : ref.chapters)
        {
            auto chIt = book.chapters.find(chRef.chapter);
            if (chIt == book.chapters.end())
                continue;

            const auto &verses = chIt->second;

            if (chRef.verses.empty())
            {
                for (const auto &[v, text] : verses)
                {
                    out << book.name << " "
                        << chRef.chapter << ":" << v
                        << " " << text << "\n";
                }
            }
            else
            {
                for (const auto &vr : chRef.verses)
                {
                    for (int v = vr.start; v <= vr.end; ++v)
                    {
                        auto it = verses.find(v);
                        if (it != verses.end())
                        {
                            out << book.name << " "
                                << chRef.chapter << ":" << v
                                << " " << it->second << "\n";
                        }
                    }
                }
            }
        }

        return out.str();
    }

};