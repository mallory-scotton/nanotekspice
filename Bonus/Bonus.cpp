///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Bonus.hpp"
#include <imgui-SFML.h>
#include <imgui.h>
#include <ImNodes.h>
#include <ImNodesEz.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
Bonus::Bonus(const char* filename)
    : m_window(
        sf::VideoMode({1920, 1080}),
        "NanoTekSpice",
        sf::State::Fullscreen
    )
{
    if (!ImGui::SFML::Init(m_window)) {
        throw std::runtime_error("Coudl'nt initialize ImGui");
    }

    nts::Parser::parseCircuit(m_circuit, filename);
}

///////////////////////////////////////////////////////////////////////////////
Bonus::~Bonus()
{
    ImGui::SFML::Shutdown();
}

///////////////////////////////////////////////////////////////////////////////
void Bonus::handleEvent(const sf::Event::Closed&)
{
    m_window.close();
}

///////////////////////////////////////////////////////////////////////////////
void Bonus::useDockSpace(void)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |=
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

    static bool first_time = true;
    if (first_time) {
        first_time = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        ImGuiID dock_main_id, dock_id_right;
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f,
                                    &dock_id_right, &dock_main_id);

        ImGui::DockBuilderDockWindow("Blueprint", dock_main_id);
        ImGui::DockBuilderDockWindow("Property Editor", dock_id_right);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Bonus::run(void)
{
    ImNodes::Ez::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    while (m_window.isOpen()) {
        m_window.handleEvents([this](const auto& event) {
            ImGui::SFML::ProcessEvent(m_window, event);
            handleEvent(event);
        });

        ImGui::SFML::Update(m_window, m_clock.restart());

        useDockSpace();
        m_circuit.draw();

        m_window.clear(sf::Color::Black);

        ImGui::SFML::Render(m_window);

        m_window.display();
    }
}

} // namespace nts
