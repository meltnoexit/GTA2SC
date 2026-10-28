cmake_minimum_required(VERSION 4.3)
project(GTA2SC-AsiPlugin LANGUAGES C CXX RC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        MinHook
        GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
        GIT_TAG v1.3.4
)
FetchContent_Declare(
        xxHash
        GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
        GIT_TAG v0.8.4
        SOURCE_SUBDIR build/cmake
)

FetchContent_MakeAvailable(MinHook)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(XXHASH_BUILD_XXHSUM OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(xxHash)

add_library(${PROJECT_NAME} SHARED
        src/AsiPlugin.cpp
        src/hook/game/FontSymbol.cpp
        src/hook/game/FontSymbol.h
        src/hook/game/FrontendDestructorHook.cpp
        src/hook/game/FrontendDestructorHook.h
        src/hook/game/FrontendFontSelectionHook.cpp
        src/hook/game/FrontendFontSelectionHook.h
        src/hook/game/FrontendKanjiState.cpp
        src/hook/game/FrontendKanjiState.h
        src/hook/game/GameConstructorHook.cpp
        src/hook/game/GameConstructorHook.h
        src/hook/game/GbhDrawQuadHook.cpp
        src/hook/game/GbhDrawQuadHook.h
        src/hook/game/GbhFreePaletteHook.cpp
        src/hook/game/GbhFreePaletteHook.h
        src/hook/game/GbhFreeTextureHook.cpp
        src/hook/game/GbhFreeTextureHook.h
        src/hook/game/GbhTexture.h
        src/hook/game/GlyphWidthHook.cpp
        src/hook/game/GlyphWidthHook.h
        src/hook/game/Kanji.h
        src/hook/game/KanjiColor.cpp
        src/hook/game/KanjiColor.h
        src/hook/game/KanjiDefaultColorHook.cpp
        src/hook/game/KanjiDefaultColorHook.h
        src/hook/game/KanjiExplicitColorHook.cpp
        src/hook/game/KanjiExplicitColorHook.h
        src/hook/game/KanjiFontSelection.cpp
        src/hook/game/KanjiFontSelection.h
        src/hook/game/KanjiGlyphCache.cpp
        src/hook/game/KanjiGlyphCache.h
        src/hook/game/KanjiOutlineHook.cpp
        src/hook/game/KanjiOutlineHook.h
        src/hook/game/KanjiRendererContext.cpp
        src/hook/game/KanjiRendererContext.h
        src/hook/game/KanjiTextureBindingHook.cpp
        src/hook/game/KanjiTextureBindingHook.h
        src/hook/game/MilesAilStartupHook.cpp
        src/hook/game/MilesAilStartupHook.h
        src/hook/game/MissionTitleDrawHook.cpp
        src/hook/game/MissionTitleDrawHook.h
        src/hook/game/PlayerNameDrawHook.cpp
        src/hook/game/PlayerNameDrawHook.h
        src/hook/game/TextDrawHook.cpp
        src/hook/game/TextDrawHook.h
        src/hook/game/TextMeasureHook.cpp
        src/hook/game/TextMeasureHook.h
        src/hook/game/VehicleBannerDrawHook.cpp
        src/hook/game/VehicleBannerDrawHook.h
        src/hook/game/ZoneBannerDrawHook.cpp
        src/hook/game/ZoneBannerDrawHook.h
        src/hook/manager/LoadBitmapHook.cpp
        src/hook/manager/LoadBitmapHook.h
        src/hook/manager/LoadResourceHook.cpp
        src/hook/manager/LoadResourceHook.h
        src/hook/util/MinHookErrorLogger.cpp
        src/hook/util/MinHookErrorLogger.h
        src/hook/util/PatternScanner.cpp
        src/hook/util/PatternScanner.h
        src/util/ExecutableDetector.cpp
        src/util/ExecutableDetector.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
        src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        minhook
        xxHash::xxhash
)

target_sources(${PROJECT_NAME} PRIVATE
        res/DebugDialog.rc
        res/FlagBitmap.rc
        res/PhysicsDialog.rc
)

target_compile_options(${PROJECT_NAME} PRIVATE
        /utf-8
        /EHsc
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".asi"
)
