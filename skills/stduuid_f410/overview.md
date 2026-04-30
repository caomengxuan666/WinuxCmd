# Overview

The `stduuid` library provides a C++ implementation of Universally Unique Identifiers (UUIDs) as specified in RFC 4122. It offers a header-only, modern C++ interface for generating, parsing, and manipulating UUIDs. The library supports multiple UUID versions including v1 (time-based), v4 (random), and v5 (SHA-1 based name UUIDs).

Use this library when you need to generate unique identifiers for distributed systems, database records, or any scenario requiring collision-resistant IDs. It's particularly useful for creating primary keys, session tokens, or correlation IDs. However, avoid using it when you need sequential or human-readable IDs, or when UUID length (36 characters) is prohibitive for your use case.

Key design features include: header-only implementation requiring no compilation, support for multiple UUID generators (system, random, name-based), standard C++ containers compatibility, and efficient byte-level operations. The library is designed for C++17 and later, leveraging modern features like `std::optional` and `std::array`.