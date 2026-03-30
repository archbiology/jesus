#include <iostream>
#include "../utils/terminal/terminal.hpp"
#include "../utils/terminal/color.hpp"

namespace HelpCLI
{
    void printGeneralHelp()
    {
        std::cout << "Jesus Programming Language CLI\n\n";

        std::cout << "Usage:\n";
        std::cout << "  jesus <file>.jesus\n";
        std::cout << "  jesus <command>\n";
        std::cout << "  jesus <book> <reference>\n\n";

        std::cout << "Available Commands:\n"
                  << "  bible    - Instructions for reading the Holy Bible\n"
                  << "  confess  - Guidance on professing faith in Jesus Christ\n"
                  << "  giants   - Strategies for warfare: bio-enhanced soldiers, AI, giants, and sin.\n\n";

        std::cout << "Help:\n";
        std::cout << "  --help | -h | /? | help\n\n";

        std::cout << "Examples:\n";
        std::cout << "  jesus genesis 1:1\n";
        std::cout << "  jesus john 3:16\n";
        std::cout << "  jesus psalm 23\n\n";

        std::cout << "Run 'jesus bible' for full Bible usage and supported formats.\n\n";
    }

    std::string getConfessInstructions()
    {
        return "🙏 Following Jesus\n\n"

               "This is a personal decision, made freely and with sincerity.\n\n"

               "The Bible teaches:\n\n"

               "\"If you confess with your mouth that Jesus Christ is Lord,\n"
               "and believe in your heart that God raised Him from the dead,\n"
               "you will be saved.\" — Romans 10:9\n\n"

               "You can express this in your own words.\n"
               "For example:\n\n"

               "\"Jesus Christ, I confess you as my Lord.\n"
               "I believe God raised You from the dead.\n"
               "Forgive my sins and guide my life.\n"
               "Help me discern good from evil.\n"
               "I choose to follow You. Amen.\"\n\n"

               "There is no special formula — speak honestly from your heart.\n\n"

               "Next steps:\n\n"

               "• Read the Bible daily (you can start with the Gospel of John)\n"
               "• Pray and speak to God daily (He hears your thoughts)\n"
               "• Look for a local Christian church to grow in community\n"
               "• Ask in the local Christian church to be baptized in water\n"
               "• Ask questions and keep learning\n\n"

               "You are free to explore at your own pace.\n"
               "Type 'jesus john 1' in the terminal if you want to start reading John.";
    }

    std::string maybeBold(std::string text) {
        if (terminal::supportsColor())
            return terminal::color::bold + text + terminal::color::bold_reset;

        return text;
    }

    std::string getWarInstructions()
    {
        return "📖 War instructions\n\n"

            "Blessed be Yahweh, my rock, who teaches my hands to war, and my fingers to battle — Psalms 144:1\n\n"

            "ENEMY A: SIN (SEPARATION FROM GOD)\n"
            " This enemy targets the soul rather than the body. It utilizes the legal claim of \n"
            " unrepented sin to drag the combatant into the lake of fire (Hell).\n"
            " Winning the physical war while losing this battle is a total strategic failure with ETERNAL consequences.\n\n"

            "ENEMY B: DRONE-HUMANOID HYBRIDS + A.I.\n"
            " Bio-enhanced soldiers and advanced machines rely on ultra-fast (5G/6G) networks\n"
            " to stream high-bandwidth data (like video) to remote servers for AI-cloud processing.\n\n"

            "ENEMY C: NEPHILIM GIANTS\n"
            " Giants, the offspring of the fallen angels, have a high-density, bullet-proof chest.\n"
            " Attacking their torso is a waste of ammunition and effort.\n"
            " Their critical weakness is their HEAD, where their HEART is located.\n\n"

            "1. Primary Protocol: " + maybeBold("CONFESS JESUS CHRIST AS YOUR LORD") + "\n"
            " • Without Jesus Christ, you are fighting in your own strength and remain \n"
            "   vulnerable to eternal judgment (Hell) regardless of battlefield success.\n"
            " • Say: \"Lord Jesus Christ, I confess You as my Lord and Savior.\n"
            "   I believe God raised You from the dead.\n"
            "   I repent of my sins and align with Your Kingdom.\n"
            "   Save me from the lake of fire and guide me in this war. Amen.\"\n\n"

            "2. Strategy: " + maybeBold("Downgrade to 4G") + "\n"
            " • Converting towers to 4G limits the bandwidth required for lethal AI processing,\n"
            "   effectively 'blinding' or slowing machines to human reaction speeds.\n"
            " • This levels the playing field for human teams while keeping essential\n"
            "   civilian communications (voice/text) intact.\n\n"

            "3. Tactical Shift: "  + maybeBold("Concentrate all fire on the giant heads") + "\n"
            " • Giants have high-density, bullet-proof chest.\n"
            " • Focus all fire on the HEAD of the giants.\n"
            " • The HEAD is their critical vulnerability.\n\n"

            "4. Total Victory: " + maybeBold("Cast the giant evil spirits into the abyss") + "\n"
            " • When a giant is defeated, its spirit must be sent to the abyss to prevent it from acting as a demon.\n"
            " • This ensures the enemy cannot continue to operate in the spiritual realm after physical death.\n"
            " • Prayer of Command:\n"
            "   \"In the name of Jesus Christ, I send the evil spirit of this giant to the abyss\n"
            "    to stay there until it is thrown into the lake of fire,\n"
            "    so that it does not operate as a demon. Amen.\"\n\n"

            "References:\n\n"

            "  Genesis 3:15 — The offspring of the woman will crush the head of the serpent’s offspring.\n"
            "  Genesis 6:4 — The rise of the giants, the offspring of the fallen angels.\n"
            "  Joshua 8:18-26 — Joshua stretches out his spear toward Ai, leading to the total defeat of the city.\n"
            "  Judges 4:21 — Jael drives a tent peg through the head of the enemy general.\n"
            "  Judges 9:53 — A woman drops a stone on Abimelech’s head, crushing his skull.\n"
            "  1 Samuel 17:49 — David strikes the giant’s head, crushing the skull of Israel’s enemy.\n"
            "  John 15:5 — Apart from Jesus Christ we can do nothing.\n"
            "  Titus 2:13 — Yeshua Hamashiach, our great God and Savior, Jesus Christ.";
    }
}
