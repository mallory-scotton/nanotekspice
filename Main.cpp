///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Shell/Shell.hpp"
#include "Errors/ComponentException.hpp"
#include <iostream>

#ifndef NTS_BONUS

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <circuit_file>" << std::endl;
        return (84);
    }

    try {
        nts::Shell shell;
        shell.loadCircuit(argv[1]);
        shell.run();
    } catch (const nts::ComponentException& e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
        return (84);
    }

    return (0);
}

#else

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Shell/Parser.hpp"

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <circuit_file>" << std::endl;
        return (84);
    }

    nts::Circuit circuit;

    try {
        nts::Parser::parseCircuit(circuit, argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
        return (84);
    }

    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "NanoTekSpice",
        sf::Style::Close
    );

    sf::View view = window.getDefaultView();

    float zoom = 1.0f;
    bool isDragging = false;
    sf::Vector2i lastMousePos;
    std::shared_ptr<nts::IComponent> selectedComponent = nullptr;
    sf::Vector2f dragOffset;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (auto scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
                const float factor = 1.1f;
                zoom *= (scroll->delta > 0) ? factor : 1.0f / factor;

                zoom = std::max(0.1f, std::min(zoom, 5.0f));

                sf::Vector2f mousePos = window.mapPixelToCoords(
                    scroll->position
                );
                view.setSize(window.getDefaultView().getSize());
                view.zoom(1.0f / zoom);

                sf::Vector2f newMousePos = window.mapPixelToCoords(
                    scroll->position,
                    view
                );
                view.move(mousePos - newMousePos);
            }
            if (auto mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Right) {
                    isDragging = true;
                    lastMousePos = mouse->position;
                } else if (mouse->button == sf::Mouse::Button::Left) {
                    sf::Vector2f worldPos = window.mapPixelToCoords(mouse->position);

                    selectedComponent = circuit.getComponentAt(worldPos);

                    if (selectedComponent) {
                        dragOffset = selectedComponent->getPosition() - worldPos;
                    }
                }
            }
            if (auto mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouse->button == sf::Mouse::Button::Right) {
                    isDragging = false;
                } else if (mouse->button == sf::Mouse::Button::Left) {
                    selectedComponent = nullptr;
                }
            }
            if (auto mouse = event->getIf<sf::Event::MouseMoved>()) {
                if (isDragging) {
                    sf::Vector2f delta =
                        window.mapPixelToCoords(lastMousePos) -
                        window.mapPixelToCoords(mouse->position);
                    view.move(delta);
                    lastMousePos = mouse->position;
                } else if (selectedComponent) {
                    sf::Vector2f worldPos = window.mapPixelToCoords(
                        mouse->position
                    );
                    selectedComponent->setPosition(worldPos + dragOffset);
                }
            }
            if (auto key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::R) {
                    view = window.getDefaultView();
                    zoom = 1.f;
                }
            }
        }

        window.setView(view);

        window.clear(sf::Color(200, 200, 200));
        circuit.draw(window);
        window.display();
    }
}

#endif
