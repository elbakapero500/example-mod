#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
using namespace geode::prelude;

class $modify(BufferLayer, GJBaseGameLayer) {
    struct Fields {
        int left = 0;
        bool held = false;
        bool injecting = false;
        int step = 0;
        int corrected = 0;
        CCLabelBMFont* label = nullptr;
    };

    void pushButton(int button, bool p2) {
        GJBaseGameLayer::pushButton(button, p2);
        if (p2 || m_fields->injecting) return;
        m_fields->held = true;
        m_fields->left = Mod::get()->getSettingValue<int64_t>("frames");
    }

    void releaseButton(int button, bool p2) {
        if (!p2 && !m_fields->injecting) {
            m_fields->held = false;
            m_fields->left = 0;
        }
        GJBaseGameLayer::releaseButton(button, p2);
    }

    void showIndicator() {
        auto f = m_fields.self();
        if (!f->label) {
            f->label = CCLabelBMFont::create("", "bigFont.fnt");
            f->label->setScale(0.4f);
            f->label->setOpacity(0);
            auto size = CCDirector::get()->getWinSize();
            f->label->setPosition({size.width / 2, size.height - 20});
            this->addChild(f->label, 1000);
        }
        f->label->setString(fmt::format("Buffer x{}", f->corrected).c_str());
        f->label->stopAllActions();
        f->label->setOpacity(255);
        f->label->runAction(CCFadeOut::create(0.6f));
    }

    void processCommands(float dt) {
        GJBaseGameLayer::processCommands(dt);
        auto f = m_fields.self();
        f->step++;

        if (f->held && f->left > 0) {
            f->left--;
            f->injecting = true;
            GJBaseGameLayer::releaseButton(1, false);
            GJBaseGameLayer::pushButton(1, false);
            f->injecting = false;

            f->corrected++;
            if (Mod::get()->getSettingValue<bool>("log")) {
                log::info("Buffer: click repetido en paso {} (x = {:.1f}, quedan {})",
                    f->step, m_player1->getPositionX(), f->left);
            }
            if (Mod::get()->getSettingValue<bool>("indicator")) {
                this->showIndicator();
            }
        }
    }
};
