#include "gui/gui.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <imgui-SFML.h>
#include <imgui.h>

#include "gui/about.hpp"
#include "gui/config_bar.hpp"
#include "gui/state.hpp"
#include "gui/menubar.hpp"

namespace pnkd::gui
{

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

  // Create gui state object
  auto g = pnkd::gui::state{app};
  g.screen_size = screen_size;
  g.app_size = app_size;

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
    //ImGui::ShowDemoWindow();

    // Render the UI
    app.clear(sf::Color{25, 25, 25});
    ImGui::SFML::Render(app);
    app.display();
  }

  ImGui::SFML::Shutdown();
}


auto cyberpunk_theme() -> void
{
  auto &style = ImGui::GetStyle();

  // Shapes etc
  style.WindowRounding = 0.0F;

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