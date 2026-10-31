# --------------------------------------------------------------------------
# VS Code IntelliSense Configuration
#
# This file automatically configures the project's local VS Code settings
# (.vscode/settings.json) to enable C++ IntelliSense using CMake's compilation
# database.
#
# CMake generates `compile_commands.json`, which provides VS Code with the
# compiler flags, include paths, and definitions used by the project.
# This allows IntelliSense to correctly recognize project dependencies,
# including AsmJit classes under `src/third_party/asmjit`, providing
# accurate code completion and preventing false error warnings.
#
# The configuration is applied automatically when the main CMakeLists.txt
# includes this file through:
#
#     include(.config/cmake/configure_vscode_intellisense.cmake)
#
# The script creates `.vscode/settings.json` if it does not exist and adds
# the compilation database setting if it is missing.
#
# Existing VS Code settings are preserved. The `.vscode` directory is
# intentionally excluded from Git so developers can maintain their own
# editor configurations.
# --------------------------------------------------------------------------


# --------------------------------------------------------------------------
# Generate compile_commands.json for VS Code completion of asmjit::* classes
# --------------------------------------------------------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VSCODE_SETTINGS_DIR "${CMAKE_SOURCE_DIR}/.vscode")
set(VSCODE_SETTINGS_FILE "${VSCODE_SETTINGS_DIR}/settings.json")

file(MAKE_DIRECTORY "${VSCODE_SETTINGS_DIR}")

# ------------------------------
# Read existing VS Code settings
# ------------------------------
if(EXISTS "${VSCODE_SETTINGS_FILE}")
    file(READ "${VSCODE_SETTINGS_FILE}" VSCODE_SETTINGS)
else()
    set(VSCODE_SETTINGS "{}")
endif()

# ---------------------------------------------------------------
# Check if the existing VS Code settings file contains valid JSON
# ---------------------------------------------------------------
string(JSON VSCODE_SETTINGS_TYPE
    ERROR_VARIABLE VSCODE_INVALID_JSON
    TYPE "${VSCODE_SETTINGS}"
)

if(VSCODE_INVALID_JSON)
    message(WARNING
        "[Jesus] Could not read .vscode/settings.json as valid JSON. "
        "Keeping the existing file unchanged."
    )
elseif(NOT VSCODE_SETTINGS_TYPE STREQUAL "OBJECT")
    message(WARNING
        "[Jesus] .vscode/settings.json must contain a JSON object. "
        "Keeping the existing file unchanged."
    )
else()
    # ---------------------------------------------------
    # Check if the compileCommands setting already exists
    # ---------------------------------------------------
    string(JSON COMPILE_COMMANDS_SETTING
        ERROR_VARIABLE VSCODE_SETTING_MISSING
        GET "${VSCODE_SETTINGS}"
        "C_Cpp.default.compileCommands"
    )

    if(VSCODE_SETTING_MISSING)
        # -----------------------------------------------------------
        # Add code completion setting without removing other settings
        # -----------------------------------------------------------
        string(JSON VSCODE_SETTINGS
            SET "${VSCODE_SETTINGS}"
            "C_Cpp.default.compileCommands"
            "\"${CMAKE_BINARY_DIR}/compile_commands.json\""
        )

        file(WRITE "${VSCODE_SETTINGS_FILE}" "${VSCODE_SETTINGS}\n")

        message(NOTICE "   [Jesus] Added C++ IntelliSense configuration to VS Code settings.\n")
    endif()
endif()