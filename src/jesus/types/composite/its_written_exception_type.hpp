#pragma once

#include "class_type.hpp"
#include <string>
#include <exception>
#include <sstream>

/**
 * @brief The default exception type: ItsWritten
 *
 * "Jesus said to him, “Away from me, Satan! For it is written:
 *  ‘Worship the Lord your God, and serve him only.’” — Matthew 4:10
 */
class ItsWritten : public std::exception, public ClassType
{

public:
    const int code = 500;
    const std::string message;
    const std::string verseRef;
    const std::string verseText;
    mutable std::string formatted;

public:
    explicit ItsWritten(std::shared_ptr<CreationType> parent = nullptr)
        : ItsWritten(500, "", "1 Thessalonians 5:21", "Test everything; hold fast that which is good.", parent) {}

    /**
     * @brief 1. Construct a new ItsWritten exception
     *
     * @param message the error message
     */
    explicit ItsWritten(const std::string &message) : ItsWritten(500, message) {}

    /**
     * @brief 2. Construct a new ItsWritten 'exception' with: code + message
     *
     * @param code the HTTP status code
     * @param message the error message
     */
    ItsWritten(int code, const std::string &message)
        : ItsWritten(code, message, "1 Thessalonians 5:21", "Test everything; hold fast that which is good.") {}

    /**
     * @brief 3. Construct a new ItsWritten exception with Verse reference + verse text (without message)
     *
     * @param verseRef
     * @param verseText
     */
    ItsWritten(const std::string &verseRef, const std::string &verseText)
        : ItsWritten(500, "", verseRef, verseText) {}

    /**
     * @brief 4. Construct a new ItsWritten exception with: message + verse reference + verse text
     *
     * @param message the error msg
     * @param verseRef the verse reference. E.g.: 'Luke 10:19'
     * @param verseText the paraphrased verse
     */
    ItsWritten(const std::string &message,
               const std::string &verseRef,
               const std::string &verseText)
        : ItsWritten(500, message, verseRef, verseText) {}

    /**
     * @brief 5. Construct a new ItsWritten exception with: error code + message + verse ref + verse text
     *
     * @param code
     * @param message
     * @param verseRef
     * @param verseText
     */
    ItsWritten(int code,
               const std::string &message,
               const std::string &verseRef,
               const std::string &verseText,
               std::shared_ptr<CreationType> parent = nullptr)
        : ClassType(parent),
          code(code), message(message),
          verseRef(verseRef), verseText(verseText)
    {
    }

    const bool isException() const override
    {
        return true;
    }

    const char *what() const noexcept override
    {
        std::ostringstream oss;
        oss << "{code: " << code << ",\n";
        oss << " ref: \"" << verseRef << " — " << verseText << "\",\n";
        oss << " msg: \"" << message << "\"\n}";

        formatted = oss.str();
        return formatted.c_str();
    }
};
