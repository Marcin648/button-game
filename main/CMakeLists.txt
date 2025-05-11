set(BUTTON_GAME_SRCS 
    "led.cpp"
    "button.cpp"
    "network.cpp"
)

idf_component_register(SRCS ${BUTTON_GAME_SRCS} INCLUDE_DIRS ".")

if(CONFIG_BUTTON_GAME_VARIANT_MASTER)
    target_sources(${COMPONENT_LIB} PRIVATE "master.cpp")
elseif(CONFIG_BUTTON_GAME_VARIANT_SLAVE)
    target_sources(${COMPONENT_LIB} PRIVATE "slave.cpp")
endif()
