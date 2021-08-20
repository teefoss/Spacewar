#include "resources.h"
#include "log.h"
#include "defines.h"
#include "storage.h"

#include <SDL2/SDL_image.h>
#include <stdlib.h>

#define GRAPHICS_PATH       "graphics/"
#define RESOURCE_NOT_FOUND  -1
#define RES_DEBUG           0

ResourceInfo info[RES_COUNT] = {
    { RES_TEXTURE_BLACKHOLE, RES_TEXTURE, GRAPHICS_PATH"black_hole.png" },
    { RES_TEXTURE_SHIPS,     RES_TEXTURE, GRAPHICS_PATH"ships.png" },
    { RES_TEXTURE_BULLETS,   RES_TEXTURE, GRAPHICS_PATH"bullets.png" },
    { RES_TEXTURE_POWERUPS,  RES_TEXTURE, GRAPHICS_PATH"powerups.png" },
    { RES_TEXTURE_MISSLE,    RES_TEXTURE, GRAPHICS_PATH"missle.png" },
};

static Storage<Resource> resources;
static int ref_counts[RES_COUNT];
static SDL_Renderer * resource_renderer;


void InitResources(SDL_Renderer * renderer)
{
    resource_renderer = renderer;
}


static void FreeResource(Resource * resource)
{
    switch (resource->info->type ) {
        case RES_TEXTURE:
            SDL_DestroyTexture((SDL_Texture *)resource->data);
            resource->data = NULL;
            break;
        case RES_SOUND:
            break;
        default:
            break;
    }
}


void FreeResources()
{
    Resource * r = resources;
    for ( int i = 0; i < resources.Count(); i++, r++ ) {
        --ref_counts[r->info->id];
        FreeResource(r);
    }
    
    resources.Empty();
}


static int FindResource(ResourceID id)
{
    for ( int i = 0; i < resources.Count(); i++ ) {
        if ( resources[i].info->id == id ) {
            return i;
        }
    }
    
    return RESOURCE_NOT_FOUND;
}


void * GetResource(ResourceID id)
{
    int i = FindResource(id);
        
    if ( i == RESOURCE_NOT_FOUND ) {

        Resource resource;
        resource.info = &info[id];
        
        // load the resource
        switch ( info[id].type ) {
            case RES_TEXTURE:
                resource.data
                    = IMG_LoadTexture(resource_renderer, info[id].file_name);
                break;
            case RES_SOUND:
                // TODO: load sound
                break;
            default:
                ERROR("GetResource: strange resource type!\n");
                break;
        }
        
        resources.Insert(resource);
        i = resources.Count() - 1;
    }
    
    ref_counts[i]++;
    return resources[i].data;
}


void ReleaseResource(ResourceID id)
{
    int i = FindResource(id);
    
    if ( i == RESOURCE_NOT_FOUND ) {
        ERROR("tried to release unloaded resource!\n"); // TEMP: debug
    } else {
        --ref_counts[id];
        if ( ref_counts[id] == 0 ) {
            FreeResource(resources.GetPointer(i));
            resources.Remove(i);
        }
    }
}
