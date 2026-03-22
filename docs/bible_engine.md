# Bible Engine Design

## Overview

This project implements a **compressed Bible engine** designed for:

- Fast verse lookup (O(1))
- Small binary size
- Low RAM usage

Instead of storing the Bible as plain text, the system uses:

- Dictionary-based compression
- Bit-packed token streams
- Precomputed offsets for direct indexing

This allows efficient access to any verse without decompressing the entire dataset.

---

## Goals

- Keep the final binary **small**
- Minimize **runtime memory usage**
- Enable **direct verse access** without parsing full text
- Avoid dynamic allocations where possible

---

## Trade-offs

We explicitly chose to include **only the English Bible**.

Including both English and Spanish (PR #67) resulted in:

| Metric | Before | After |
|------|--------|-------|
| Binary size | 3.6MB → 16MB | ❌ too large |
| RAM usage | 2.4MB → 28MB | ❌ too heavy |

Current design (English only):

| Metric | Value |
|------|--------|
| Binary size | ~5.5MB |
| RAM usage | ~5MB |

This is the optimal balance for performance and footprint.

---

## High-Level Architecture

The system is composed of three main parts:

### 1. Compression Tool

- Source:
  `src/bible/tools/bible_compressor.cpp`

- Responsibility:
  - Reads raw Bible data (`src/bible/data/en/*.hpp`)
  - Builds:
    - dictionary
    - token stream
    - offsets
  - Outputs:
    - `compressed_bible.hpp`

---

### 2. Generated Data

- Output file:
  `src/bible/include/bible/compressed_bible.hpp`

Contains:

- `DICTIONARY_DATA` → all words (null-terminated)
- `DICTIONARY_OFFSETS` → word index
- `TOKEN_STREAM` → bit-packed tokens
- `TOKEN_BITS` → bits per token
- `VERSE_OFFSETS` → verse boundaries
- `CHAPTER_OFFSETS` → chapter boundaries
- `BOOK_OFFSETS` → book boundaries
- `CHAPTER_COUNT` → chapters per book
- `BOOK_CHAPTER_INDEX` → mapping helper

This file is **fully generated** — do not edit manually.

---

### 3. Runtime Reader (Decompression)

- Source:
  `src/jesus/cli/bible.cpp`

Responsibilities:

- Parse CLI input (e.g. `jesus john 3:16`)
- Resolve:
  - book
  - chapter
  - verse(s)
- Read tokens from `TOKEN_STREAM`
- Convert tokens → words via dictionary
- Build final verse string

---

## Compression Model

### Dictionary

All unique words are stored once:

```

"the\0beginning\0God\0created\0..."

```

Each word is referenced by an index (`uint16_t`).

---

### Token Stream

Verses are stored as sequences of dictionary indices:

```

[12, 45, 78, ...]

````

These indices are **bit-packed** using `TOKEN_BITS`.

Example:
- If 4096 words → 12 bits per token

This reduces memory usage significantly.

---

### Verse Representation

A verse is not stored directly.

Instead:

- `VERSE_OFFSETS[id]` → start token index
- next offset → end

---

## Indexing System

The system is entirely index-based.

### Book → Chapter → Verse resolution

```cpp
getVerseId(book, chapter, verse)
````

Internally:

```
bookStart
+ chapterStart
+ (verse - 1)
```

---

### Offsets

| Array             | Meaning                     |
| ----------------- | --------------------------- |
| `BOOK_OFFSETS`    | first verse of each book    |
| `CHAPTER_OFFSETS` | first verse of each chapter |
| `VERSE_OFFSETS`   | first token of each verse   |

---

## Verse Lookup Flow

1. Parse input:

   ```
   jesus rev 22:21
   ```

2. Resolve:

   * book = Revelation
   * chapter = 22
   * verse = 21

3. Compute:

   ```
   verseID = getVerseId(...)
   ```

4. Get token range:

   ```
   start = VERSE_OFFSETS[verseID]
   end   = VERSE_OFFSETS[verseID + 1]
   ```

5. Read tokens:

   ```
   readBibleToken(...)
   ```

6. Convert tokens → words:

   ```
   getWordFromDictionary(...)
   ```

7. Concatenate result

---

## Critical Invariants ⚠️

These must **always hold true**:

* Offsets must be **strictly increasing**
* `VERSE_OFFSETS` must align with `TOKEN_STREAM`
* `TOKEN_BITS` must match dictionary size
* `getVerseId()` must not overflow boundaries

---

## Boundary Safety (Important)

Because the system is **index-based**, invalid references can read incorrect data.

### Problem

Without validation:

```
rev 22:23
```

Would read into the next chapter/book.

---

### Solution

We enforce:

```cpp
chapterExists(book, chapter)
verseExists(book, chapter, verse)
```

These checks are **mandatory before calling `getVerseId()`**.

---

### Rule

> Never call `getVerseId()` without validating chapter and verse.

---

## Known Pitfalls

### 1. Invalid verse reads garbage

Cause:

* No boundary validation

Fix:

* Always call `verseExists()`

---

### 2. Invalid chapter reads garbage

Cause:

* `getVerseId()` still returns a valid index

Fix:

* Always call `chapterExists()`

---

### 3. Offset mismatch bugs

Cause:

* Incorrect generation of offsets

Symptom:

* Wrong verses returned

Fix:

* Regenerate `compressed_bible.hpp`

---

## Build Pipeline

Defined in:

```
CMakeLists.txt
```

---

### Flow (dependency chain)

```
jesus (executable)
  ↓
generate_compressed_bible (custom target)
  ↓
compressed_bible.hpp (generated file)
  ↓
bible_compressor (tool)
  ↓
src/bible/data/en/*.hpp (input data)
```

---

### Key Steps

#### 1. Build compressor

```
add_executable(bible_compressor ...)
```

#### 2. Generate header

```
add_custom_command(
    OUTPUT compressed_bible.hpp
    COMMAND bible_compressor ...
    DEPENDS bible_compressor + bible data
)
```

#### 3. Create target

```
add_custom_target(generate_compressed_bible ...)
```

#### 4. Ensure dependency

```
add_dependencies(jesus generate_compressed_bible)
```

---

### Result

* The Bible is **recompressed automatically** when:

  * data changes
  * compressor changes

---

## CLI Integration

Entry point:

```
src/jesus/cli/bible.cpp
```

Example:

```
jesus gen 1:3
jesus rev 22:21
```

Flow:

1. Parse input
2. Resolve reference
3. Validate
4. Read verses
5. Print output

---

## Design Philosophy

This system is:

* ❌ Not a generic compression engine
* ✅ A **Bible-specific optimized format**

It leverages:

* Known structure (books, chapters, verses)
* Static data
* Precomputed indices

---

## Future Improvements

* Precomputed `VERSES_PER_CHAPTER` table (faster validation)
* `string_view` instead of `std::string` (less allocation)
* Optional strict mode (error reporting)
* Range clamping (`rev 22:20-30 → 20-21`)

---

## Final Notes

This system behaves like a:

> **Compressed, index-driven text database**

All correctness depends on:

* Proper offset generation
* Strict boundary validation

If something looks wrong:

Check offsets first.
