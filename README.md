# MENG
A physics and rendering engine simulating Newtonian physics I created for my A-Level Computer Science coursework.
It's called MENG (mechanics engine).
The interface is a wide pannable open space where you can drag and drop simple shapes onto it. 
These shapes can be resized, changed in colour, as well as having quantifiable data about
them changed, such as mass, velocity, acceleration, etc.
A simulation can be executed, where the objects will interact via Newtonian mechanics.
A file can be saved under extension .MENG, which is just an SQLite database.

This was made using SDL2 and the WINAPI, so will only run under Windows.
Still need to add a makefile, but one can get this up and running with g++
and adding linker flags for SDL2, SDL_image exetension library and SDL true type fonts.
