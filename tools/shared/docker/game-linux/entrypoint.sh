#!/usr/bin/env python3
import platform as platform_module

from shared.docker.game_entrypoint import main

# The image is shared between amd64 and arm64 (built via `docker build
# --platform`), so which one this container actually is gets picked up from
# uname rather than baked in.
is_arm64 = platform_module.machine() in ("aarch64", "arm64")
main(platform="linux-arm64" if is_arm64 else "linux")
