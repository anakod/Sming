#!/bin/bash
set -ex # exit with nonzero exit code if anything fails

sudo apt-get install -f -y g++-9-multilib linux-libc-dev:i386

if [ -f "$TRAVIS_BUILD_DIR/Sming/Arch/$SMING_ARCH/Tools/travis/install.sh" ]; then
	source "$TRAVIS_BUILD_DIR/Sming/Arch/$SMING_ARCH/Tools/travis/install.sh"
fi
