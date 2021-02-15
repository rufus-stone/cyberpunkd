#pragma once

#include <imgui.h>
#include <string>

namespace pnkd::gui
{

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.txt)
inline auto help_marker(const char *desc) -> void
{
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0F);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}


auto enum_to_string(ImGuiCol color) -> std::string
{
  switch (color)
  {
    case ImGuiCol_Text:
      return "ImGuiCol_Text";
    case ImGuiCol_TextDisabled:
      return "ImGuiCol_TextDisabled";
    case ImGuiCol_WindowBg:
      return "ImGuiCol_WindowBg";
    case ImGuiCol_ChildBg:
      return "ImGuiCol_ChildBg";
    case ImGuiCol_PopupBg:
      return "ImGuiCol_PopupBg";
    case ImGuiCol_Border:
      return "ImGuiCol_Border";
    case ImGuiCol_BorderShadow:
      return "ImGuiCol_BorderShadow";
    case ImGuiCol_FrameBg:
      return "ImGuiCol_FrameBg";
    case ImGuiCol_FrameBgHovered:
      return "ImGuiCol_FrameBgHovered";
    case ImGuiCol_FrameBgActive:
      return "ImGuiCol_FrameBgActive";
    case ImGuiCol_TitleBg:
      return "ImGuiCol_TitleBg";
    case ImGuiCol_TitleBgActive:
      return "ImGuiCol_TitleBgActive";
    case ImGuiCol_TitleBgCollapsed:
      return "ImGuiCol_TitleBgCollapsed";
    case ImGuiCol_MenuBarBg:
      return "ImGuiCol_MenuBarBg";
    case ImGuiCol_ScrollbarBg:
      return "ImGuiCol_ScrollbarBg";
    case ImGuiCol_ScrollbarGrab:
      return "ImGuiCol_ScrollbarGrab";
    case ImGuiCol_ScrollbarGrabHovered:
      return "ImGuiCol_ScrollbarGrabHovered";
    case ImGuiCol_ScrollbarGrabActive:
      return "ImGuiCol_ScrollbarGrabActive";
    case ImGuiCol_CheckMark:
      return "ImGuiCol_CheckMark";
    case ImGuiCol_SliderGrab:
      return "ImGuiCol_SliderGrab";
    case ImGuiCol_SliderGrabActive:
      return "ImGuiCol_SliderGrabActive";
    case ImGuiCol_Button:
      return "ImGuiCol_Button";
    case ImGuiCol_ButtonHovered:
      return "ImGuiCol_ButtonHovered";
    case ImGuiCol_ButtonActive:
      return "ImGuiCol_ButtonActive";
    case ImGuiCol_Header:
      return "ImGuiCol_Header";
    case ImGuiCol_HeaderHovered:
      return "ImGuiCol_HeaderHovered";
    case ImGuiCol_HeaderActive:
      return "ImGuiCol_HeaderActive";
    case ImGuiCol_Separator:
      return "ImGuiCol_Separator";
    case ImGuiCol_SeparatorHovered:
      return "ImGuiCol_SeparatorHovered";
    case ImGuiCol_SeparatorActive:
      return "ImGuiCol_SeparatorActive";
    case ImGuiCol_ResizeGrip:
      return "ImGuiCol_ResizeGrip";
    case ImGuiCol_ResizeGripHovered:
      return "ImGuiCol_ResizeGripHovered";
    case ImGuiCol_ResizeGripActive:
      return "ImGuiCol_ResizeGripActive";
    case ImGuiCol_Tab:
      return "ImGuiCol_Tab";
    case ImGuiCol_TabHovered:
      return "ImGuiCol_TabHovered";
    case ImGuiCol_TabActive:
      return "ImGuiCol_TabActive";
    case ImGuiCol_TabUnfocused:
      return "ImGuiCol_TabUnfocused";
    case ImGuiCol_TabUnfocusedActive:
      return "ImGuiCol_TabUnfocusedActive";
    case ImGuiCol_PlotLines:
      return "ImGuiCol_PlotLines";
    case ImGuiCol_PlotLinesHovered:
      return "ImGuiCol_PlotLinesHovered";
    case ImGuiCol_PlotHistogram:
      return "ImGuiCol_PlotHistogram";
    case ImGuiCol_PlotHistogramHovered:
      return "ImGuiCol_PlotHistogramHovered";
    case ImGuiCol_TextSelectedBg:
      return "ImGuiCol_TextSelectedBg";
    case ImGuiCol_DragDropTarget:
      return "ImGuiCol_DragDropTarget";
    case ImGuiCol_NavHighlight:
      return "ImGuiCol_NavHighlight";
    case ImGuiCol_NavWindowingHighlight:
      return "ImGuiCol_NavWindowingHighlight";
    case ImGuiCol_NavWindowingDimBg:
      return "ImGuiCol_NavWindowingDimBg";
    case ImGuiCol_ModalWindowDimBg:
      return "ImGuiCol_ModalWindowDimBg";
    default:
      return "UNKNOWN";
  };
}

} // namespace pnkd::gui
