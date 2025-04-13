# Reina Valera 1960 Bible C++ classes

The ***.hpp*** files located at this folder were created using the following script:

```python
#! /usr/bin/env python

"""
Create cpp classes for each book of the bible (Reina Valera 1960 version)

This file is expected to be called in the following way:

    ./tocpp.py genesis.hpp Spanish

This file was called using the following command
in the json folder of the church-utils repository,
availble at https://github.com/xtiam57/church-utils/tree/main/dist/biblia:

    for f in $(ls *.json); do name=$(echo $f | cut -d'.' -f1); ./tocpp.py $f Spanish > ../../../$name.hpp; done
"""
import json
import sys

books_dict = {
    'genesis.json': (1, 'Genesis', 'Génesis'),
    'exodo.json': (2, 'Exodus', 'Éxodo'),
    'levitico.json': (3, 'Leviticus', 'Levítico'),
    'numeros.json': (4, 'Numbers', 'Números'),
    'deuteronomio.json': (5, 'Deuteronomy', 'Deuteronomio'),
    'josue.json': (6, 'Joshua', 'Josué'),
    'jueces.json': (7, 'Judges', 'Jueces'),
    'rut.json': (8, 'Ruth', 'Rut'),
    '1_samuel.json': (9, '1 Samuel', '1 Samuel'),
    '2_samuel.json': (10, '2 Samuel', '2 Samuel'),
    '1_reyes.json': (11, '1 Kings', '1 Reyes'),
    '2_reyes.json': (12, '2 Kings', '2 Reyes'),
    '1_cronicas.json': (13, '1 Chronicles', '1 Crónicas'),
    '2_cronicas.json': (14, '2 Chronicles', '2 Crónicas'),
    'esdras.json': (15, 'Ezra', 'Esdras'),
    'nehemias.json': (16, 'Nehemiah', 'Nehemías'),
    'ester.json': (17, 'Esther', 'Ester'),
    'job.json': (18, 'Job', 'Job'),
    'salmos.json': (19, 'Psalms', 'Salmos'),
    'proverbios.json': (20, 'Proverbs', 'Proverbios'),
    'eclesiastes.json': (21, 'Ecclesiastes', 'Eclesiastés'),
    'cantares.json': (22, 'Song of Solomon', 'Cantares'),
    'isaias.json': (23, 'Isaiah', 'Isaías'),
    'jeremias.json': (24, 'Jeremiah', 'Jeremías'),
    'lamentaciones.json': (25, 'Lamentations', 'Lamentaciones'),
    'ezequiel.json': (26, 'Ezekiel', 'Ezequiel'),
    'daniel.json': (27, 'Daniel', 'Daniel'),
    'oseas.json': (28, 'Hosea', 'Oseas'),
    'joel.json': (29, 'Joel', 'Joel'),
    'amos.json': (30, 'Amos', 'Amós'),
    'abdias.json': (31, 'Obadiah', 'Abdías'),
    'jonas.json': (32, 'Jonah', 'Jonás'),
    'miqueas.json': (33, 'Micah', 'Miqueas'),
    'nahum.json': (34, 'Nahum', 'Nahúm'),
    'habacuc.json': (35, 'Habakkuk', 'Habacuc'),
    'sofonias.json': (36, 'Zephaniah', 'Sofonías'),
    'hageo.json': (37, 'Haggai', 'Hageo'),
    'zacarias.json': (38, 'Zechariah', 'Zacarías'),
    'malaquias.json': (39, 'Malachi', 'Malaquías'),
    'mateo.json': (40, 'Matthew', 'Mateo'),
    'marcos.json': (41, 'Mark', 'Marcos'),
    'lucas.json': (42, 'Luke', 'Lucas'),
    'juan.json': (43, 'John', 'Juan'),
    'hechos.json': (44, 'Acts', 'Hechos'),
    'romanos.json': (45, 'Romans', 'Romanos'),
    '1_corintios.json': (46, '1 Corinthians', '1 Corintios'),
    '2_corintios.json': (47, '2 Corinthians', '2 Corintios'),
    'galatas.json': (48, 'Galatians', 'Gálatas'),
    'efesios.json': (49, 'Ephesians', 'Efesios'),
    'filipenses.json': (50, 'Philippians', 'Filipenses'),
    'colosenses.json': (51, 'Colossians', 'Colosenses'),
    '1_tesalonicenses.json': (52, '1 Thessalonians', '1 Tesalonicenses'),
    '2_tesalonicenses.json': (53, '2 Thessalonians', '2 Tesalonicenses'),
    '1_timoteo.json': (54, '1 Timothy', '1 Timoteo'),
    '2_timoteo.json': (55, '2 Timothy', '2 Timoteo'),
    'tito.json': (56, 'Titus', 'Tito'),
    'filemon.json': (57, 'Philemon', 'Filemón'),
    'hebreos.json': (58, 'Hebrews', 'Hebreos'),
    'santiago.json': (59, 'James', 'Santiago'),
    '1_pedro.json': (60, '1 Peter', '1 Pedro'),
    '2_pedro.json': (61, '2 Peter', '2 Pedro'),
    '1_juan.json': (62, '1 John', '1 Juan'),
    '2_juan.json': (63, '2 John', '2 Juan'),
    '3_juan.json': (64, '3 John', '3 Juan'),
    'judas.json': (65, 'Jude', 'Judas'),
    'apocalipsis.json': (66, 'Revelation', 'Apocalipsis')
}

def generate_cpp_class(number, name, filename, namespace, spanish_name):
    f = open(filename)

    data = json.load(f)
    className = name.replace('1', 'First').replace('2', 'Second').replace('3', 'Third').replace(' ', '')

    chapter = 0
    filename_nojson = name.replace(' ', '').lower()
    define_name = f'{namespace}_{filename_nojson.replace(" ", "")}'.upper()
    print(f'#ifndef {define_name}')
    print(f'#define {define_name}\n')

    print('#include <book/book.hpp>\n')

    print(f'namespace {namespace}\n' + '{')
    print(f'    class {className} : public Book')
    print('    {')
    print('    public:')
    print(f'        {className}()')
    print('            : Book(')
    print(f'                  {number}, "{spanish_name}",')

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

    chapterNumber = 0
    for chapter in data:
        chapterNumber += 1
        verseNumber = 0
        for verse in chapter:
            verseNumber += 1

            verseContent = verse.replace('_', '')

            verses[verseNumber] = verseContent

        print_verses(chapterNumber)
        close_chapter = True
        verses = {}

    # ---------------------------
    # Print verses of last chapter
    # ----------------------------
    # print_verses(chapter)

    print('                       }},')
    print('                  }){};') # </constructor>
    print('    };') # </class>
    print('}') # </namespace>

    print(f'\n#endif // {define_name}')


def main():
    book_fname = sys.argv[1]
    namespace = sys.argv[2]
    number, name, spanish_name = books_dict[book_fname]

    generate_cpp_class(number, name, book_fname, namespace, spanish_name)


if __name__ == '__main__':
    main()

```
