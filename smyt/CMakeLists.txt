cmake_minimum_required(VERSION 3.20)

add_executable(smyt
    "src/main.cpp"
)

# Linux only for now
target_link_options(smyt PRIVATE "-lpcap")
