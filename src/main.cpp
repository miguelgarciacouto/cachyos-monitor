#include <iostream> 
#include <memory>
#include <atomic> 
#include <thread>

//FTXUI includes 
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "providers/psiProvider.hpp"
#include "providers/schedulerProvider.hpp"
#include "providers/zramProviders.hpp"
#include "providers/ramProvider.hpp"

int main() {
    using namespace ftxui; 

    auto screen = ScreenInteractive::Fullscreen();

    //Create the providers
    cachy::PsiProvider psiTracker;
    cachy::schedulerProvider schedulerTracker;
    cachy::ZramProvider zramTracker;
    cachy::RamProvider ramTracker;

    std::atomic<bool> refresh_ui = true;
    std::jthread refresh_thread([&] {
        while (refresh_ui) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);

            if (refresh_ui) {
                screen.PostEvent(Event::Custom);
            }
        }
    });

    //Build the dyanmic document
    auto renderer = Renderer([&] {

        auto schedulerColor = schedulerTracker.GetCurrentRate() > 100000 ? Color::Red : Color::Green;

        auto scheduler_graph = [&](int width, int height) {

            if(width <= 0 || height <= 0) {
                return std::vector<int>(0);
            }

            std::vector<int> out(width, 0); 
            auto hist = schedulerTracker.GetHistory();
            if (hist.empty()) {
                return out;
            }

            int max_val = 1;
            for (int v : hist) {

                if (v > max_val) {
                    max_val = v;
                }
            }

            int hist_idx = hist.size() -1; 
            for (int i = width -1; i >= 0 && hist_idx >= 0; --i, --hist_idx) {
                int available_height_in_dots = height * 4;
                out[i] = (hist[hist_idx] * available_height_in_dots) / max_val;
            }
            return out; 
         };

        return vbox({
            text("CachyOS Monitor - Live System Metrics") | bold | center,
            separator(),

            //Fetch the live data from the provider and display it 
            text(psiTracker.GetName() + ": ") | bold,
            text(psiTracker.GetLatestPayload()) | color(Color::Green),
            separator(),

            hbox({
                text(ramTracker.GetName() + ": ") | bold,
                text(ramTracker.GetLatestPayload()) | size(WIDTH, EQUAL, 20) | color(Color::RedLight),
                gauge(ramTracker.GetUsagePercentage()) | color(Color::RedLight) | flex,
            }),
            separator(),

            text(zramTracker.GetName() + ": ") | bold,
            text(zramTracker.GetLatestPayload()) | color(Color::BlueLight),
            separator(),

            text(schedulerTracker.GetName() + ": ") | bold,

            hbox({
                text(schedulerTracker.GetLatestPayload()) | color(schedulerColor) | size(WIDTH, EQUAL, 25),
                graph(scheduler_graph) | color(schedulerColor) | size(HEIGHT, EQUAL, 5) | flex,
            }), 
            separator(),
            text("Press CTRL + X to exit") | dim | center,
        }) | border;
    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::CtrlX) {
            refresh_ui = false;
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(component);

    refresh_ui = false;

    return 0;

}