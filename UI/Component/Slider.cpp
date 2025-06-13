#include <algorithm>
#include <string>
#include <iostream>

#include "Engine/Point.hpp"
#include "Slider.hpp"

Slider::Slider(float x, float y, float w, float h)
    : ImageButton("stage-select/slider.png", "stage-select/slider-blue.png", x, y),
      Bar("stage-select/bar.png", x, y, w, h),
      End1("stage-select/end.png", -100, -100, 0, 0, 0.5, 0.5),
      End2("stage-select/end.png",-100,-100, 0, 0, 0.5, 0.5),
      Arrow("stage-select/slider.png", "stage-select/slider-blue.png",
          x, y, 30, 79, 0.5, 0.0) {
    Position.x += w;
    Position.y += h / 2;
    Anchor = Engine::Point(0.5, 0.5);
}
void Slider::Draw() const {
    // TODO HACKATHON-5 (3/4): The slider's component should be drawn here.
    Bar.Draw();
    End1.Draw();
    End2.Draw();
    Arrow.Draw();
}
void Slider::SetOnValueChangedCallback(std::function<void(float value)> onValueChangedCallback) {
    OnValueChangedCallback = onValueChangedCallback;
}
void Slider::SetValue(float value) {
    // TODO HACKATHON-5 (4/4): Set the value of the slider and call the callback.
    value = std::min(std::max(value, 0.0f), 1.0f);

    Arrow.Position.x = Bar.Position.x + value * Bar.Size.x;

    if (OnValueChangedCallback)
        OnValueChangedCallback(value);
}
void Slider::OnMouseDown(int button, int mx, int my) {
    if ((button & 1) && mx >= Arrow.Position.x - Arrow.Size.x * Arrow.Anchor.x &&
        mx <= Arrow.Position.x + Arrow.Size.x * (1 - Arrow.Anchor.x) &&
        my >= Arrow.Position.y - Arrow.Size.y * Arrow.Anchor.y &&
        my <= Arrow.Position.y + Arrow.Size.y * (1 - Arrow.Anchor.y))
    Down = true;
}
void Slider::OnMouseUp(int button, int mx, int my) {
    Down = false;
    Arrow.OnMouseMove(mx, my);
}
void Slider::OnMouseMove(int mx, int my) {
    Arrow.OnMouseMove(mx, my);
    if (Down) {
        Arrow.OnMouseMove(Arrow.Position.x + 1, Arrow.Position.y + 1);
        // Clamp
        float clamped = std::min(std::max(static_cast<float>(mx), Bar.Position.x), Bar.Position.x + Bar.Size.x);
        float value = (clamped - Bar.Position.x) / Bar.Size.x * 1.0f;
        SetValue(value);
    }
}
