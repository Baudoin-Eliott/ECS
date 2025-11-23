#pragma once

#include "Menu.h"
#include <vector>
#include <memory>
#include <iostream>

class UIManager
{
private:
    UIManager() = default;
    UIManager(const UIManager &) = delete;
    UIManager &operator=(const UIManager &) = delete;

    std::vector<std::unique_ptr<Menu>> menuStack;

    SDL_Renderer *renderer = nullptr;

public:
    static UIManager &getInstance()
    {
        static UIManager instance;
        return instance;
    }

    void setRenderer(SDL_Renderer *rend)
    {
        renderer = rend;
    }

    void pushMenu(Menu *menu)
    {
        if (!menuStack.empty())
        {
            menuStack.back()->setActive(false);
        }
        menuStack.push_back(std::unique_ptr<Menu>(menu));
        menu->onEnter();
    }

    void popMenu()
    {
        if (menuStack.empty())
            return;
        menuStack.back()->onExit();
        menuStack.pop_back();

        if (!menuStack.empty())
        {
            menuStack.back()->setActive(true);
        }
    }

    void clearMenu()
    {
        while (!menuStack.empty())
        {
            popMenu();
        }
        std::cout << "[UIManager] All menus cleared\n";
    }

    void update(float deltaTime)
    {
        if (!menuStack.empty())
            menuStack.back()->update(deltaTime);
    }

    void render()
    {
        for (auto &menu : menuStack)
        {
            menu->render(renderer);
        }
    }

    void handleInput(SDL_Event &event)
    {
        if (!menuStack.empty())
            menuStack.back()->handleInput(event);
    }

    bool hasMenus() const{
        return !menuStack.empty();
    }

    Menu* getCurrentMenu() const {
        return menuStack.back().get();
    }

    size_t getMenuCount() const {
        return menuStack.size();
    }
};