
#pragma once
// ---------------------------------------------------------------------
// Doctrine / Law
// Ungodly Naming
//
// Enforces explicit acknowledgment when identifiers
// contain names associated with death, disease, destruction, etc.
//
// Rationale:
//   "Choose life, that you and your descendants may live." (Deut 30:19)
// ---------------------------------------------------------------------
#include <unordered_map>
#include <string>
#include <sstream>
#include <algorithm>
#include "../../../utils/terminal/terminal.hpp"
#include "../../../utils/terminal/color.hpp"

namespace doctrine::law
{

    static const std::string rev20_2 = "Revelation 20:2 — He seized the dragon, that ancient serpent, who is the devil, or Satan, and bound him for a thousand years.";
    static const std::string mark16_17 = "Mark 16:17 — And these signs will accompany those who believe: In my name they will drive out demons; they will speak in new tongues;";
    static const std::string gen6_4 = "Genesis 6:4 — The Nephilim were on the earth in those days — and also afterward — when the sons of God went to the daughters of humans and had children by them. They were the heroes of old, men of renown.";
    static const std::string _1John4_23 = "1 John 4:2-3 — This is how you can recognize the Spirit of God: Every spirit that acknowledges that Jesus Christ has come in the flesh is from God, but every spirit that does not acknowledge Jesus is not from God. This is the spirit of the antichrist, which you have heard is coming and even now is already in the world.";
    static const std::string john24_23_25 = "John 24:23-25 — Then if any man tells you, ‘Look, here is the Messiah!’ or, ‘There he is!’ don’t believe it.\nFor false christs/messiahs and false prophets will arise, and they will perform great signs and wonders, so as to lead astray, if possible, even the chosen ones.\nBehold, I have told you beforehand.";
    static const std::string rev_13_3 = "Revelation 13:3 —  One of the heads of the beast seemed to have had a fatal wound, but the fatal wound had been healed (a clone). The whole world was filled with wonder and followed the beast.";
    static const std::string dark_eyes = "Matthew 6:22-23 — The eye is the lamp of the body... So if the light within you is darkness — how deep is that darkness!";
    static const std::string gen3_14 = "Genesis 3:14 — So the Lord God said to the serpent, “Because you have done this, “Cursed are you above all livestock and all wild animals! You will crawl on your belly and you will eat dust all the days of your life.";
    static const std::string rev9_11 = "Revelation 9:11 — They had as king over them the angel of the Abyss, whose name in Hebrew is Abaddon and in Greek is Apollyon (that is, Destroyer).";
    static const std::string daniel2_43 = "Daniel 2:43 — As you saw the iron mixed with soft clay, so they will mix with one another in marriage, but they will not hold together, just as iron does not mix with clay.";
    static const std::string matthew7_15 = "Matthew 7:15 — Watch out for false prophets. They come to you in sheep’s clothing, but inwardly they are ferocious wolves.";
    static const std::string matthew23_9 = "Matthew 23:9 — And call no man your father on earth, for you have one Father, who is in heaven.";
    static const std::string mark_beast = "Revelation 14:11 — And the smoke of their torment will rise for ever and ever. There will be no rest day or night for those who worship the beast and its image, or for anyone who receives the mark of its name.\nRevelaton 13:18 — This calls for wisdom: let the one who has understanding calculate the number of the beast, for it is the number of a man, and his number is 666.";
    static const std::string rev_20_14 = "Revelation 20:14 — Then death and Hades/hell were thrown into the lake of fire. The lake of fire is the second death.";
    static const std::string gen_2_24 = "Genesis 2:24 — That is why a man leaves his father and mother and is united to his wife, and they become one flesh.";
    static const std::string gen_1_27 = "Genesis 1:27 — So God created mankind in his own image, in the image of God he created them; male and female he created them.";
    static const std::string you_shall_not_kill = "Exodus 20:13 — You shall not murder. 1 Corinthians 3:17 — If anyone destroys God’s temple, God will destroy that person; for God’s temple is sacred, and you together are that temple.";
    static const std::string rev_22_15 = "Revelation 22:15 — Outside are the dogs, those who practice magic arts (sorcerers), the sexually immoral, the murderers, the idolaters and everyone who loves and practices falsehood.";
    static const std::string you_shall_not_steal = "Exodus 20:15 — You shall not steal.\n" + rev_22_15;
    static const std::string samuel_15_23 = "1 Samuel 15:23 — For rebellion is as the sin of divination, and presumption is as iniquity and idolatry. Because you have rejected the word of the Lord, he has also rejected you from being king.";
    static const std::string be_healthy = "Deuteronomy 28:15+ —  However, if you do not obey the Lord your God and do not carefully follow all his commands and decrees I am giving you today, all these curses will come on you and overtake you: ...;\n1 Chronicles 15:23 — In the thirty-ninth year of his reign Asa was afflicted with a disease in his feet. Though his disease was severe, even in his illness he did not seek help from the Lord, but only from the physicians.";
    static const std::string romans_12_19 = "Romans 12:19 —  Do not take revenge, my dear friends, but leave room for God’s wrath, for it is written: “It is mine to avenge; I will repay,” says the Lord.";
    static const std::string be_pure = "Exodus 20:14 — You shall not commit adultery. Proverbs 7:26  — Many are the victims she has brought down; her deads are a mighty army.";
    static const std::string exodus_20_4_6 = "Exodus 20:4-6 — You shall not make for yourself an image in the form of anything in heaven above or on the earth beneath or in the waters below.\nYou shall not bow down to them or worship them; for I, the Lord your God, am a jealous God, punishing the children for the sin of the parents to the third and fourth generation of those who hate me,\nbut showing love to a thousand generations of those who love me and keep my commandments.";
    static const std::string _2kings_18_4 = "2 Kings 18:4 — He removed the high places, broke the pillars, and cut down the Asherah. He also broke in pieces the bronze serpent that Moses had made, because in those days the children of Israel burned incense to it; and he called it Nehushtan.";
    static const std::string no_statues = exodus_20_4_6 + "\n" + _2kings_18_4;
    static const std::string prov_20_1 = "Proverbs 20:1 — Wine causes mocking, and beer causes fights; everyone led astray by them lacks wisdom.";
    static const std::string prov_31_4 = "Proverbs 31:4 — It is not for kings, Lemuel — Not for kings to drink wine or for rulers to desire liquor.";
    static const std::string no_beer = prov_20_1 + "\n" + prov_31_4;
    static const std::string revelation_9_18 = "Revelation 9:18 — By these three plagues — the fire, the smoke, and the sulfur that came out of their mouths — one-third of humanity was killed.";
    static const std::string timothy6_10 = "1 Timothy 6:10 — For the love of money is a root of all kinds of evil. Some people, eager for money, have wandered from the faith and pierced themselves with many griefs.";
    static const std::string _2timothy1_7 = "2 Timothy 1:7 — For God hath not given us the spirit of fear; but of power, and of love, and of a sound mind.";
    static const std::string no_pride = "Proverbs 16:18 — Pride goes before destruction, a haughty spirit before a fall.";
    static const std::string prov_27_4 = "Proverbs 27:4 — Chemah (anger) is cruel, and fury is a torrent, but who is able to stand before kinah (jealousy, envy)?";
    static const std::string romans6_23 = "Romans 6:23 — For the wages of sin is death; but the gift of God is eternal life through Jesus Christ our Lord.";

    static const std::string confess = "1 John 1:9 — If we confess our sins (to Jesus Christ, not to man), he is faithful and just and will forgive us our sins and purify us from all unrighteousness.";

    static const std::unordered_map<std::string, std::string> UNGODLY_WORDS = {
        {"satan", rev20_2},
        {"satanas", rev20_2},
        {"dragon", rev20_2},
        {"dragao", rev20_2},
        {"evil", mark16_17},
        {"devil", mark16_17},
        {"demon", mark16_17},
        {"daemon", mark16_17},
        {"giant", gen6_4},  // Fallen angels + mankind or beast (Either Nephilim or Rephaim) - https://www.mylovelyjesusministry.com/
        {"giants", gen6_4},
        {"gigante", gen6_4},
        {"nephilim", gen6_4},  // Fallen angels 50% + mankind or beast 50% (pure nephilim, purebred) - https://www.mylovelyjesusministry.com/
        {"avengers", romans_12_19 + "\n" + confess},  // Nephilim - https://www.mylovelyjesusministry.com/
        {"rephaim", gen6_4},  // Fallen angels + mankind or beast (of 'second generation', not purebred with 50% of Godly DNA like the Nephilim) - https://www.mylovelyjesusministry.com/
        {"refaim", gen6_4 + "\n" + confess},
        {"refains", gen6_4 + "\n" + confess},
        {"rephaites", gen6_4},
        {"zamzummim", gen6_4},
        {"zanzumim", gen6_4},
        {"alien", dark_eyes + "\n" + _1John4_23}, // Alien = fallen angels, nephilim/rephaim, and demons (posessing robots) pretending to be good. - https://www.mylovelyjesusministry.com/
        {"aliens", dark_eyes + "\n" + _1John4_23},
        {"alienigena", dark_eyes + "\n" + _1John4_23},
        {"alienigenas", dark_eyes + "\n" + _1John4_23},
        {"snake", gen3_14},
        {"serpent", gen3_14},
        {"cobra", gen3_14},

        {"abaddon", rev9_11},
        {"abadom", rev9_11},
        {"apollyon", rev9_11},
        {"apoliom", rev9_11},
        {"destroyer", rev9_11},
        {"destruir", rev9_11},
        {"antichrist", _1John4_23},
        {"anticristo", _1John4_23},
        {"lucifer", rev20_2},
        {"ateu", _1John4_23 + "\n" + confess},
        {"atheist", _1John4_23 + "\n" + confess},
        {"atheism", _1John4_23 + "\n" + confess},
        {"skeptical", _1John4_23 + "\n" + confess},
        {"cetico", _1John4_23 + "\n" + confess},

        {"pope", matthew23_9 + "\n" + exodus_20_4_6},  // The Holy War - John Bunyan
        {"papa", matthew23_9 + "\n" + exodus_20_4_6},
        {"macron", _1John4_23 + "\n" + john24_23_25 + "\n" + rev_13_3},  // https://www.mylovelyjesusministry.com/
        {"obama", matthew7_15 + "\n" + john24_23_25},  // https://www.mylovelyjesusministry.com/
        {"musk", _1John4_23},    // https://www.mylovelyjesusministry.com/
        {"neuralink", daniel2_43},
        {"cyborg", daniel2_43},
        {"ciborgue", daniel2_43},
        {"transhumanism", daniel2_43},
        {"transumanismo", daniel2_43},
        {"666", mark_beast},
        {"mark", mark_beast},

        {"abyss", rev_20_14},
        {"sheol", rev_20_14},
        {"hell", rev_20_14},
        {"inferno", rev_20_14},
        {"volcano", rev_20_14},
        {"vulcao", rev_20_14},
        {"vulkan", rev_20_14},

        {"die", you_shall_not_kill},
        {"death", you_shall_not_kill},
        {"kill", you_shall_not_kill + "\n" + confess},
        {"killer", you_shall_not_kill + "\n" + confess},
        {"suicide", you_shall_not_kill + "\n" + confess},
        {"suicidio", you_shall_not_kill + "\n" + confess},
        {"suicidar", you_shall_not_kill + "\n" + confess},
        {"harakiri", you_shall_not_kill + "\n" + confess},
        {"abortion", you_shall_not_kill + "\n" + confess},
        {"aborto", you_shall_not_kill + "\n" + confess},
        {"abortar", you_shall_not_kill + "\n" + confess},
        {"steal", you_shall_not_steal + "\n" + confess},
        {"stealer", you_shall_not_steal + "\n" + confess},
        {"roubo", you_shall_not_steal + "\n" + confess},
        {"roubar", you_shall_not_steal + "\n" + confess},
        {"ladrao", you_shall_not_steal + "\n" + confess},
        {"matar", you_shall_not_kill + "\n" + confess},
        {"murder", you_shall_not_kill + "\n" + confess},
        {"murderer", you_shall_not_kill + "\n" + confess},
        {"murderer", you_shall_not_kill + "\n" + confess},
        {"assassin", you_shall_not_kill + "\n" + confess},
        {"assassino", you_shall_not_kill + "\n" + confess},
        {"disease", be_healthy + "\n" + confess},
        {"doenca", be_healthy + "\n" + confess},
        {"infirmity", be_healthy + "\n" + confess},
        {"ilness", be_healthy + "\n" + confess},
        {"infirmities", be_healthy + "\n" + confess},
        {"enfermidade", be_healthy + "\n" + confess},
        {"enfermidades", be_healthy + "\n" + confess},
        {"rebellion", samuel_15_23 + "\n" + confess},
        {"rebelde", samuel_15_23 + "\n" + confess},
        {"witch", rev_22_15 + "\n" + confess},
        {"witches", rev_22_15 + "\n" + confess},
        {"wizard", rev_22_15 + "\n" + confess},
        {"bruxo", rev_22_15 + "\n" + confess},
        {"bruxaria", rev_22_15 + "\n" + confess},
        {"feitico", rev_22_15 + "\n" + confess},
        {"feiticaria", rev_22_15 + "\n" + confess},
        {"divination", rev_22_15 + "\n" + confess},
        {"adivinhar", rev_22_15 + "\n" + confess},
        {"magic", rev_22_15 + "\n" + confess},
        {"magics", rev_22_15 + "\n" + confess},
        {"magia", rev_22_15 + "\n" + confess},
        {"magica", rev_22_15 + "\n" + confess},
        {"magico", rev_22_15 + "\n" + confess},
        {"avenger", romans_12_19 + "\n" + confess},
        {"vingadores", romans_12_19 + "\n" + confess},
        {"vingar", romans_12_19 + "\n" + confess},
        {"vinganca", romans_12_19 + "\n" + confess},
        {"prostitute", be_pure + "\n" + confess},
        {"prostituta", be_pure + "\n" + confess},
        {"prostitution", be_pure + "\n" + confess},
        {"prostituicao", be_pure + "\n" + confess},
        {"adultery", be_pure + "\n" + confess},
        {"adulterio", be_pure + "\n" + confess},

        {"statue", no_statues + "\n" + confess},
        {"statues", no_statues + "\n" + confess},
        {"idol", no_statues + "\n" + confess},
        {"idolo", no_statues + "\n" + confess},

        {"beer", no_beer + "\n" + confess},
        {"liquor", no_beer + "\n" + confess},
        {"vodka", no_beer + "\n" + confess},
        {"cerveja", no_beer + "\n" + confess},
        {"cerveza", no_beer + "\n" + confess},
        {"licor", no_beer + "\n" + confess},
        {"cigarette", revelation_9_18 + "\n" + confess},
        {"cigarro", revelation_9_18 + "\n" + confess},
        {"fumo", revelation_9_18 + "\n" + confess},
        {"fumar", revelation_9_18 + "\n" + confess},
        {"maconha", revelation_9_18 + "\n" + confess},
        {"marijuana", revelation_9_18 + "\n" + confess},
        {"cannabis", revelation_9_18 + "\n" + confess},
        {"tobacco", revelation_9_18 + "\n" + confess},
        {"tabaco", revelation_9_18 + "\n" + confess},
        {"cocaine", revelation_9_18 + "\n" + confess},
        {"opium", revelation_9_18 + "\n" + confess},

        {"greed", timothy6_10 + "\n" + confess},
        {"ganancia", timothy6_10 + "\n" + confess},
        {"economy", timothy6_10 + "\n" + confess},  // Love for money
        {"economia", timothy6_10 + "\n" + confess}, // Love for money
        {"lust", romans6_23 + "\n" + confess},
        {"envy", prov_27_4 + "\n" + confess},
        {"anger", prov_27_4 + "\n" + confess},
        {"angry", prov_27_4 + "\n" + confess},
        {"gluttony", romans6_23 + "\n" + confess},
        {"glutton", romans6_23 + "\n" + confess},
        {"liar", romans6_23 + "\n" + confess}, // Social Engineering
        {"blackhat", romans6_23 + "\n" + confess},
        {"jealous", prov_27_4 + "\n" + confess},
        {"jealousy", prov_27_4 + "\n" + confess},
        {"fear", _2timothy1_7 + "\n" + confess},
        {"medo", _2timothy1_7 + "\n" + confess},
        {"pride", no_pride + "\n" + confess},
        {"orgulho", no_pride + "\n" + confess},
        {"lbgt", no_pride + "\n" + confess},

        {"gay", gen_2_24 + "\n" + confess},
        {"lesbian", gen_2_24 + "\n" + confess},
        {"lesbica", gen_2_24 + "\n" + confess},
        {"trans", gen_1_27 + "\n" + confess},
        {"homosexual", gen_2_24 + "\n" + confess},
        {"homossexual", gen_2_24 + "\n" + confess},
        {"transvestite", gen_1_27 + "\n" + confess},
        {"travesti", gen_1_27 + "\n" + confess},
        {"bisexual", gen_2_24 + "\n" + confess},
        {"bissexual", gen_2_24 + "\n" + confess},
        {"intersex", gen_1_27 + "\n" + confess},
        {"intersexo", gen_1_27 + "\n" + confess},
        {"queer", gen_2_24 + "\n" + confess},
        {"threesome", gen_2_24 + "\n" + confess},
        {"throuple", gen_2_24 + "\n" + confess},
        {"trisal", gen_2_24 + "\n" + confess},
        {"lgbt", gen_2_24 + "\n" + confess},

        {"buddha", _1John4_23 + "\n" + confess},
        {"buda", _1John4_23 + "\n" + confess},
        {"budism", _1John4_23 + "\n" + confess},
        {"budismo", _1John4_23 + "\n" + confess},
        {"candomble", _1John4_23 + "\n" + confess},
        {"umbanda", _1John4_23 + "\n" + confess},
        {"orixa", _1John4_23 + "\n" + confess},
        {"orixas", _1John4_23 + "\n" + confess},
        {"spiritism", _1John4_23 + "\n" + confess},
        {"spiritualism", _1John4_23 + "\n" + confess},
        {"espiritismo", _1John4_23 + "\n" + confess},
        {"espirita", _1John4_23 + "\n" + confess},

        {"python", gen3_14},
        {"rust", "James 5:3 — Your gold and your silver have rusted; and their rust will be a witness against you and will consume your flesh like fire. It is in the last days that you have stored up your treasure!"},
        {"dart", "Ephesians 6:16 — Above all, taking the shield of faith, with which you can extinguish all the fiery darts of the wicked."},
        {"mojo", rev_22_15}
    };

    static bool isUngodlyName(const std::string &name, std::string &outReason)
    {
        std::string token;
        std::stringstream ss(name);

        while (std::getline(ss, token, '_'))
        {
            std::string lower = token;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

            auto it = UNGODLY_WORDS.find(lower);
            if (it != UNGODLY_WORDS.end())
            {
                outReason = it->second;
                return true;
            }
        }

        return false;
    }

    enum class SymbolKind
    {
        Variable,
        Method,
        Class
    };

    static void handleUngodlyNaming(const std::string &msg) {
        if (terminal::supportsColor())
        {
            std::cerr
                << terminal::color::yellow
                << msg
                << terminal::color::reset
                << std::endl;
        }
        else
        {
            std::cerr << msg << std::endl;
        }
    }

}
