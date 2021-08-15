#include "resources.h"
#include "log.h"
#include "defines.h"

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
    
    if ( element != m_textures.end() ) { // found
        
        auto ref_count = m_reference_counts.find(file_name);
        ++ref_count->second;
        
        return element->second;
        
    } else { // not found, load it

        char * path = new char[strlen(file_name) + strlen(GRAPHICS)];
        strcpy(path, GRAPHICS);
        strcat(path, file_name);
              
        SDL_Texture * texture = IMG_LoadTexture(m_renderer, path);
        if ( texture == NULL ) {
            SDL_ERROR("IMG_LoadTexture failed\n");
        }
        
        m_reference_counts.insert( {file_name, 1} );
        m_textures.insert( {file_name, texture} );
        delete[] path;
        
        return texture;
    }
}


void ResourceManager::ReleaseTexture(const char * file_name)
{
    auto element = m_textures.find(file_name);
    
    if ( element != m_textures.end() ) {

        auto ref_count = m_reference_counts.find(file_name);
                
        if ( ref_count->second == 1 ) {
            ref_count->second = 0;
            SDL_DestroyTexture(element->second);
            m_textures.erase(element);
        } else {
            --ref_count->second;
        }
    }
}
