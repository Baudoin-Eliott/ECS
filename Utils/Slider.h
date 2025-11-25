#pragma once
#include <SDL2/SDL.h>
#include <functional>
#include <algorithm>

struct Slider
{

    int x, y;
    int width, height;

    int min, max;
    int value;

    bool isHorizontal;

    int thumbSize;
    bool isDragging;

    const char *bgTexture;
    const char *fillTexture;
    const char *thumbTexture;
    const char *thumbHoverTexture;

    std::function<void(int)> onValueChanged;

    bool useMouseWheel;
    int wheelStep;

    Slider(int posX, int posY, int w, int h,
           int minVal, int maxVal, int initialValue,
           bool horizontal = true, bool uMW = false, int wS = 5)
        : x(posX), y(posY), width(w), height(h),
          min(minVal), max(maxVal), value(initialValue),
          isHorizontal(horizontal),
          thumbSize(20), isDragging(false),
          bgTexture("assets/Ui/Theme/Theme Wood/nine_path_bg_2.png"),
          fillTexture("assets/Ui/Theme/Theme Wood/slider_progress_hover.png"),
          thumbTexture("assets/Ui/Theme/Theme Wood/slider_progress.png"),
          thumbHoverTexture("assets/Ui/Theme/Theme Wood/v_slidder_grabber.png"),
          onValueChanged(nullptr),
          useMouseWheel(uMW), wheelStep(wS)
    {

        value = std::clamp(value, min, max);
    }

    int getThumbPosition() const
    {

        float ratio = (float)(value - min) / (float)(max - min);

        if (isHorizontal)
        {

            int availableSpace = width - thumbSize;
            return x + (int)(availableSpace * ratio);
        }
        else
        {

            int availableSpace = height - thumbSize;
            return y + availableSpace - (int)(availableSpace * ratio);
        }
    }

    bool isMouseInRect(int mouseX, int mouseY, SDL_Rect rect) const
    {
        return mouseX >= rect.x && mouseX <= rect.x + rect.w &&
               mouseY >= rect.y && mouseY <= rect.y + rect.h;
    }

    SDL_Rect getThumbRect() const
    {
        SDL_Rect rect;

        if (isHorizontal)
        {
            rect.x = getThumbPosition();
            rect.y = y;
            rect.w = thumbSize;
            rect.h = height;
        }
        else
        {
            rect.x = x;
            rect.y = getThumbPosition();
            rect.w = width;
            rect.h = thumbSize;
        }

        return rect;
    }

    void setValue(int newValue)
    {
        int oldValue = value;
        value = std::clamp(newValue, min, max);

        if (value != oldValue && onValueChanged)
        {
            onValueChanged(value);
        }
    }

    void updateValueFromMouse(int mouseX, int mouseY)
    {
        if (isHorizontal)
        {

            int relativeX = mouseX - x - (thumbSize / 2);
            int availableSpace = width - thumbSize;

            relativeX = std::clamp(relativeX, 0, availableSpace);

            float ratio = (float)relativeX / (float)availableSpace;
            int newValue = min + (int)(ratio * (max - min));

            setValue(newValue);
        }
        else
        {

            int relativeY = mouseY - y - (thumbSize / 2);
            int availableSpace = height - thumbSize;

            relativeY = std::clamp(relativeY, 0, availableSpace);

            float ratio = 1.0f - ((float)relativeY / (float)availableSpace);
            int newValue = min + (int)(ratio * (max - min));

            setValue(newValue);
        }
    }

    bool handleInput(SDL_Event &event)
    {

        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            SDL_Rect thumbRect = getThumbRect();

            if (isMouseInRect(event.button.x, event.button.y, thumbRect))
            {
                isDragging = true;
                return true;
            }
        }

        if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
        {
            if (isDragging)
            {
                isDragging = false;
                return true;
            }
        }

        if (event.type == SDL_MOUSEMOTION && isDragging)
        {
            updateValueFromMouse(event.motion.x, event.motion.y);
            return true;
        }

        if (event.type == SDL_MOUSEWHEEL && useMouseWheel)
        {

            if (event.wheel.y > 0)
            {
                setValue(value + wheelStep);
                return true;
            }
            else if (event.wheel.y < 0)
            {
                setValue(value - wheelStep);
                return true;
            }
        }

        return false;
    }

    void render()
    {
        UIThemeManager::getInstance().renderPanel(x, y, width, height, bgTexture);

        if (isHorizontal)
        {
            int thumbPos = getThumbPosition();
            int fillWidth = thumbPos - x;

            if (fillWidth > 0)
            {
                UIThemeManager::getInstance().renderPanel(x, y, fillWidth, height, fillTexture);
            }
        }
        else
        {
            int thumbPos = getThumbPosition();
            int fillHeight = (y + height) - thumbPos;
            int fillY = thumbPos;

            if (fillHeight > 0)
            {
                UIThemeManager::getInstance().renderPanel(x, fillY, width, fillHeight, fillTexture);
            }
        }

        SDL_Rect thumbRect = getThumbRect();
        const char *currentThumbTexture = isDragging ? thumbHoverTexture : thumbTexture;

        UIThemeManager::getInstance().renderPanel(
            thumbRect.x, thumbRect.y,
            thumbRect.w, thumbRect.h,
            currentThumbTexture);
    }
};