--add_rules("mode.debug", "mode.release")

add_requires("libcurl 8.11.0")

--includes("src/json")

set_languages("c++23")

target("llmapi")
    --set_kind("moduleonly") -- link failed issue when other lib reference llmapi
    set_kind("static")
    add_files("src/*.cppm", { public = true, install = true })
    add_packages("libcurl")
    --add_deps("__nlohmann_json")
    add_includedirs("src/json")
    add_headerfiles("src/json/json.hpp")
    add_files("src/json/json.cppm", { public = true })

includes("examples")