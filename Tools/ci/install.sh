#!/bin/bash
#
# CI installer script
#
# May also be used for library CI builds. The following variables must be set:
#
#   CI_BUILD_DIR
#   SMING_HOME
#
set -ex

if [ -z "$SMING_TOOLS_PREINSTALLED" ]; then

INSTALL_OPTS=(fonts)
if [ "$SMING_ARCH" = "Host" ]; then
    INSTALL_OPTS+=(optional)
fi

# Ensure default path is writeable
sudo chown "$USER" /opt

"$CI_BUILD_DIR/Tools/install.sh" "$SMING_ARCH" "${INSTALL_OPTS[@]}"

fi

# Configure ccache
ccache --set-config cache_dir="$CI_BUILD_DIR/.ccache"
ccache --set-config max_size=500M
ccache -z
if [ "$(uname)" == "Darwin" ]; then
    ccache --set-config compiler_check=content
fi

# Clean up tools installation
source "$CI_BUILD_DIR/Tools/export.sh"
python "$CI_BUILD_DIR/Tools/ci/clean-tools.py" clean --delete
