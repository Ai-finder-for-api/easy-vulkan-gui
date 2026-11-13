# PLACEHOLDERS: set REPO to your GitHub repo, tag a release v0.1.0, then run `vcpkg install evg` once to get the real SHA512.
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO YOUR_GITHUB_USER/evg
    REF "v${VERSION}"
    SHA512 0
    HEAD_REF main
)
vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DEVG_BUILD_TESTS=OFF -DEVG_BUILD_EXAMPLES=OFF
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/evg)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
