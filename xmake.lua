set_languages("c++23")
set_policy("build.c++.modules", true)

add_requires("mbedtls 3.6.1")

target("tinyhttps")
    set_kind("static")
    add_files("src/tinyhttps/*.cppm", { public = true })
    add_packages("mbedtls", { public = true })

target("llmapi")
    set_kind("static")
    add_files("src/*.cppm", { public = true, install = true })
    add_files("src/providers/*.cppm", { public = true, install = true })
    add_deps("tinyhttps")
    add_includedirs("src/json")
    add_headerfiles("src/json/json.hpp")
    add_files("src/json/json.cppm", { public = true })

includes("examples")
includes("tests")
