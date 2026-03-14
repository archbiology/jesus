#include "book_registry.hpp"
#include "../../bible/data/en/__books__.hpp"
#include "../../bible/data/es/__books__.hpp"


static English::Genesis en_Genesis;
static English::Exodus en_Exodus;
static English::Leviticus en_Leviticus;
static English::Numbers en_Numbers;
static English::Deuteronomy en_Deuteronomy;
static English::Joshua en_Joshua;
static English::Judges en_Judges;
static English::Ruth en_Ruth;
static English::FirstSamuel en_FirstSamuel;
static English::SecondSamuel en_SecondSamuel;
static English::FirstKings en_FirstKings;
static English::SecondKings en_SecondKings;
static English::FirstChronicles en_FirstChronicles;
static English::SecondChronicles en_SecondChronicles;
static English::Ezra en_Ezra;
static English::Nehemiah en_Nehemiah;
static English::Esther en_Esther;
static English::Job en_Job;
static English::Psalms en_Psalms;
static English::Proverbs en_Proverbs;
static English::Ecclesiastes en_Ecclesiastes;
static English::SongofSolomon en_SongofSolomon;
static English::Isaiah en_Isaiah;
static English::Jeremiah en_Jeremiah;
static English::Lamentations en_Lamentations;
static English::Ezekiel en_Ezekiel;
static English::Daniel en_Daniel;
static English::Hosea en_Hosea;
static English::Joel en_Joel;
static English::Amos en_Amos;
static English::Obadiah en_Obadiah;
static English::Jonah en_Jonah;
static English::Micah en_Micah;
static English::Nahum en_Nahum;
static English::Habakkuk en_Habakkuk;
static English::Zephaniah en_Zephaniah;
static English::Haggai en_Haggai;
static English::Zechariah en_Zechariah;
static English::Malachi en_Malachi;
static English::Matthew en_Matthew;
static English::Mark en_Mark;
static English::Luke en_Luke;
static English::John en_John;
static English::Acts en_Acts;
static English::Romans en_Romans;
static English::FirstCorinthians en_FirstCorinthians;
static English::SecondCorinthians en_SecondCorinthians;
static English::Galatians en_Galatians;
static English::Ephesians en_Ephesians;
static English::Philippians en_Philippians;
static English::Colossians en_Colossians;
static English::FirstThessalonians en_FirstThessalonians;
static English::SecondThessalonians en_SecondThessalonians;
static English::FirstTimothy en_FirstTimothy;
static English::SecondTimothy en_SecondTimothy;
static English::Titus en_Titus;
static English::Philemon en_Philemon;
static English::Hebrews en_Hebrews;
static English::James en_James;
static English::FirstPeter en_FirstPeter;
static English::SecondPeter en_SecondPeter;
static English::FirstJohn en_FirstJohn;
static English::SecondJohn en_SecondJohn;
static English::ThirdJohn en_ThirdJohn;
static English::Jude en_Jude;
static English::Revelation en_Revelation;

static Spanish::Genesis es_Genesis;
static Spanish::Exodus es_Exodus;
static Spanish::Leviticus es_Leviticus;
static Spanish::Numbers es_Numbers;
static Spanish::Deuteronomy es_Deuteronomy;
static Spanish::Joshua es_Joshua;
static Spanish::Judges es_Judges;
static Spanish::Ruth es_Ruth;
static Spanish::FirstSamuel es_FirstSamuel;
static Spanish::SecondSamuel es_SecondSamuel;
static Spanish::FirstKings es_FirstKings;
static Spanish::SecondKings es_SecondKings;
static Spanish::FirstChronicles es_FirstChronicles;
static Spanish::SecondChronicles es_SecondChronicles;
static Spanish::Ezra es_Ezra;
static Spanish::Nehemiah es_Nehemiah;
static Spanish::Esther es_Esther;
static Spanish::Job es_Job;
static Spanish::Psalms es_Psalms;
static Spanish::Proverbs es_Proverbs;
static Spanish::Ecclesiastes es_Ecclesiastes;
static Spanish::SongofSolomon es_SongofSolomon;
static Spanish::Isaiah es_Isaiah;
static Spanish::Jeremiah es_Jeremiah;
static Spanish::Lamentations es_Lamentations;
static Spanish::Ezekiel es_Ezekiel;
static Spanish::Daniel es_Daniel;
static Spanish::Hosea es_Hosea;
static Spanish::Joel es_Joel;
static Spanish::Amos es_Amos;
static Spanish::Obadiah es_Obadiah;
static Spanish::Jonah es_Jonah;
static Spanish::Micah es_Micah;
static Spanish::Nahum es_Nahum;
static Spanish::Habakkuk es_Habakkuk;
static Spanish::Zephaniah es_Zephaniah;
static Spanish::Haggai es_Haggai;
static Spanish::Zechariah es_Zechariah;
static Spanish::Malachi es_Malachi;
static Spanish::Matthew es_Matthew;
static Spanish::Mark es_Mark;
static Spanish::Luke es_Luke;
static Spanish::John es_John;
static Spanish::Acts es_Acts;
static Spanish::Romans es_Romans;
static Spanish::FirstCorinthians es_FirstCorinthians;
static Spanish::SecondCorinthians es_SecondCorinthians;
static Spanish::Galatians es_Galatians;
static Spanish::Ephesians es_Ephesians;
static Spanish::Philippians es_Philippians;
static Spanish::Colossians es_Colossians;
static Spanish::FirstThessalonians es_FirstThessalonians;
static Spanish::SecondThessalonians es_SecondThessalonians;
static Spanish::FirstTimothy es_FirstTimothy;
static Spanish::SecondTimothy es_SecondTimothy;
static Spanish::Titus es_Titus;
static Spanish::Philemon es_Philemon;
static Spanish::Hebrews es_Hebrews;
static Spanish::James es_James;
static Spanish::FirstPeter es_FirstPeter;
static Spanish::SecondPeter es_SecondPeter;
static Spanish::FirstJohn es_FirstJohn;
static Spanish::SecondJohn es_SecondJohn;
static Spanish::ThirdJohn es_ThirdJohn;
static Spanish::Jude es_Jude;
static Spanish::Revelation es_Revelation;

const HolyBible::Book *HolyBible::BookRegistry::books[2][66] = {
    {// English
     &en_Genesis,
     &en_Exodus,
     &en_Leviticus,
     &en_Numbers,
     &en_Deuteronomy,
     &en_Joshua,
     &en_Judges,
     &en_Ruth,
     &en_FirstSamuel,
     &en_SecondSamuel,
     &en_FirstKings,
     &en_SecondKings,
     &en_FirstChronicles,
     &en_SecondChronicles,
     &en_Ezra,
     &en_Nehemiah,
     &en_Esther,
     &en_Job,
     &en_Psalms,
     &en_Proverbs,
     &en_Ecclesiastes,
     &en_SongofSolomon,
     &en_Isaiah,
     &en_Jeremiah,
     &en_Lamentations,
     &en_Ezekiel,
     &en_Daniel,
     &en_Hosea,
     &en_Joel,
     &en_Amos,
     &en_Obadiah,
     &en_Jonah,
     &en_Micah,
     &en_Nahum,
     &en_Habakkuk,
     &en_Zephaniah,
     &en_Haggai,
     &en_Zechariah,
     &en_Malachi,
     &en_Matthew,
     &en_Mark,
     &en_Luke,
     &en_John,
     &en_Acts,
     &en_Romans,
     &en_FirstCorinthians,
     &en_SecondCorinthians,
     &en_Galatians,
     &en_Ephesians,
     &en_Philippians,
     &en_Colossians,
     &en_FirstThessalonians,
     &en_SecondThessalonians,
     &en_FirstTimothy,
     &en_SecondTimothy,
     &en_Titus,
     &en_Philemon,
     &en_Hebrews,
     &en_James,
     &en_FirstPeter,
     &en_SecondPeter,
     &en_FirstJohn,
     &en_SecondJohn,
     &en_ThirdJohn,
     &en_Jude,
     &en_Revelation},

    {// Spanish
     &es_Genesis,
     &es_Exodus,
     &es_Leviticus,
     &es_Numbers,
     &es_Deuteronomy,
     &es_Joshua,
     &es_Judges,
     &es_Ruth,
     &es_FirstSamuel,
     &es_SecondSamuel,
     &es_FirstKings,
     &es_SecondKings,
     &es_FirstChronicles,
     &es_SecondChronicles,
     &es_Ezra,
     &es_Nehemiah,
     &es_Esther,
     &es_Job,
     &es_Psalms,
     &es_Proverbs,
     &es_Ecclesiastes,
     &es_SongofSolomon,
     &es_Isaiah,
     &es_Jeremiah,
     &es_Lamentations,
     &es_Ezekiel,
     &es_Daniel,
     &es_Hosea,
     &es_Joel,
     &es_Amos,
     &es_Obadiah,
     &es_Jonah,
     &es_Micah,
     &es_Nahum,
     &es_Habakkuk,
     &es_Zephaniah,
     &es_Haggai,
     &es_Zechariah,
     &es_Malachi,
     &es_Matthew,
     &es_Mark,
     &es_Luke,
     &es_John,
     &es_Acts,
     &es_Romans,
     &es_FirstCorinthians,
     &es_SecondCorinthians,
     &es_Galatians,
     &es_Ephesians,
     &es_Philippians,
     &es_Colossians,
     &es_FirstThessalonians,
     &es_SecondThessalonians,
     &es_FirstTimothy,
     &es_SecondTimothy,
     &es_Titus,
     &es_Philemon,
     &es_Hebrews,
     &es_James,
     &es_FirstPeter,
     &es_SecondPeter,
     &es_FirstJohn,
     &es_SecondJohn,
     &es_ThirdJohn,
     &es_Jude,
     &es_Revelation}};
