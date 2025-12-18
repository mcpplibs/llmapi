set_languages("cxx23")

target("__nlohmann_json")
    set_kind("static")
    add_includedirs(".")
    add_headerfiles("json.hpp")
    add_files("json.cppm", { public = true })