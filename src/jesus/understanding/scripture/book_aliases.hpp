#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "book_id.hpp"

namespace HolyBible
{
    struct BookAliasGroup
    {
        HolyBible::BookId id;
        std::vector<std::string> aliases;
    };

    const std::vector<BookAliasGroup> BOOK_ALIAS_GROUPS = {
        // Old Testament
        {HolyBible::BookId::Genesis, {"genesis", "gen"}},
        {HolyBible::BookId::Exodus, {"exodus", "exodo", "exo"}},
        {HolyBible::BookId::Leviticus, {"leviticus", "levitico", "lev"}},
        {HolyBible::BookId::Numbers, {"numbers", "numeros", "num"}},
        {HolyBible::BookId::Deuteronomy, {"deuteronomy", "deuteronomio", "deut", "deu"}},
        {HolyBible::BookId::Joshua, {"joshua", "josue", "jos", "josh"}},
        {HolyBible::BookId::Judges, {"judges", "juizes", "jueces", "judg"}},
        {HolyBible::BookId::Ruth, {"ruth", "rute", "rut"}},
        {HolyBible::BookId::FirstSamuel, {"1samuel", "1s", "1sa", "1sm", "1sam"}},
        {HolyBible::BookId::SecondSamuel, {"2samuel", "2s", "2sa", "2sm", "2sam"}},
        {HolyBible::BookId::FirstKings, {"1kings", "1reis", "1reyes", "1k", "1ki", "1re"}},
        {HolyBible::BookId::SecondKings, {"2kings", "2reis", "2reyes", "2k", "2ki", "2re"}},
        {HolyBible::BookId::FirstChronicles, {"1chronicles", "1cronicas", "1chr", "1ch", "1c", "1cr"}},
        {HolyBible::BookId::SecondChronicles, {"2chronicles", "2cronicas", "2chr", "2ch", "2c", "2cr"}},
        {HolyBible::BookId::Ezra, {"ezra", "esdras", "ezd", "ezr"}},
        {HolyBible::BookId::Nehemiah, {"nehemiah", "neemias", "nehemias", "nee", "neh"}},
        {HolyBible::BookId::Esther, {"esther", "ester", "est"}},
        {HolyBible::BookId::Job, {"job", "jo"}},
        {HolyBible::BookId::Psalms, {"psalms", "salmos", "salmo", "psa", "psm", "pslm", "psalm", "sal"}},
        {HolyBible::BookId::Proverbs, {"proverbs", "proverbios", "prov", "pro"}},
        {HolyBible::BookId::Ecclesiastes, {"ecclesiastes", "eclesiastes", "eccles", "eccle", "ecc", "ecl"}},
        {HolyBible::BookId::SongofSolomon, {"songs", "cantares", "song", "sos", "canticles", "cant", "can"}},
        {HolyBible::BookId::Isaiah, {"isaiah", "isaias", "isa"}},
        {HolyBible::BookId::Jeremiah, {"jeremiah", "jeremias", "jer"}},
        {HolyBible::BookId::Lamentations, {"lamentations", "lamentacoes", "lamentaciones", "lam"}},
        {HolyBible::BookId::Ezekiel, {"ezekiel", "ezequiel", "ezek", "eze", "ezk"}},
        {HolyBible::BookId::Daniel, {"daniel", "dan"}},
        {HolyBible::BookId::Hosea, {"hosea", "oseias", "oseas", "hos", "ose"}},
        {HolyBible::BookId::Joel, {"joel"}},
        {HolyBible::BookId::Amos, {"amos", "amo"}},
        {HolyBible::BookId::Obadiah, {"obadiah", "obadias", "abdias", "obad", "oba"}},
        {HolyBible::BookId::Jonah, {"jonah", "jonas", "jon"}},
        {HolyBible::BookId::Micah, {"micah", "miqueias", "miqueas", "mic", "miq"}},
        {HolyBible::BookId::Nahum, {"nahum", "naum", "nah", "nau"}},
        {HolyBible::BookId::Habakkuk, {"habakkuk", "habacuque", "habacuc", "hab"}},
        {HolyBible::BookId::Zephaniah, {"zephaniah", "sofonias", "zeph", "zep", "sof"}},
        {HolyBible::BookId::Haggai, {"haggai", "ageu", "hageo", "hag"}},
        {HolyBible::BookId::Zechariah, {"zechariah", "zacarias", "zech", "zec", "zac"}},
        {HolyBible::BookId::Malachi, {"malachi", "malaquias", "mal"}},

        // New Testament
        {HolyBible::BookId::Matthew, {"matthew", "mateus", "mateo", "matt", "mat"}},
        {HolyBible::BookId::Mark, {"mark", "marcos", "mrk", "mar"}},
        {HolyBible::BookId::Luke, {"luke", "lucas", "luk", "luc"}},
        {HolyBible::BookId::John, {"john", "joao", "juan"}},
        {HolyBible::BookId::Acts, {"acts", "atos", "hechos", "act", "hec"}},
        {HolyBible::BookId::Romans, {"romans", "romanos", "rom"}},
        {HolyBible::BookId::FirstCorinthians, {"1corinthians", "1corintios", "1cor", "1co"}},
        {HolyBible::BookId::SecondCorinthians, {"2corinthians", "2corintios", "2cor", "2co"}},
        {HolyBible::BookId::Galatians, {"galatians", "galatas", "gal"}},
        {HolyBible::BookId::Ephesians, {"ephesians", "efesios", "ephes", "eph", "efe"}},
        {HolyBible::BookId::Philippians, {"philippians", "filipenses", "phili", "fili"}},
        {HolyBible::BookId::Colossians, {"colossians", "colossenses", "colosenses", "col"}},
        {HolyBible::BookId::FirstThessalonians, {"1thessalonians", "1tessalonicenses", "1tesalonicenses", "1thess", "1th", "1te"}},
        {HolyBible::BookId::SecondThessalonians, {"2thessalonians", "2tessalonicenses", "2tesalonicenses", "2thess", "2th", "2te"}},
        {HolyBible::BookId::FirstTimothy, {"1timothy", "1timoteo", "1tim", "1tm"}},
        {HolyBible::BookId::SecondTimothy, {"2timothy", "2timoteo", "2tim", "2tm"}},
        {HolyBible::BookId::Titus, {"titus", "tito"}},
        {HolyBible::BookId::Philemon, {"philemon", "filemon", "philem", "phile", "file"}},
        {HolyBible::BookId::Hebrews, {"hebrews", "hebreus", "hebreos", "heb"}},
        {HolyBible::BookId::James, {"james", "tiago", "santiago", "jam", "san"}},
        {HolyBible::BookId::FirstPeter, {"1peter", "1pedro", "1pe"}},
        {HolyBible::BookId::SecondPeter, {"2peter", "2pedro", "2pe"}},
        {HolyBible::BookId::FirstJohn, {"1john", "1joao", "1juan", "1jo", "1ju"}},
        {HolyBible::BookId::SecondJohn, {"2john", "2joao", "2juan", "2jo", "2ju"}},
        {HolyBible::BookId::ThirdJohn, {"3john", "3joao", "3juan", "3jo", "3ju"}},
        {HolyBible::BookId::Jude, {"jude", "judas"}},
        {HolyBible::BookId::Revelation, {"revelation", "apocalipse", "apocalipsis", "rev", "apo"}}};

    std::unordered_map<std::string, HolyBible::BookId> buildAliasMap();
    extern const std::unordered_map<std::string, HolyBible::BookId> BOOK_ALIASES;
};