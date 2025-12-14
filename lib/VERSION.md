# Vendored Dependencies

This directory contains vendored (embedded) third-party libraries.

## Libraries

### SQLiteCpp (3.3.3)
- **Source:** https://github.com/SRombauts/SQLiteCpp
- **Tag:** 3.3.3
- **Commit:** 59a047b8d3fe8574406ed73ab9fac0474e87bd03
- **License:** MIT
- **Purpose:** C++ wrapper for SQLite database
- **Note:** Uses original CMakeLists.txt from upstream

## Updating Dependencies

To update a vendored library:

1. Remove the old directory: `rm -rf lib/<library>/`
2. Clone the new version: `git clone --depth 1 --branch <TAG> <URL> lib/<library>/`
3. Remove git metadata: `cd lib/<library> && rm -rf .git .gitignore`
4. For Argon2: Copy `CMakeLists.txt` from this commit
5. Test build: `ninja -C build`
6. Update this VERSION file with new tag/commit

## Why Vendored?

Vendoring dependencies provides:
- **Reproducible builds** - Same source every time
- **No network dependency** - Builds work offline
- **Version pinning** - Control when to update
- **Simplified setup** - No package manager required
