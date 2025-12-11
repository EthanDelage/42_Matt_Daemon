add_executable(MattDaemon
        main.c
        Daemon.cpp
        Socket.cpp
        SocketServer.cpp
        SocketClient.cpp
        TintinReporter.cpp
        MattDaemon.cpp
)

target_link_libraries(MattDaemon
        PRIVATE
        compile_flags
)