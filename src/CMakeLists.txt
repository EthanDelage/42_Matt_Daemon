add_executable(MattDaemon
        main.cpp
        Daemon.cpp
        Socket.cpp
        SocketServer.cpp
        TintinReporter.cpp
        MattDaemon.cpp
)

target_link_libraries(MattDaemon
        PRIVATE
        compile_flags
)