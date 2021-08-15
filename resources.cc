#include "resources.h"
#include "log.h"

#include <SDL2/SDL_image.h>
#include <stdlib.h>

ResourceManager::~ResourceManager()
{
    for ( auto it : m_textures ) {
        SDL_DestroyTexture(it.second);
    }
}


void ResourceManager::Init(SDL_Renderer * renderer)
{
    m_renderer = renderer;
}


SDL_Texture *
ResourceManager::GetTexture(const char * file_name)
{
    auto element = m_textures.find(file_name);
    
    if ( element != m_textures.end() ) {
        // found it
        auto tracker = m_tracker.find(file_name);
        ++tracker->second;
        LOG("GetTexture: %s already exists (count is now %d)\n", file_name, tracker->second);
        return element->second;
    } else { // does not exist, load it
        // append directory name
        LOG("GetTexture: %s not found, loading...\n", file_name);
        char * path = new char[strlen(file_name) + strlen(GRAPHICS)];
        strcpy(path, GRAPHICS);
        strcat(path, file_name);
              
        SDL_Texture * texture = IMG_LoadTexture(m_renderer, path);
        if ( texture == NULL ) {
            fprintf(stderr, "failed to load texture %s\n", path);
            exit(EXIT_FAILURE);
        }
        
        m_tracker.insert( {file_name, 1} );
        m_textures.insert( {file_name, texture} );
        delete[] path;
        
        return texture;
    }
}


void ResourceManager::DestroyTexture(const char * file_name)
{
    auto element = m_textures.find(file_name);
    LOG("DestoryTexture: trying to remove %s...\n", file_name);
    
    if ( element != m_textures.end() ) {
        LOG("-- removing %s\n", file_name);
        auto tracker = m_tracker.find(file_name);
                
        if ( tracker->second == 1 ) {
            tracker->second = 0;
            LOG("-- unloading\n");
            SDL_DestroyTexture(element->second);
            m_textures.erase(element);
            m_tracker.erase(tracker);
        } else {
            --tracker->second;
        }
        LOG("-- count is now %d\n", tracker->second);
    }
}
