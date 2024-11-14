cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 23)
project(algo)
add_executable(
        algo
        main.c
        list.c
        hashmap.c
        hashset.c
        stack.c
        heap.c
        graph.c
        queue.c
    )
