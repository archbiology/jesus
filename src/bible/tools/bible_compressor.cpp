#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstdint>
#include <cstdint>
#include <memory>

#include "../data/en/__books__.hpp"

using namespace std;

vector<unique_ptr<HolyBible::Book>> loadBooks()
{
    using namespace English;

    vector<unique_ptr<HolyBible::Book>> books;

    books.push_back(make_unique<Genesis>());
    books.push_back(make_unique<Exodus>());
    books.push_back(make_unique<Leviticus>());
    books.push_back(make_unique<Numbers>());
    books.push_back(make_unique<Deuteronomy>());
    books.push_back(make_unique<Joshua>());
    books.push_back(make_unique<Judges>());
    books.push_back(make_unique<Ruth>());
    books.push_back(make_unique<FirstSamuel>());
    books.push_back(make_unique<SecondSamuel>());
    books.push_back(make_unique<FirstKings>());
    books.push_back(make_unique<SecondKings>());
    books.push_back(make_unique<FirstChronicles>());
    books.push_back(make_unique<SecondChronicles>());
    books.push_back(make_unique<Ezra>());
    books.push_back(make_unique<Nehemiah>());
    books.push_back(make_unique<Esther>());
    books.push_back(make_unique<Job>());
    books.push_back(make_unique<Psalms>());
    books.push_back(make_unique<Proverbs>());
    books.push_back(make_unique<Ecclesiastes>());
    books.push_back(make_unique<SongofSolomon>());
    books.push_back(make_unique<Isaiah>());
    books.push_back(make_unique<Jeremiah>());
    books.push_back(make_unique<Lamentations>());
    books.push_back(make_unique<Ezekiel>());
    books.push_back(make_unique<Daniel>());
    books.push_back(make_unique<Hosea>());
    books.push_back(make_unique<Joel>());
    books.push_back(make_unique<Amos>());
    books.push_back(make_unique<Obadiah>());
    books.push_back(make_unique<Jonah>());
    books.push_back(make_unique<Micah>());
    books.push_back(make_unique<Nahum>());
    books.push_back(make_unique<Habakkuk>());
    books.push_back(make_unique<Zephaniah>());
    books.push_back(make_unique<Haggai>());
    books.push_back(make_unique<Zechariah>());
    books.push_back(make_unique<Malachi>());
    books.push_back(make_unique<Matthew>());
    books.push_back(make_unique<Mark>());
    books.push_back(make_unique<Luke>());
    books.push_back(make_unique<John>());
    books.push_back(make_unique<Acts>());
    books.push_back(make_unique<Romans>());
    books.push_back(make_unique<FirstCorinthians>());
    books.push_back(make_unique<SecondCorinthians>());
    books.push_back(make_unique<Galatians>());
    books.push_back(make_unique<Ephesians>());
    books.push_back(make_unique<Philippians>());
    books.push_back(make_unique<Colossians>());
    books.push_back(make_unique<FirstThessalonians>());
    books.push_back(make_unique<SecondThessalonians>());
    books.push_back(make_unique<FirstTimothy>());
    books.push_back(make_unique<SecondTimothy>());
    books.push_back(make_unique<Titus>());
    books.push_back(make_unique<Philemon>());
    books.push_back(make_unique<Hebrews>());
    books.push_back(make_unique<James>());
    books.push_back(make_unique<FirstPeter>());
    books.push_back(make_unique<SecondPeter>());
    books.push_back(make_unique<FirstJohn>());
    books.push_back(make_unique<SecondJohn>());
    books.push_back(make_unique<ThirdJohn>());
    books.push_back(make_unique<Jude>());
    books.push_back(make_unique<Revelation>());

    return books;
}

struct VerseTokens
{
    uint16_t book;
    uint16_t chapter;
    uint16_t verse;
    vector<string> tokens;
};

unordered_map<string, uint16_t> BOOK_NAME_TO_ID;
uint16_t NEXT_BOOK_ID = 0;

bool isWordChar(char c)
{
    return isalpha((unsigned char)c) || c == '\'';
}

vector<string> tokenize(const string &text)
{
    vector<string> tokens;
    string current;

    bool pendingSpace = false;
    bool inQuote = false;

    for (char c : text)
    {
        // ==========================
        // SPACE
        // ==========================
        if (c == ' ')
        {
            if (!current.empty())
            {
                tokens.push_back(current);
                current.clear();
            }

            pendingSpace = true;
            continue;
        }

        // ==========================
        // WORD
        // ==========================
        if (isWordChar(c))
        {
            if (current.empty() && pendingSpace)
            {
                current += ' ';
                pendingSpace = false;
            }

            current += c;
            continue;
        }

        // ==========================
        // FLUSH WORD
        // ==========================
        if (!current.empty())
        {
            tokens.push_back(current);
            current.clear();
        }

        // ==========================
        // QUOTES (STATEFUL)
        // ==========================
        if (c == '"')
        {
            if (!inQuote)
            {
                // opening quote
                if (pendingSpace)
                {
                    tokens.push_back(" \"");
                }
                else
                {
                    tokens.push_back("\"");
                }
                inQuote = true;
            }
            else
            {
                // closing quote → NEVER prepend space
                tokens.push_back("\"");
                inQuote = false;
            }

            pendingSpace = false;
            continue;
        }

        // ==========================
        // OTHER PUNCTUATION
        // ==========================
        tokens.push_back(string(1, c));

        // punctuation that carries space forward
        if (c == ',' || c == '.' || c == ':' || c == ';' ||
            c == '!' || c == '?')
        {
            pendingSpace = true;
        }
        else
        {
            pendingSpace = false;
        }
    }

    if (!current.empty())
        tokens.push_back(current);

    return tokens;
}

string normalize_quotes_utf8(const string &input)
{
    string out;

    for (size_t i = 0; i < input.size();)
    {
        unsigned char c = input[i];

        // detect UTF-8 length
        int len = 1;
        if ((c & 0x80) == 0x00)
            len = 1;
        else if ((c & 0xE0) == 0xC0)
            len = 2;
        else if ((c & 0xF0) == 0xE0)
            len = 3;
        else if ((c & 0xF8) == 0xF0)
            len = 4;

        string ch = input.substr(i, len);

        // ==========================
        // NORMALIZATION RULES
        // ==========================
        if (ch == "“" || ch == "”")
        {
            out += "\"";
        }
        else if (ch == "‘" || ch == "’")
        {
            out += "'";
        }
        else
        {
            out += ch;
        }

        i += len;
    }

    return out;
}

vector<uint8_t> bitpack(const vector<uint16_t> &tokens, uint8_t bits)
{
    vector<uint8_t> out;

    uint64_t buffer = 0;
    int bitCount = 0;

    for (uint16_t token : tokens)
    {
        buffer |= ((uint64_t)token << bitCount);
        bitCount += bits;

        while (bitCount >= 8)
        {
            out.push_back(buffer & 0xFF);
            buffer >>= 8;
            bitCount -= 8;
        }
    }

    if (bitCount > 0)
    {
        out.push_back(buffer & 0xFF);
    }

    return out;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage:   compressor <output>\n";
        std::cerr << "Example: compressor compressed_bible.hpp\n";
        return 1;
    }

    ofstream out(argv[1]);

    auto books = loadBooks();

    string line;

    vector<VerseTokens> all_tokens;
    unordered_map<string, int> freq;

    for (auto &bookPtr : books)
    {
        const auto &book = *bookPtr;

        uint16_t bookId = NEXT_BOOK_ID++;
        BOOK_NAME_TO_ID[book.name] = bookId;

        for (const auto &[chapterNum, verses] : book.chapters)
        {
            for (const auto &[verseNum, verseTextRaw] : verses)
            {
                string text = normalize_quotes_utf8(verseTextRaw);

                auto tokens = tokenize(text);

                all_tokens.push_back({bookId,
                                      (uint16_t)chapterNum,
                                      (uint16_t)verseNum,
                                      tokens});

                for (auto &t : tokens)
                    freq[t]++;
            }
        }
    }

    // ==========================
    // BUILD FULL DICTIONARY
    // ==========================
    vector<string> vocab;
    for (auto &[w, _] : freq)
        vocab.push_back(w);

    sort(vocab.begin(), vocab.end());

    unordered_map<string, uint16_t> dict;
    for (uint16_t i = 0; i < vocab.size(); i++)
        dict[vocab[i]] = i;

    uint32_t vocab_size = vocab.size();
    uint8_t bits = 0;
    while ((1u << bits) < vocab_size)
        bits++;
    cout << "\n  Using " << (int)bits << " bits per token\n";

    // ==========================
    // DICTIONARY STORAGE
    // ==========================
    vector<unsigned char> dict_data;
    vector<uint32_t> dict_offsets;

    for (auto &w : vocab)
    {
        dict_offsets.push_back(dict_data.size());

        for (unsigned char c : w)
            dict_data.push_back(c);

        dict_data.push_back('\0');
    }

    // ==========================
    // TOKEN STREAM + OFFSETS
    // ==========================
    vector<uint16_t> token_stream;
    vector<uint32_t> verse_offsets;

    vector<uint32_t> book_offsets;
    vector<uint32_t> chapter_offsets;
    vector<uint32_t> book_chapter_index;
    vector<uint16_t> chapter_count;

    uint16_t currentBook = UINT16_MAX;
    uint16_t currentChapter = UINT16_MAX;

    uint32_t chapterCounter = 0;

    for (auto &vt : all_tokens)
    {

        // NEW BOOK
        if (vt.book != currentBook)
        {
            currentBook = vt.book;
            currentChapter = UINT16_MAX;

            book_offsets.push_back(verse_offsets.size());
            book_chapter_index.push_back(chapter_offsets.size());

            if (chapterCounter > 0)
                chapter_count.push_back(chapterCounter);

            chapterCounter = 0;
        }

        // NEW CHAPTER
        if (vt.chapter != currentChapter)
        {
            currentChapter = vt.chapter;

            chapter_offsets.push_back(
                verse_offsets.size() - book_offsets.back());

            chapterCounter++;
        }

        // VERSE START
        verse_offsets.push_back(token_stream.size());

        // TOKENS
        for (auto &t : vt.tokens)
        {
            auto it = dict.find(t);
            if (it == dict.end())
            {
                cout << "Missing token: " << t << endl;
                exit(1);
            }
            token_stream.push_back(it->second);
        }
    }

    // last book
    chapter_count.push_back(chapterCounter);

    // ==========================
    // OUTPUT HEADER
    // ==========================
    // ofstream out("compressed_bible.hpp");

    out << "// ==========================================\n";
    out << "// COMPRESSED BIBLE FORMAT\n";
    out << "// ==========================================\n";
    out << "// - Tokenized text using dictionary encoding\n";
    out << "// - Tokens bit-packed for compact storage\n";
    out << "// - Indexed by book → chapter → verse\n";
    out << "// ==========================================\n\n";

    out << "#pragma once\n#include <cstdint>\n\n";

    // ==========================
    // DICTIONARY
    // ==========================

    // Flat byte array containing all dictionary words concatenated,
    // each terminated by '\0' (C-style string).
    // Example: "God\0 said\0 light\0"
    out << "// Concatenated dictionary words (null-terminated)\n";
    out << "static const unsigned char DICTIONARY_DATA[] = {";
    for (auto c : dict_data)
        out << (int)c << ",";
    out << "};\n\n";

    // Offsets into DICTIONARY_DATA where each word starts.
    // DICTIONARY_OFFSETS[i] → starting byte of token i
    out << "// Starting index of each word inside DICTIONARY_DATA\n";
    out << "static const uint32_t DICTIONARY_OFFSETS[] = {";
    for (auto v : dict_offsets)
        out << v << ",";
    out << "};\n\n";

    // ==========================
    // TOKEN STREAM
    // ==========================

    // Bit-packed token stream.
    // Each token is an index into the dictionary.
    // Tokens are packed using TOKEN_BITS bits per token.
    auto packed = bitpack(token_stream, bits);

    out << "// Bit-packed stream of dictionary token IDs\n";
    out << "static const uint8_t TOKEN_STREAM[] = {";
    for (auto v : packed)
        out << (int)v << ",";
    out << "};\n\n";

    // Number of bits used per token in TOKEN_STREAM
    out << "// Number of bits per token in TOKEN_STREAM\n";
    out << "static const uint8_t TOKEN_BITS = " << (int)bits << ";\n\n";

    // ==========================
    // VERSE INDEXING
    // ==========================

    // For each verse, stores the starting token index in TOKEN_STREAM.
    // VERSE_OFFSETS[i] → first token of verse i
    out << "// Token index where each verse begins\n";
    out << "static const uint32_t VERSE_OFFSETS[] = {";
    for (auto v : verse_offsets)
        out << v << ",";
    out << "};\n\n";

    // ==========================
    // BOOK / CHAPTER STRUCTURE
    // ==========================

    // BOOK_OFFSETS[i] → index in VERSE_OFFSETS where book i starts
    out << "// Verse index where each book begins\n";
    out << "static const uint32_t BOOK_OFFSETS[] = {";
    for (auto v : book_offsets)
        out << v << ",";
    out << "};\n\n";

    // CHAPTER_OFFSETS stores chapter starts RELATIVE to each book.
    // To find absolute verse index:
    //   BOOK_OFFSETS[book] + CHAPTER_OFFSETS[chapterIndex]
    out << "// Chapter start offsets (relative to each book)\n";
    out << "static const uint32_t CHAPTER_OFFSETS[] = {";
    for (auto v : chapter_offsets)
        out << v << ",";
    out << "};\n\n";

    // Maps each book to its starting index in CHAPTER_OFFSETS.
    // BOOK_CHAPTER_INDEX[i] → first chapter index for book i
    out << "// Index into CHAPTER_OFFSETS where each book's chapters begin\n";
    out << "static const uint32_t BOOK_CHAPTER_INDEX[] = {";
    for (auto v : book_chapter_index)
        out << v << ",";
    out << "};\n\n";

    // Number of chapters in each book
    out << "// Number of chapters per book\n";
    out << "static const uint16_t CHAPTER_COUNT[] = {";
    for (auto v : chapter_count)
        out << v << ",";
    out << "};\n\n";

    // ==========================
    // GLOBAL METADATA
    // ==========================

    // Total number of verses in the Bible
    out << "// Total number of verses\n";
    out << "static const uint32_t VERSE_COUNT = " << verse_offsets.size() << ";\n\n";

    // Size (in bytes) of the packed TOKEN_STREAM array
    out << "// Size in bytes of TOKEN_STREAM\n";
    out << "static const uint32_t TOKEN_STREAM_SIZE = " << packed.size() << ";\n\n";

    // Total number of tokens BEFORE bit-packing
    // Needed to correctly decode the last verse
    out << "// Total number of tokens (before bit-packing)\n";
    out << "static const uint32_t TOTAL_TOKENS = " << token_stream.size() << ";\n";

    // ==========================
    // LOG
    // ==========================
    cout << "  Generated file: " << argv[1] << "\n";
    cout << "  Books: " << book_offsets.size() << "\n";
    cout << "  Chapters: " << chapter_offsets.size() << "\n";
    cout << "  Verses: " << verse_offsets.size() << "\n\n";
}
