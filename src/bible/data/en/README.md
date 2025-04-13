# WEB (World English Bible) C++ classes

The ***.hpp*** files located at this folder were created using the following script:

```python
#! /usr/bin/env python

"""
Create cpp classes for each book of the bible

This file is expected to be called in the following way:

    ./tocpp.py genesis.hpp English

This file was called using the following command
in the json folder of the World English Bible repository,
availble at https://github.com/TehShrike/world-english-bible/tree/master/json:

    for f in $(ls *.json); do name=$(echo $f | cut -d'.' -f1); ./tocpp.py $f English > ../../en/$name.h; done
"""
import json
import sys

books_dict = {
    'genesis.json': (1, 'Genesis', 'genesis.json'),
    'exodus.json': (2, 'Exodus', 'exodus.json'),
    'leviticus.json': (3, 'Leviticus', 'leviticus.json'),
    'numbers.json': (4, 'Numbers', 'numbers.json'),
    'deuteronomy.json': (5, 'Deuteronomy', 'deuteronomy.json'),
    'joshua.json': (6, 'Joshua', 'joshua.json'),
    'judges.json': (7, 'Judges', 'judges.json'),
    'ruth.json': (8, 'Ruth', 'ruth.json'),
    '1samuel.json': (9, '1 Samuel', '1samuel.json'),
    '2samuel.json': (10, '2 Samuel', '2samuel.json'),
    '1kings.json': (11, '1 Kings', '1kings.json'),
    '2kings.json': (12, '2 Kings', '2kings.json'),
    '1chronicles.json': (13, '1 Chronicles', '1chronicles.json'),
    '2chronicles.json': (14, '2 Chronicles', '2chronicles.json'),
    'ezra.json': (15, 'Ezra', 'ezra.json'),
    'nehemiah.json': (16, 'Nehemiah', 'nehemiah.json'),
    'esther.json': (17, 'Esther', 'esther.json'),
    'job.json': (18, 'Job', 'job.json'),
    'psalms.json': (19, 'Psalms', 'psalms.json'),
    'proverbs.json': (20, 'Proverbs', 'proverbs.json'),
    'ecclesiastes.json': (21, 'Ecclesiastes', 'ecclesiastes.json'),
    'songofsolomon.json': (22, 'Song of Solomon', 'songofsolomon.json'),
    'isaiah.json': (23, 'Isaiah', 'isaiah.json'),
    'jeremiah.json': (24, 'Jeremiah', 'jeremiah.json'),
    'lamentations.json': (25, 'Lamentations', 'lamentations.json'),
    'ezekiel.json': (26, 'Ezekiel', 'ezekiel.json'),
    'daniel.json': (27, 'Daniel', 'daniel.json'),
    'hosea.json': (28, 'Hosea', 'hosea.json'),
    'joel.json': (29, 'Joel', 'joel.json'),
    'amos.json': (30, 'Amos', 'amos.json'),
    'obadiah.json': (31, 'Obadiah', 'obadiah.json'),
    'jonah.json': (32, 'Jonah', 'jonah.json'),
    'micah.json': (33, 'Micah', 'micah.json'),
    'nahum.json': (34, 'Nahum', 'nahum.json'),
    'habakkuk.json': (35, 'Habakkuk', 'habakkuk.json'),
    'zephaniah.json': (36, 'Zephaniah', 'zephaniah.json'),
    'haggai.json': (37, 'Haggai', 'haggai.json'),
    'zechariah.json': (38, 'Zechariah', 'zechariah.json'),
    'malachi.json': (39, 'Malachi', 'malachi.json'),
    'matthew.json': (40, 'Matthew', 'matthew.json'),
    'mark.json': (41, 'Mark', 'mark.json'),
    'luke.json': (42, 'Luke', 'luke.json'),
    'john.json': (43, 'John', 'john.json'),
    'acts.json': (44, 'Acts', 'acts.json'),
    'romans.json': (45, 'Romans', 'romans.json'),
    '1corinthians.json': (46, '1 Corinthians', '1corinthians.json'),
    '2corinthians.json': (47, '2 Corinthians', '2corinthians.json'),
    'galatians.json': (48, 'Galatians', 'galatians.json'),
    'ephesians.json': (49, 'Ephesians', 'ephesians.json'),
    'philippians.json': (50, 'Philippians', 'philippians.json'),
    'colossians.json': (51, 'Colossians', 'colossians.json'),
    '1thessalonians.json': (52, '1 Thessalonians', '1thessalonians.json'),
    '2thessalonians.json': (53, '2 Thessalonians', '2thessalonians.json'),
    '1timothy.json': (54, '1 Timothy', '1timothy.json'),
    '2timothy.json': (55, '2 Timothy', '2timothy.json'),
    'titus.json': (56, 'Titus', 'titus.json'),
    'philemon.json': (57, 'Philemon', 'philemon.json'),
    'hebrews.json': (58, 'Hebrews', 'hebrews.json'),
    'james.json': (59, 'James', 'james.json'),
    '1peter.json': (60, '1 Peter', '1peter.json'),
    '2peter.json': (61, '2 Peter', '2peter.json'),
    '1john.json': (62, '1 John', '1john.json'),
    '2john.json': (63, '2 John', '2john.json'),
    '3john.json': (64, '3 John', '3john.json'),
    'jude.json': (65, 'Jude', 'jude.json'),
    'revelation.json': (66, 'Revelation', 'revelation.json')
}

def generate_cpp_class(number, name, filename, namespace):
    f = open(filename)

    data = json.load(f)
    className = name.replace('1', 'First').replace('2', 'Second').replace('3', 'Third').replace(' ', '')

    chapter = 0
    filename_nojson = filename.split('.json')[0]
    define_name = f'{namespace}_{filename_nojson}'.upper()
    print(f'#ifndef {define_name}')
    print(f'#define {define_name}\n')

    print('#include <book/book.hpp>\n')

    print(f'namespace {namespace}\n' + '{')
    print(f'    class {className} : public Book')
    print('    {')
    print('    public:')
    print(f'        {className}()')
    print('            : Book(')
    print(f'                  {number}, "{className}",')

    print('                  {')
    close_chapter = False
    previousVerse = 0
    verseContent = ''
    verses = {}

    def print_verses(chapter):
        if close_chapter:
            print('                       }},')

        print('                      {' + f'{chapter},')
        print('                       {')
        for number, verse in verses.items():
            print('                           {' + f'{number}, "{verse}"' + '},')

    for verse in data:

        chapterNumber = verse.get('chapterNumber') or 0
        if chapterNumber:
            verseNumber = int(verse['verseNumber'])
            value = verse['value'].strip()
            if not value:
                continue

            if verseNumber == previousVerse:
                verseContent = f'{verseContent} {value}'
                verses[verseNumber] = verseContent
                continue
            else:
                verseContent = value

            previousVerse = verseNumber

            new_chapter = chapterNumber > chapter and chapterNumber > 1
            chapter = chapterNumber
            if new_chapter:

                # --------------------------------
                # Print verses of previous chapter
                # --------------------------------
                print_verses(chapter -1)

                close_chapter = True

                # ---------------------
                # Reset the verses dict
                # ---------------------
                verses = {}


            verses[verseNumber] = verseContent

    # ---------------------------
    # Print verses of last chapter
    # ----------------------------
    print_verses(chapter)

    print('                       }},')
    print('                  }){};') # </constructor>
    print('    };') # </class>
    print('}') # </namespace>

    print(f'\n#endif // {define_name}')


def main():
    book_fname = sys.argv[1]
    namespace = sys.argv[2]
    number, name, filename = books_dict[book_fname]

    generate_cpp_class(number, name, filename, namespace)


if __name__ == '__main__':
    main()
```
