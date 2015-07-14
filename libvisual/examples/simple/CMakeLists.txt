IF(HAVE_SDL)
  INCLUDE_DIRECTORIES(
    ${SDL_INCLUDE_DIR}
  )

  LINK_DIRECTORIES(${ORC_LIBRARY_DIRS})
  ADD_EXECUTABLE(simple
    main.cpp
    simple_example.cpp
  )

  TARGET_LINK_LIBRARIES(simple
    libvisual
    ${SDL_LIBRARY}
  )
ENDIF()
