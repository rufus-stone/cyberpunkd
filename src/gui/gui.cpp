#include "gui/gui.hpp"

#include <future>
#include <chrono>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <imgui-SFML.h>
#include <imgui.h>

#include "core/watcher.hpp"
#include "gui/about.hpp"
#include "gui/config_bar.hpp"
#include "gui/state.hpp"
#include "gui/menubar.hpp"

#include "utils/config.hpp"
#include "utils/json.hpp"

namespace pnkd::gui
{

using namespace std::chrono_literals;

auto start() -> void
{
  // How big is our screen?
  unsigned int const w = sf::VideoMode::getDesktopMode().width;
  unsigned int const h = sf::VideoMode::getDesktopMode().height;
  auto screen_size = ImVec2{static_cast<float>(w), static_cast<float>(h)};

  // Set up the app
  auto app = sf::RenderWindow{sf::VideoMode{w / 2, h / 2}, "Cyberpunkd"};
  app.setFramerateLimit(144);
  app.setVerticalSyncEnabled(true);

  // How big is our app window?
  auto [ww, hh] = app.getSize();
  auto app_size = ImVec2{static_cast<float>(ww), static_cast<float>(hh)};

  // Activate imgui
  ImGui::SFML::Init(app);

  // Fix broken backwards mouse cursor on Linux
  ImGuiIO &io = ImGui::GetIO();
  io.MouseDrawCursor = true;

  // Load our gui theme
  cyberpunk_theme();

  // Load the config file
  auto cfg_helper = pnkd::cfg::config_helper{};
  auto config_json = cfg_helper.load_config();
  spdlog::info(config_json.dump());

  // Create gui state object
  auto g = pnkd::gui::state{app};
  g.screen_size = screen_size;
  g.app_size = app_size;
  g.cfg_helper = std::move(cfg_helper);
  g.sleep_for = pnkd::json::int_optional(g.cfg_helper.cfg_json(), "sleep_for").value_or(1);
  g.buffer_size = pnkd::json::int_optional(g.cfg_helper.cfg_json(), "buffer_size").value_or(3);
  g.screenshot_dir = pnkd::json::string_optional(g.cfg_helper.cfg_json(), "screenshot_dir").value_or("");
  g.tessdata_dir = pnkd::json::string_optional(g.cfg_helper.cfg_json(), "tessdata_dir").value_or("tessdata");

  // Flag to trigger background threads to finish up
  std::atomic_bool stop_threads{false};

  // Create background thread to handle puzzle solving
  auto fut = std::async(std::launch::async, [&stop_threads, &g]() {
    auto screenshot_dir = g.get_screenshot_dir();
    auto tessdata_dir = g.get_tessdata_dir();
    auto sleep_for = g.get_sleep_for();
    auto buffer_size = g.get_buffer_size();

    // Create a watcher object
    auto watcher = pnkd::watcher{
      screenshot_dir,
      tessdata_dir,
      static_cast<std::size_t>(buffer_size),
      static_cast<std::size_t>(sleep_for)};

    while (stop_threads == false)
    {
      screenshot_dir = g.get_screenshot_dir();
      tessdata_dir = g.get_tessdata_dir();
      sleep_for = g.get_sleep_for();
      buffer_size = g.get_buffer_size();

      if (g.start_watcher)
      {
        spdlog::info("Starting watcher - screenshots_path: {} / tessdata_dir: {} / buffer_size: {} / sleep_for: {}", screenshot_dir, tessdata_dir, buffer_size, sleep_for);

        watcher.do_once();

        g.start_watcher = false;
      }

      if (g.stop_watcher)
      {
        spdlog::info("Stopping watcher...");
        g.stop_watcher = false;
        watcher.forget_previous_image();
      }

      spdlog::info("Sleeping {}s...", sleep_for);
      std::this_thread::sleep_for(std::chrono::seconds{sleep_for});
    }
  });

  // Clock to track frame render times
  auto deltaClock = sf::Clock{};

  // Start main gui loop
  while (app.isOpen())
  {
    // Poll for events
    auto event = sf::Event{};
    while (app.pollEvent(event))
    {
      ImGui::SFML::ProcessEvent(event);

      if (event.type == sf::Event::Closed)
      {
        app.close();
      }

      // Keyboard shortcuts
      // Ctrl+Q == quit
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
      {
        app.close();
      }
    }

    // Check how big the app window is now (in case it's been resized)
    auto [ww, hh] = app.getSize();
    auto const current_app_size = ImVec2{static_cast<float>(ww), static_cast<float>(hh)};
    g.app_size = current_app_size;

    // Update the app - this must be called before drawing any imgui UI elements
    ImGui::SFML::Update(app, deltaClock.restart());

    // Prepare imgui UI elements
    pnkd::gui::menubar(g);
    pnkd::gui::about_dialog(g);
    pnkd::gui::config_bar(g);
    ImGui::ShowDemoWindow();

    // Render the UI
    app.clear(sf::Color{25, 25, 25});
    ImGui::SFML::Render(app);
    app.display();
  }


  stop_threads = true;
  fut.get();

  ImGui::SFML::Shutdown();
}


auto cyberpunk_theme() -> void
{
  auto &style = ImGui::GetStyle();

  // Shapes etc
  style.WindowRounding = 0.0F;

  // Colour palette
  // clang-format off
  auto *colors = style.Colors;
  colors[ImGuiCol_Text]                   = ImVec4(1.00F, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
  colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
  colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
  colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
  colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
  colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
  colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
  colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
  colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
  colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
  colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
  colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
  colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
  colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
  colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  colors[ImGuiCol_Tab]                    = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
  colors[ImGuiCol_TabHovered]             = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
  colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
  colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
  colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
  // clang-format on

  /*
  static auto const yellow1 = static_cast<ImVec4>(ImColor{252, 236, 12});
  static auto const yellow2 = static_cast<ImVec4>(ImColor{216, 188, 102});
  static auto const yellow3 = static_cast<ImVec4>(ImColor{151, 141, 1});
  static auto const black = static_cast<ImVec4>(ImColor{0, 0, 0});
  static auto const blue = static_cast<ImVec4>(ImColor{4, 218, 246});
  static auto const red = static_cast<ImVec4>(ImColor{255, 0, 60});
  static auto const grey = static_cast<ImVec4>(ImColor{85, 75, 65});

  // Colour palette
  // clang-format off
  style.Colors[ImGuiCol_Text]                   = blue;
  style.Colors[ImGuiCol_TextDisabled]           = grey;
  style.Colors[ImGuiCol_WindowBg]               = black;
  //style.Colors[ImGuiCol_ChildBg]                = 
  //style.Colors[ImGuiCol_PopupBg]                = 
  //style.Colors[ImGuiCol_Border]                 = 
  //style.Colors[ImGuiCol_BorderShadow]           = black;
  //style.Colors[ImGuiCol_FrameBg]                = 
  //style.Colors[ImGuiCol_FrameBgHovered]         = 
  //style.Colors[ImGuiCol_FrameBgActive]          = 
  //style.Colors[ImGuiCol_TitleBg]                = 
  //style.Colors[ImGuiCol_TitleBgActive]          = 
  //style.Colors[ImGuiCol_TitleBgCollapsed]       = 
  //style.Colors[ImGuiCol_MenuBarBg]              = 
  //style.Colors[ImGuiCol_ScrollbarBg]            = 
  //style.Colors[ImGuiCol_ScrollbarGrab]          = 
  //style.Colors[ImGuiCol_ScrollbarGrabHovered]   = 
  //style.Colors[ImGuiCol_ScrollbarGrabActive]    = 
  style.Colors[ImGuiCol_CheckMark]              = red;
  //style.Colors[ImGuiCol_SliderGrab]             = 
  //style.Colors[ImGuiCol_SliderGrabActive]       = 
  style.Colors[ImGuiCol_Button]                 = yellow1;
  style.Colors[ImGuiCol_ButtonHovered]          = yellow2;
  style.Colors[ImGuiCol_ButtonActive]           = yellow3;
  //style.Colors[ImGuiCol_Header]                 = 
  //style.Colors[ImGuiCol_HeaderHovered]          = 
  //style.Colors[ImGuiCol_HeaderActive]           = 
  style.Colors[ImGuiCol_Separator]              = red;
  style.Colors[ImGuiCol_SeparatorHovered]       = red;
  style.Colors[ImGuiCol_SeparatorActive]        = red;
  //style.Colors[ImGuiCol_ResizeGrip]             = 
  //style.Colors[ImGuiCol_ResizeGripHovered]      = 
  //style.Colors[ImGuiCol_ResizeGripActive]       = 
  //style.Colors[ImGuiCol_PlotLines]              = 
  //style.Colors[ImGuiCol_PlotLinesHovered]       = 
  //style.Colors[ImGuiCol_PlotHistogram]          = 
  //style.Colors[ImGuiCol_PlotHistogramHovered]   = 
  //style.Colors[ImGuiCol_TextSelectedBg]         = 
  //style.Colors[ImGuiCol_ModalWindowDarkening]   = 
  //style.Colors[ImGuiCol_DragDropTarget]         = 
  //style.Colors[ImGuiCol_NavHighlight]           = 
  //style.Colors[ImGuiCol_NavWindowingHighlight]  =
  // clang-format on
  */
}

auto light_theme() -> void
{
}

} // namespace pnkd::gui