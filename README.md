

1) including GameNetworkingSockets as submodule seems bad:
   Building GameNetworkingSockets with mingw occurs error cause one of winsock2's header (ws2def.h)
   is incomplete, there is even comments about it. But msvc's winsock2 has this file complete. Didnt check clang.