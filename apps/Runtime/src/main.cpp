#include "Application.hpp"

using namespace Aurora;

class TriangleApp final : public RuntimeApp::Application {
protected:
    void onUpdate(double dt) override {
        auto& g = getGlobals();
        static double timeAcc = 0.0; timeAcc += dt*10;
        g.color[0] = 0.5f + 0.5f * static_cast<float>(std::sin(timeAcc));
        g.color[1] = 0.5f + 0.5f * static_cast<float>(std::sin(timeAcc * 1.3));
        g.color[2] = 0.5f + 0.5f * static_cast<float>(std::sin(timeAcc * 0.7));
    }
};

int main() {
    TriangleApp app;
    return app.run();
}


