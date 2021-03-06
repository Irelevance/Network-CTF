#ifndef PACKETIDENTIFICATION_H_INCLUDED
#define PACKETIDENTIFICATION_H_INCLUDED

///id of all packets that can be exchanged between the client and server
///how a packet is processed is dependant on the id of the packet

extern const int PLAYER_INPUT;
extern const int PLAYER_KEYSTATE_UPDATE;
extern const int PLAYER_STATE_UPDATE;

//packet id for packets containing information about the other players connected to the server, does not contain info about the user's player
extern const int PLAYER_INTERPOLATION_INFO;

extern const int PLAYER_GUNFIRE;

//empty packets exchanged to test latency between server and client
extern const int LATENCY_TEST;

//empty packets used to attempt to connect to the server, if a response is received then client is connected
//response to this connection will be another connection attempt
extern const int CONNECTION_ATTEMPT;

extern const int WORLD_STATE_UPDATE;

extern const int LOBBY_CONNECTION_INFO;

extern const int START_GAME;

extern const int CHANGE_TEAM;

extern const int HEART_BEAT;

extern const int VICTORY;

extern const int DEFEAT;

//packet clients send to server to let the server know that the player is currently in the victory or defeat screen
extern const int AT_END_SCREEN;

extern const int CLIENT_DAMAGE_REPORT;

#endif // PACKETIDENTIFICATION_H_INCLUDED
