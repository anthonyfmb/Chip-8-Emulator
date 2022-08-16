#include <SDL.h>

class Platform {
public:
 
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
 
  Platform(char const *title, int window_width, int window_height, int texture_width,
      int texture_height);
  ~Platform();

  void Update(void const *buffer, int pitch);
  
  bool ProcessInput(uint8_t* keys);
};
