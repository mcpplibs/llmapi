--add_rules("mode.debug", "mode.release")

add_requires("mbedtls 3.6.1")

--includes("src/json")

set_languages("c++23")

target("llmapi")
    --set_kind("moduleonly") -- link failed issue when other lib reference llmapi
    set_kind("static")
    add_files("src/*.cppm", { public = true, install = true })
    add_deps("tinyhttps")
    --add_deps("__nlohmann_json")
    add_includedirs("src/json")
    add_headerfiles("src/json/json.hpp")
    add_files("src/json/json.cppm", { public = true })

target("tinyhttps")
    set_kind("static")
    set_languages("c++23")
    set_policy("build.c++.modules", true)
    add_files("src/tinyhttps/*.cppm", { public = true })
    add_packages("mbedtls")

includes("examples")
includes("tests")