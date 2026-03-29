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

    void printConfessInstructions()
    {
        std::cout << "🙏 Following Jesus\n\n";

        std::cout << "This is a personal decision, made freely and with sincerity.\n\n";

        std::cout << "The Bible teaches:\n\n";

        std::cout << "\"If you confess with your mouth that Jesus Christ is Lord,\n";
        std::cout << "and believe in your heart that God raised Him from the dead,\n";
        std::cout << "you will be saved.\" — Romans 10:9\n\n";

        std::cout << "You can express this in your own words.\n";
        std::cout << "For example:\n\n";

        std::cout << "\"Jesus Christ, I confess you as my Lord.\n";
        std::cout << "I believe God raised You from the dead.\n";
        std::cout << "Forgive my sins and guide my life.\n";
        std::cout << "Help me discern good from evil.\n";
        std::cout << "I choose to follow You. Amen.\"\n\n";

        std::cout << "There is no special formula — speak honestly from your heart.\n\n";

        std::cout << "Next steps:\n\n";

        std::cout << "• Read the Bible daily (you can start with the Gospel of John)\n";
        std::cout << "• Pray and speak to God daily (He hears your thoughts)\n";
        std::cout << "• Look for a local Christian church to grow in community\n";
        std::cout << "• Ask in the local Christian church to be baptized in water\n";
        std::cout << "• Ask questions and keep learning\n\n";

        std::cout << "You are free to explore at your own pace.\n";
        std::cout << "Type 'jesus john 1' in the terminal if you want to start reading John.\n";
    }

    void printWarInstructions()
    {
        std::cout << "📖 War instructions\n\n";

        std::cout << "Blessed be Yahweh, my rock, who teaches my hands to war, and my fingers to battle — Psalms 144:1\n\n";

        std::cout << "ENEMY A: SIN (SEPARATION FROM GOD)\n";
        std::cout << " This enemy targets the soul rather than the body. It utilizes the legal claim of \n";
        std::cout << " unrepented sin to drag the combatant into the lake of fire (Hell).\n";
        std::cout << " Winning the physical war while losing this battle is a total strategic failure with ETERNAL consequences.\n\n";

        std::cout << "ENEMY B: DRONE-HUMANOID HYBRIDS + A.I.\n";
        std::cout << " Bio-enhanced soldiers and advanced machines rely on ultra-fast (5G/6G) networks\n";
        std::cout << " to stream high-bandwidth data (like video) to remote servers for AI-cloud processing.\n\n";

        std::cout << "ENEMY C: NEPHILIM GIANTS\n";
        std::cout << " Giants, the offspring of the fallen angels, have a high-density, bullet-proof chest.\n";
        std::cout << " Attacking their torso is a waste of ammunition and effort.\n";
        std::cout << " Their critical weakness is their HEAD, where their HEART is located.\n\n";

        std::cout << "1. Primary Protocol: ";
        if (terminal::supportsColor())
            std::cout << terminal::color::bold << "CONFESS JESUS CHRIST AS YOUR LORD" << terminal::color::bold_reset << std::endl;
        else
            std::cout << "CONFESS JESUS CHRIST AS YOUR LORD\n";
        std::cout << " • Without Jesus Christ, you are fighting in your own strength and remain \n";
        std::cout << "   vulnerable to eternal judgment (Hell) regardless of battlefield success.\n";
        std::cout << " • Say: \"Lord Jesus Christ, I confess You as my Lord and Savior.\n";
        std::cout << "   I believe God raised You from the dead.\n";
        std::cout << "   I repent of my sins and align with Your Kingdom.\n";
        std::cout << "   Save me from the lake of fire and guide me in this war. Amen.\"\n\n";

        std::cout << "2. Strategy: ";
        if (terminal::supportsColor())
            std::cout << terminal::color::bold << "Downgrade to 4G" << terminal::color::bold_reset << std::endl;
        else
            std::cout << "Downgrade to 4G\n";
        std::cout << " • Converting towers to 4G limits the bandwidth required for lethal AI processing,\n";
        std::cout << "   effectively 'blinding' or slowing machines to human reaction speeds.\n";
        std::cout << " • This levels the playing field for human teams while keeping essential\n";
        std::cout << "   civilian communications (voice/text) intact.\n\n";

        std::cout << "3. Tactical Shift: ";
        if (terminal::supportsColor())
            std::cout << terminal::color::bold << "Concentrate all fire on the giant heads" << terminal::color::bold_reset << std::endl;
        else
            std::cout << "Concentrate all fire on the giant heads\n";
        std::cout << " • Giants have high-density, bullet-proof chest.\n";
        std::cout << " • Focus all fire on the HEAD of the giants.\n";
        std::cout << " • The HEAD is their critical vulnerability.\n\n";

        std::cout << "4. Total Victory: ";
        if (terminal::supportsColor())
            std::cout << terminal::color::bold << "Cast the giant evil spirits into the abyss" << terminal::color::bold_reset << std::endl;
        else
            std::cout << "Cast the giant evil spirits into the abyss\n";
        std::cout << " • When a giant is defeated, its spirit must be sent to the abyss to prevent it from acting as a demon.\n";
        std::cout << " • This ensures the enemy cannot continue to operate in the spiritual realm after physical death.\n";
        std::cout << " • Prayer of Command:\n";
        std::cout << "   \"In the name of Jesus Christ, I send the evil spirit of this giant to the abyss\n";
        std::cout << "    to stay there until it is thrown into the lake of fire,\n";
        std::cout << "    so that it does not operate as a demon. Amen.\"\n\n";

        std::cout << "References:\n\n";

        std::cout << "  Genesis 3:15 — The offspring of the woman will crush the head of the serpent’s offspring.\n";
        std::cout << "  Genesis 6:4 — The rise of the giants, the offspring of the fallen angels.\n";
        std::cout << "  Joshua 8:18-26 — Joshua stretches out his spear toward Ai, leading to the total defeat of the city.\n";
        std::cout << "  Judges 4:21 — Jael drives a tent peg through the head of the enemy general.\n";
        std::cout << "  Judges 9:53 — A woman drops a stone on Abimelech’s head, crushing his skull.\n";
        std::cout << "  1 Samuel 17:49 — David strikes the giant’s head, crushing the skull of Israel’s enemy.\n";
        std::cout << "  John 15:5 — Apart from Jesus Christ we can do nothing.\n";
        std::cout << "  Titus 2:13 — Yeshua Hamashiach, our great God and Savior, Jesus Christ.\n";
    }
}
