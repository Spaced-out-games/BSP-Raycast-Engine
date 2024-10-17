#pragma once

template<typename header_t, typename... Ts>
class Server;
class ent;
class client;
class ent_camera;
class ent_controller;
struct windowContent;
//typedef struct none_t {}; // Define an empty type

#define server_t Server<char, ent, ent_camera, ent_controller>


// Declare the static instance of server_t at the bottom of Server.h
