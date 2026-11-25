# Лабораторная №7 «UDP и TCP сокеты»

add_executable(lab7_udp_server udp_server.cpp)
target_link_libraries(lab7_udp_server PRIVATE ws2_32)

add_executable(lab7_udp_client udp_client.cpp)
target_link_libraries(lab7_udp_client PRIVATE ws2_32)

add_executable(lab7_tcp_server tcp_server.cpp)
target_link_libraries(lab7_tcp_server PRIVATE ws2_32)

add_executable(lab7_tcp_client tcp_client.cpp)
target_link_libraries(lab7_tcp_client PRIVATE ws2_32)
