# basic-http-server

Minimal static HTTP server in C for experimenting with sockets and simple integration tests. The core request handling lives in [`src/main.c`](src/main.c), while reusable helpers (logging, MIME table, etc.) are defined in [`include/utils.h`](include/utils.h) and implemented in [`src/utils.c`](src/utils.c) via the [`extensions`](src/utils.c) lookup table.

## Features

- Serves files rooted at the working directory’s `static/` tree using the `web` request handler in [`src/main.c`](src/main.c).
- Logs to stdout/stderr through [`logger`](src/utils.c) and exposes a small helper (`writePortFile`) for writing the bound port to disk.
- Mozilla-style MIME resolution driven by [`extensions`](src/utils.c).
- CTest-powered smoke tests (`smoke.root`, `client.loopback`) defined in [`CMakeLists.txt`](CMakeLists.txt) to validate both curl access and the loopback client in [`src/test/client.c`](src/test/client.c).

## Requirements

- CMake ≥ 3.20 ([`CMakeLists.txt`](CMakeLists.txt))
- Ninja (or another CMake generator)
- A POSIX toolchain (tested with Apple Clang 17 on arm64)
- Optional: curl (for the `smoke.root` test)

## Building

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

or use presets from [`CMakePresets.json`](CMakePresets.json):

```sh
cmake --preset debug
cmake --build --preset debug
```

The main executable target is `basic-http-server` (see [`CMakeLists.txt`](CMakeLists.txt)).

## Running

```sh
./build/basic-http-server
```

By default the server listens on port 8080 and serves files from `static/`. Adjust logging or port handling directly in [`src/main.c`](src/main.c) if needed.

## Testing

With the server running (or by adapting the workflow used in `.github/workflows/ci.yaml`):

```sh
ctest --test-dir build --output-on-failure
```

- `smoke.root`: hits `http://127.0.0.1:8080/` via curl.
- `client.loopback`: runs the [`client_test`](src/test/client.c) executable to ensure the TCP path works end-to-end.

## Project layout

```
include/        Public headers (`utils.h`)
src/            Server implementation (`main.c`, `utils.c`)
src/test/       Loopback client used by CTest
static/         Sample site assets (e.g., `index.html`)
.github/        CI workflow (`workflows/ci.yaml`)
```

Happy hacking!
