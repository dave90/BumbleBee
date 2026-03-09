# Linux Build (Docker)

Cross-compile BumbleBee on macOS using a Linux container with Clang 19 (same as Google Colab).

## Build the Docker image (one-time)

```bash
docker build -t bumblebee-linux docker/linux
```

## Compile BumbleBee

```bash
docker run --rm -v $(pwd):/workspace bumblebee-linux
```

Build output goes to `cmake-build-linux/` in your repo directory.

## Interactive shell

```bash
docker run --rm -it -v $(pwd):/workspace bumblebee-linux bash
```