#include <Geode/Geode.hpp>

using namespace geode::prelude;

#define RIFCT_ACC_ID 8927557
#define RIFCT_ALT_ACC_ID 32518396

static constexpr bool isRifct(int accountID) {
    return accountID == RIFCT_ACC_ID || accountID == RIFCT_ALT_ACC_ID;
}

#include <Geode/modify/ProfilePage.hpp>
class $modify(ProfilePage) {

    struct Fields {
        bool loadedRifct = false;
        bool isRifct = false;
    };

    void loadRifctProfile(int accountID) {
        auto fields = m_fields.self();
        if (fields->loadedRifct)
            return;

        fields->loadedRifct = true;

        if (!isRifct(accountID))
            return;

        fields->isRifct = true;

        // TODO: fix rifcts profile

        if (auto socialMenu = typeinfo_cast<CCMenu*>(m_mainLayer->getChildByID("socials-menu"))) {
            socialMenu->setOpacity(20);
        }

        if (auto socialHint = typeinfo_cast<CCSprite*>(m_mainLayer->getChildByID("my-stuff-hint"))) {
            socialHint->setOpacity(20);
        }

        if (auto rankLabel = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("global-rank-label"))) {
            rankLabel->setString("like 1 billion");
        }

        auto statsMenu = typeinfo_cast<CCMenu*>(m_mainLayer->getChildByID("stats-menu"));
        if (!statsMenu)
            return;

        if (auto starsLabel = typeinfo_cast<CCLabelBMFont*>(statsMenu->getChildByIDRecursive("stars-label"))) {
            starsLabel->setString("2");
            starsLabel->setColor({255, 0, 0});
            starsLabel->limitLabelWidth(50.f, 0.6f, 0.f);
            starsLabel->getParent()->setContentWidth(starsLabel->getScaledContentWidth());
        }

        if (auto moonsLabel = typeinfo_cast<CCLabelBMFont*>(statsMenu->getChildByIDRecursive("moons-label"))) {
            moonsLabel->setString("7");
            moonsLabel->setColor({255, 0, 0});
            moonsLabel->limitLabelWidth(50.f, 0.6f, 0.f);
            moonsLabel->getParent()->setContentWidth(moonsLabel->getScaledContentWidth());
        }

        if (auto gcoinsLabel = typeinfo_cast<CCLabelBMFont*>(statsMenu->getChildByIDRecursive("coins-label"))) {
            gcoinsLabel->setString("-1");
            gcoinsLabel->setColor({255, 0, 0});
            gcoinsLabel->limitLabelWidth(50.f, 0.6f, 0.f);
            gcoinsLabel->getParent()->setContentWidth(gcoinsLabel->getScaledContentWidth());
        }

        if (auto ucoinsLabel = typeinfo_cast<CCLabelBMFont*>(statsMenu->getChildByIDRecursive("user-coins-label"))) {
            ucoinsLabel->setString("5");
            ucoinsLabel->setColor({255, 0, 0});
            ucoinsLabel->limitLabelWidth(50.f, 0.6f, 0.f);
            ucoinsLabel->getParent()->setContentWidth(ucoinsLabel->getScaledContentWidth());
        }

        if (auto demonsLabel = typeinfo_cast<CCLabelBMFont*>(statsMenu->getChildByIDRecursive("demons-label"))) {
            demonsLabel->setString("0");
            demonsLabel->setColor({255, 0, 0});
            demonsLabel->limitLabelWidth(50.f, 0.6f, 0.f);
            demonsLabel->getParent()->setContentWidth(demonsLabel->getScaledContentWidth());
        }

        // 50.f 0.6f 0.f
        if (auto creatorLabel = typeinfo_cast<CCLabelBMFont*>(statsMenu->getChildByIDRecursive("creator-points-label"))) {
            creatorLabel->setString("-67");
            creatorLabel->setColor({255, 0, 0});
            creatorLabel->limitLabelWidth(50.f, 0.6f, 0.f);
            creatorLabel->getParent()->setContentWidth(creatorLabel->getScaledContentWidth());
        }

        statsMenu->updateLayout();

    }

    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);
        loadRifctProfile(score->m_accountID);
    }
};

#include <Geode/modify/CCLabelBMFont.hpp>
class $modify(CCLabelBMFont) {
    bool initWithString(const char *str, const char *fntFile, float width, CCTextAlignment alignment, CCPoint imageOffset) {
        std::string rifct = str == nullptr ? "" : str;
        replaceRifct(rifct);
        return CCLabelBMFont::initWithString(rifct.c_str(), fntFile, width, alignment, imageOffset);
    }

    void setString(const char *newString) {
        std::string rifct = newString == nullptr ? "" : newString;
        replaceRifct(rifct);
        CCLabelBMFont::setString(rifct.c_str());
    }

    std::string replaceRifct(std::string& string) {
        std::string lowered = string;
        for (auto& c : lowered) {
            c = std::tolower(c);
        }

        size_t pos = 0;
        while ((pos = lowered.find("rifct", pos)) != std::string::npos) {
            string.replace(pos + 2, 2, "**");
            pos += 5;
        }

        return string;
    }
};

#include <Geode/modify/CommentCell.hpp>
class $modify(CommentCell) {
    void loadFromComment(GJComment* comment) {
        CommentCell::loadFromComment(comment);
        fixRifctValues();
    }

    void fixRifctValues() {
        if (!isRifct(m_comment->m_accountID))
            return;

        if (m_comment->m_likeCount <= 0)
            return;

        int newLikeCount = 0 - m_comment->m_likeCount * 3;

        if (m_likeLabel) {
            m_likeLabel->setString(fmt::format("{}", newLikeCount).c_str());
            m_likeLabel->limitLabelWidth(m_compactMode ? 15 : 28, m_compactMode ? 0.3f : 0.4f, 0.f);
        }

        auto newFrame = CCSpriteFrameCache::get()->spriteFrameByName("GJ_dislikesIcon_001.png");
        m_iconSprite->setDisplayFrame(newFrame);
    }

    void updateLabelValues() {
        CommentCell::updateLabelValues();
        fixRifctValues();
    }
};

#include <Geode/modify/LevelCell.hpp>
class $modify(LevelCell) {
    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
        fixRifctValues();
    }

    void fixRifctValues() {
        if (!isRifct(m_level->m_accountID))
            return;

        int likeRatio = m_level->m_likes - m_level->m_dislikes;

        if (likeRatio <= 0)
            return;

        auto likeIcon = typeinfo_cast<CCSprite*>(m_mainLayer->getChildByID("likes-icon"));
        if (!likeIcon)
            return;

        auto likeLabel = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("likes-label"));
        if (!likeLabel)
            return;

        auto diffSprite = typeinfo_cast<GJDifficultySprite*>(m_mainLayer->getChildByIDRecursive("difficulty-sprite"));
        if (!diffSprite)
            return;

        auto likeStr = GameToolbox::intToShortString(0 - likeRatio);
        likeLabel->setString(likeStr.c_str());
        likeLabel->limitLabelWidth(45.f, m_compactView ? 0.3f : 0.4f, 0.f);

        auto dislikeFrame = CCSpriteFrameCache::get()->spriteFrameByName("GJ_dislikesIcon_001.png");
        likeIcon->setDisplayFrame(dislikeFrame);

        diffSprite->updateFeatureState(GJFeatureState::None);
    }
};

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(LevelInfoLayer) {
    struct Fields {
        bool tryingToEnterStupidChudRifctLevel = false;
    };

    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();
        fixRifctValues();
    }

    void fixRifctValues() {
        if (!isRifct(m_level->m_accountID))
            return;

        int likeRatio = m_level->m_likes - m_level->m_dislikes;

        if (likeRatio <= 0)
            return;
        
        auto likeStr = GameToolbox::pointsToString(0 - likeRatio);
        m_likesLabel->setString(likeStr.c_str());
        m_likesLabel->limitLabelWidth(60.f, 0.5f, 0.f);

        auto dislikeFrame = CCSpriteFrameCache::get()->spriteFrameByName("GJ_dislikesIcon_001.png");
        m_likesIcon->setDisplayFrame(dislikeFrame);

        m_difficultySprite->updateFeatureState(GJFeatureState::None);
    }

    void onPlay(CCObject* sender) {
        if (!isRifct(m_level->m_accountID)) {
            LevelInfoLayer::onPlay(sender);
            return;
        }

        if (m_fields->tryingToEnterStupidChudRifctLevel) {
            LevelInfoLayer::onPlay(sender);
            return;
        }

        createQuickPopup("Are you sure?", "This <cl>level</c> was made by <cr>Rifct</c>, are you <cy>sure</c> you want to play this?", "Cancel", "Yes", [this](FLAlertLayer* alert, bool btn2) {
            if (!btn2)
                return;

            m_fields->tryingToEnterStupidChudRifctLevel = true;
            LevelInfoLayer::onPlay(nullptr);
        });
    }
};
