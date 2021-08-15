#ifndef resources_h
#define resources_h

#include <SDL2/SDL.h>
#include <unordered_map>

#define GRAPHICS    "graphics/"

class ResourceManager
{
public:
    static ResourceManager& Shared()
    {
        static ResourceManager instance;
        return instance;
    }
    ResourceManager(ResourceManager const&) = delete;
    void operator=(ResourceManager const&) = delete;
    
    void Init(SDL_Renderer * renderer);
    SDL_Texture * GetTexture(const char * filename);
    void DestroyTexture(const char * file_name);
    
private:
    ResourceManager() { };
    ~ResourceManager();
    
    SDL_Renderer * m_renderer = NULL;
    std::unordered_map<const char *, SDL_Texture *> m_textures;
    std::unordered_map<const char *, int> m_tracker;
};

#endif /* resources_h */
