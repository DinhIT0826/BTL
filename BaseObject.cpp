
#include "BaseObject.h"

BaseObject::BaseObject()
{
    P_object_ = NULL;
    rect_.x = 0;
    rect_.y = 0;
    rect_.w = 0;
    rect_.h = 0;

}
BaseObject::~BaseObject()
{
    Free();
}

bool BaseObject::LoadImg(const std::string& path, SDL_Renderer* screen)
{
    SDL_Texture* new_texture = NULL;

    SDL_Surface* load_surface = IMG_Load(path.c_str());
    if (load_surface != NULL)
    {
        SDL_SetColorKey(load_surface, SDL_TRUE, SDL_MapRGB(load_surface->format, COLOR_KEY_R, COLOR_KEY_G, COLOR_KEY_B));
        new_texture = SDL_CreateTextureFromSurface(screen, load_surface);
        if (new_texture != NULL)
        {
            rect_.w = load_surface->w;
            rect_.h = load_surface->h;
        }

        SDL_FreeSurface(load_surface);
    }

    P_object_ = new_texture;

    return P_object_ != NULL;
}

void BaseObject::Render(SDL_Renderer* des, const SDL_Rect* clip /*= NULL*/)
{
    SDL_Rect renderquad = {rect_.x, rect_.y, rect_.w, rect_.h};
    SDL_RenderCopy(des, P_object_, clip, &renderquad);
}

void BaseObject::Free()
{
    if (P_object_ != NULL)
    {
        SDL_DestroyTexture(P_object_);
        P_object_ = NULL;
        rect_.w = 0;
        rect_.h = 0;
    }
}

