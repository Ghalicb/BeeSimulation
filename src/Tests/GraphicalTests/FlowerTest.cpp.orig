/*
 * prjsv 2015
 * 2014
 * Marco Antognini
 */

#include <Application.hpp>
#include <Env/Env.hpp>

class FlowerTest : public Application
{
public:
    using Application::Application;

    // Override a few methods
    virtual void onRun() override final;
    virtual void onEvent(sf::Event event, sf::RenderWindow& window) override final;
    virtual void onDraw(sf::RenderTarget& target) override final;

private:
    bool mShowFlowerZone = false; ///< Enable/disable flower zone visualisation
};

IMPLEMENT_MAIN(FlowerTest);

void FlowerTest::onRun()
{
    // Disable flower generator
    getConfig()["simulation"]["flower generator"]["active"] = j::boolean(false);
}

void FlowerTest::onEvent(sf::Event event, sf::RenderWindow&)
{
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        default: break;

        // Add flower: show the area covered by the flower
        case sf::Keyboard::F:
            mShowFlowerZone = true;
            break;
        }
    } else if (event.type == sf::Event::KeyReleased) {
        switch (event.key.code) {
        default: break;

        // Add flower: try to create a new one and disable visualisation zone
        case sf::Keyboard::F:
            mShowFlowerZone = false;
            if (getEnv().addFlowerAt(getCursorPositionInView())) {
                std::cout << "New flower created\n";
            } else {
                std::cout << "Couldn't create new flower\n";
            }
            break;
        }
    }
}

void FlowerTest::onDraw(sf::RenderTarget& target)
{
    if (mShowFlowerZone) {
        auto pos = getCursorPositionInView();
        getEnv().drawFlowerZone(target, pos);
    }
}
