cmake_minimum_required(VERSION 3.27)
project(2025_I2P2_TowerDefense)

set(CMAKE_CXX_STANDARD 17)

# Project & vendored headers
include_directories(
        ${CMAKE_SOURCE_DIR}                   # your own headers
        ${CMAKE_SOURCE_DIR}/Resource          # for <sha256/sha256.h>
        ${CMAKE_SOURCE_DIR}/Resource/include  # for <nlohmann/json.hpp>
)

# Copy assets & Allegro DLL
file(COPY
        Resource/audios
        Resource/fonts
        Resource/images
        allegro_monolith-5.2.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Define executable
add_executable(${PROJECT_NAME}
        Resource/sha256/sha256.cpp

        Enemy/Enemy.cpp
        Enemy/Enemy.hpp
        Enemy/Knight.cpp
        Enemy/Knight.hpp
        Enemy/Boss.cpp
        Enemy/Boss.hpp

        Engine/Allegro5Exception.cpp
        Engine/AudioHelper.cpp
        Engine/Collider.cpp
        Engine/GameEngine.cpp
        Engine/Group.cpp
        Engine/IControl.cpp
        Engine/IObject.cpp
        Engine/IScene.cpp
        Engine/LOG.cpp
        Engine/Point.cpp
        Engine/Resources.cpp
        Engine/Sprite.cpp
        Engine/Sheets.cpp
        Engine/map.hpp
        Engine/map.cpp
        Engine/utility.hpp
        Engine/Location.cpp
        Engine/Location.hpp
        Engine/ParallaxCloud.cpp
        Engine/ParallaxCloud.hpp
        Engine/Chat.cpp

        Scene/LoseScene.cpp
        Scene/PlayScene.cpp
        Scene/StageSelectScene.cpp
        Scene/StartScene.cpp
        Scene/WinScene.cpp
        Scene/IntroScene.cpp
        Scene/SettingsScene.cpp
        Scene/Boarding.cpp
        Scene/Login.cpp
        Scene/Signup.cpp
        Scene/friendlist.cpp
        Scene/friendlist.hpp
        Scene/search.cpp
        Scene/search.hpp
        Scene/requests.hpp
        Scene/requests.cpp
        Scene/leaderboard.cpp

        Player/Player.cpp
        Player/Player.hpp
        Player/coin.cpp

        Shop/Shop.cpp
        Shop/item.cpp

        UI/Animation/DirtyEffect.cpp
        UI/Animation/ExplosionEffect.cpp

        UI/Component/Image.cpp
        UI/Component/ImageButton.cpp
        UI/Component/Label.cpp
        UI/Component/Slider.cpp

        main.cpp
        Engine/ParallaxBackground.hpp
        Engine/ParallaxBackground.cpp
        Engine/Location.hpp
        Engine/Location.cpp
        Engine/ParallaxCloud.hpp
        Engine/ParallaxCloud.cpp
)

# Vendored libcurl
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/Resource/curl/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/Resource/curl/lib/libcurl.dll.a
)

# Copy whichever DLL matches at configure time
file(GLOB CURL_DLL
        "${CMAKE_SOURCE_DIR}/Resource/curl/bin/libcurl*.dll"
)
if (CURL_DLL)
    get_filename_component(DLL_NAME "${CURL_DLL}" NAME)
    message(STATUS "Vendoring libcurl DLL: ${DLL_NAME}")
    configure_file(
            "${CURL_DLL}"
            "${CMAKE_CURRENT_BINARY_DIR}/${DLL_NAME}"
            COPYONLY
    )
else()
    message(WARNING "No curl DLL found in Resource/curl/bin — copy it manually")
endif()

# Allegro integration
if (WIN32)
    target_include_directories(${PROJECT_NAME} PUBLIC C:/allegro/include)
    target_link_directories     (${PROJECT_NAME} PUBLIC C:/allegro/lib)
    target_compile_options      (${PROJECT_NAME} PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries       (${PROJECT_NAME} PRIVATE liballegro_monolith)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALLEGRO5 REQUIRED
            allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5
            allegro_primitives-5 allegro_image-5 allegro_ttf-5
            allegro_audio-5 allegro_acodec-5 allegro_shader-5
    )
    target_include_directories(${PROJECT_NAME} PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
    target_compile_options     (${PROJECT_NAME} PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries      (${PROJECT_NAME} PRIVATE ${ALLEGRO5_LIBRARIES})
    target_link_directories    (${PROJECT_NAME} PUBLIC ${ALLEGRO5_LIBRARY_DIRS})
endif()
