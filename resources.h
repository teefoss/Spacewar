#ifndef resources_h
#define resources_h

#include <SDL2/SDL.h>
#include <unordered_map>

typedef enum
{
    RES_TEXTURE,
    RES_SOUND
} ResourceType;

typedef enum
{
    RES_TEXTURE_BLACKHOLE,
    RES_TEXTURE_SHIPS,
    RES_TEXTURE_BULLETS,
    RES_TEXTURE_POWERUPS,
    RES_TEXTURE_MISSLE,
    RES_COUNT
} ResourceID;

typedef struct
{
    ResourceID      id;
    ResourceType    type;
    const char *    file_name;
} ResourceInfo;

typedef struct
{
    void *          data;
    ResourceInfo *  info;
} Resource;

void    InitResources(SDL_Renderer * renderer);
void    FreeResources(void);
void *  GetResource(ResourceID id);
void    ReleaseResource(ResourceID id);

#endif /* resources_h */
