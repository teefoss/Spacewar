#ifndef resources_h
#define resources_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unordered_map>

#define GRAPHICS    "graphics/"

class ResourceManager
{
public:
    static ResourceManager& shared()
    {
        static ResourceManager instance;
        return instance;
    }
    ResourceManager(ResourceManager const&) = delete;
    void operator=(ResourceManager const&) = delete;
    
    SDL_Texture * getTexture(const char * filename, SDL_Renderer * renderer);
    void destroyTexture(const char * file_name);
    
private:
    ResourceManager() { };
    ~ResourceManager();
    
    std::unordered_map<const char *, SDL_Texture *> textures;
    std::unordered_map<const char *, int> texture_tracker;
};

#endif /* resources_h */
