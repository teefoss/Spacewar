#include "resources.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>

ResourceManager::~ResourceManager()
{
    for ( auto it : textures ) {
        SDL_DestroyTexture(it.second);
    }
}

SDL_Texture *
ResourceManager::getTexture(const char * file_name, SDL_Renderer * renderer)
{
    auto element = textures.find(file_name);
    
    if ( element != textures.end() ) {
        // found it
        auto tracker = texture_tracker.find(file_name);
        ++tracker->second;
        return element->second;
    } else { // does not exist, load it
        // append directory name
        char * path = new char[strlen(file_name) + strlen(GRAPHICS)];
        strcpy(path, GRAPHICS);
        strcat(path, file_name);
              
        SDL_Texture * texture = IMG_LoadTexture(renderer, path);
        if ( texture == NULL ) {
            fprintf(stderr, "failed to load texture %s\n", path);
            exit(EXIT_FAILURE);
        }
        
        texture_tracker.insert( {file_name, 1} );
        textures.insert( {file_name, texture} );
        delete[] path;
        
        return texture;
    }
}

void ResourceManager::destroyTexture(const char * file_name)
{
    auto element = textures.find(file_name);
    
    if ( element != textures.end() ) {
        auto tracker = texture_tracker.find(file_name);
        
        if ( --tracker->second == 0 ) {
            SDL_DestroyTexture(element->second);
            textures.erase(element);
        }
    }
}
