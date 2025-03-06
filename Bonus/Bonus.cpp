///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Bonus.hpp"
#include "DigitalInput.hpp"
#include <SFML/Audio.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <ImNodes.h>
#include <ImNodesEz.h>
#include <filesystem>
#include <unistd.h>
#include <istream>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
namespace fs = std::filesystem;

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
void Bonus::loadNtsFiles(
    const std::string& directory,
    std::vector<std::string>& m_files
)
{
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Directory does not exist: " << directory << std::endl;
        return;
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".nts") {
            m_files.push_back(entry.path().string());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
Bonus::Bonus(const char* filename)
    : m_window(
        sf::VideoMode({1920, 1080}),
        "NanoTekSpice",
        sf::State::Fullscreen
    )
    , m_commandBuffer(nullptr)
    , m_bufferSize(0)
    , m_bufferPosition(0)
    , m_processingCommands(false)
    , m_waitingForNextFrame(false)
{
    if (!ImGui::SFML::Init(m_window)) {
        throw std::runtime_error("Coudl'nt initialize ImGui");
    }

    if (filename) {
        nts::Parser::parseCircuit(m_circuit, filename);
    }

    Bonus::loadNtsFiles("Circuits/", m_files);

    initializeCommandProcessing();
}

///////////////////////////////////////////////////////////////////////////////
Bonus::~Bonus()
{
    if (m_commandBuffer) {
        delete[] m_commandBuffer;
    }
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
// Command Types
///////////////////////////////////////////////////////////////////////////////
enum CommandType {
    CMD_SET = 0x01,
    CMD_WAIT = 0x02,
    CMD_PLAY = 0x03
};

///////////////////////////////////////////////////////////////////////////////
void Bonus::initializeCommandProcessing(void)
{
    if (!isatty(fileno(stdin)) && m_circuit.getComponents().size() > 0) {
        // Find the required components
        for (auto& [name, cmp] : m_circuit.getComponents()) {
            if (auto color = std::dynamic_pointer_cast<Components::InputColor>(cmp)) {
                m_colorComponent = color;
            } else if (auto position = std::dynamic_pointer_cast<Components::Input16>(cmp)) {
                m_positionComponent = position;
            }
        }

        if (!m_colorComponent || !m_positionComponent) {
            return;
        }

        // Read binary data from stdin into buffer
        std::vector<uint8_t> tempBuffer;
        uint8_t byte;
        while (std::cin.read(reinterpret_cast<char*>(&byte), 1)) {
            tempBuffer.push_back(byte);
        }

        // Copy to permanent buffer
        m_bufferSize = tempBuffer.size();
        if (m_bufferSize > 0) {
            m_commandBuffer = new uint8_t[m_bufferSize];
            std::copy(tempBuffer.begin(), tempBuffer.end(), m_commandBuffer);
            m_processingCommands = true;
            m_bufferPosition = 0;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Bonus::processNextFrame(sf::Music& music)
{
    if (!m_processingCommands || !m_commandBuffer || m_bufferPosition >= m_bufferSize) {
        return (false);
    }

    bool frameProcessed = false;

    while (m_bufferPosition < m_bufferSize) {
        // Read command type
        uint8_t cmdType = m_commandBuffer[m_bufferPosition++];

        // Process based on command type
        switch (cmdType) {
            case CMD_SET: {
                // Make sure we have enough data for a SET command (7 bytes total)
                if (m_bufferPosition + 5 > m_bufferSize) {
                    return (frameProcessed);
                }

                uint8_t x = m_commandBuffer[m_bufferPosition++];
                uint8_t y = m_commandBuffer[m_bufferPosition++];
                uint8_t r = m_commandBuffer[m_bufferPosition++];
                uint8_t g = m_commandBuffer[m_bufferPosition++];
                uint8_t b = m_commandBuffer[m_bufferPosition++];
                uint8_t a = 255;

                m_positionComponent->setValue(x + y * 64);
                m_colorComponent->setValue(a << 24 | b << 16 | g << 8 | r);
                m_circuit.simulate();
                frameProcessed = true;
                break;
            }

            case CMD_WAIT:
                // End of frame, return to render
                return (frameProcessed);

            case CMD_PLAY: {
                // Read string length
                if (m_bufferPosition >= m_bufferSize) {
                    return (frameProcessed);
                }

                uint8_t strLen = m_commandBuffer[m_bufferPosition++];

                // Make sure we have enough data for the filename
                if (m_bufferPosition + strLen > m_bufferSize) {
                    return (frameProcessed);
                }

                // Extract filename
                std::string audioFile(reinterpret_cast<char*>(&m_commandBuffer[m_bufferPosition]), strLen);
                m_bufferPosition += strLen;

                // Play the audio file
                if (music.openFromFile(audioFile)) {
                    std::cout << "Playing " << audioFile << std::endl;
                    music.setVolume(100);
                    music.play();
                }
                break;
            }

            default:
                // Unknown command type, skip to next byte
                std::cerr << "Unknown command type: " << static_cast<int>(cmdType) << std::endl;
                break;
        }
    }

    // End of buffer reached
    return (frameProcessed);
}

///////////////////////////////////////////////////////////////////////////////
void Bonus::optimizeMemory(void)
{
    // If we've processed more than half of the buffer, we can compact memory
    if (m_commandBuffer && m_bufferPosition > m_bufferSize / 2) {
        // Calculate remaining data size
        size_t remainingSize = m_bufferSize - m_bufferPosition;

        // Create a new buffer for the remaining data
        uint8_t* newBuffer = new uint8_t[remainingSize];

        // Copy remaining data to the new buffer
        std::memcpy(newBuffer, m_commandBuffer + m_bufferPosition, remainingSize);

        // Delete old buffer
        delete[] m_commandBuffer;

        // Update buffer pointer and size
        m_commandBuffer = newBuffer;
        m_bufferSize = remainingSize;
        m_bufferPosition = 0;

        // Force a GC if possible
        #ifdef __cpp_lib_memory_resource
        std::pmr::get_default_resource()->release();
        #endif
    }
}

///////////////////////////////////////////////////////////////////////////////
void Bonus::run(void)
{
    ImNodes::Ez::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    sf::Clock frameClock;
    sf::Music music;
    float frameTime = 1.0f / 12.0f;
    float lastFrameTime = 0.0f;

    const size_t memoryCleanupInterval = 50;
    size_t frameCounter = 0;

    while (m_window.isOpen()) {
        m_window.handleEvents([this](const auto& event) {
            ImGui::SFML::ProcessEvent(m_window, event);
            handleEvent(event);
        });

        ImGui::SFML::Update(m_window, m_clock.restart());

        // Process the next frame of commands when it's time
        float currentTime = frameClock.getElapsedTime().asSeconds();
        if (m_processingCommands) {
            if (!m_waitingForNextFrame) {
                // Process all set commands until next wait
                if (processNextFrame(music)) {
                    m_waitingForNextFrame = true;
                    lastFrameTime = currentTime;
                    frameCounter++;

                    // Periodically force garbage collection by performing cleanup
                    if (frameCounter % memoryCleanupInterval == 0) {
                        optimizeMemory();
                    }
                }
            } else if (currentTime - lastFrameTime >= frameTime) {
                // Time to process the next frame
                m_waitingForNextFrame = false;
            }
        }

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New")) {
                    m_circuit.clear();
                }
                if (ImGui::BeginMenu("Open")) {
                    for (const auto& file : m_files) {
                        if (ImGui::MenuItem(file.c_str())) {
                            m_circuit.clear();
                            Parser::parseCircuit(m_circuit, file);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        useDockSpace();
        m_circuit.draw();

        m_window.clear(sf::Color::Black);
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
}

} // namespace nts
