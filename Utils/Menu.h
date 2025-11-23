#pragma once

#include <SDL2/SDL.h>
#include <string>

class Menu
{
protected:
    std::string menuName;
    bool isActive = false;

public:
    Menu(const std::string &name) : menuName(name) {}

    virtual ~Menu() = default;

    virtual void onEnter() {std::cout << "[Menu] enter on" << menuName << "\n";}
    virtual void onExit() {std::cout << "[Menu] exit off" << menuName << "\n";};

    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer *renderer) = 0;

    const std::string &getName() const { return menuName; }
    bool isMenuActive() const { return isActive; }

    void setActive(bool state) { isActive = state; }

    virtual void handleInput(SDL_Event& event) = 0;
};