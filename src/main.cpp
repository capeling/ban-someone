#include <Geode/Geode.hpp>

using namespace geode::prelude;

static std::string person = "";
static int personAccountID = -1;

static constexpr bool isPerson(int accountID) {
    return accountID == personAccountID;
}

static void lowerString(std::string& str) {
    for (auto& c : str) {
        c = tolower(c);
    }
}

class UserGetter : public CCObject, public LevelManagerDelegate {
public:
    static UserGetter* create(const std::string& user) {
        auto ret = new UserGetter;
        ret->m_searchObject = GJSearchObject::create(SearchType::Users, user);
        GameLevelManager::get()->m_levelManagerDelegate = ret;
        GameLevelManager::get()->getUsers(ret->m_searchObject);
        return ret;
    }

    virtual void loadLevelsFinished(cocos2d::CCArray* levels, char const* key) {
        auto score = typeinfo_cast<GJUserScore*>(levels->objectAtIndex(0));
        if (!score) {
            invalidUser();
            return;
        }
        log::info("Found {} [{}]", score->m_userName, score->m_accountID);
        personAccountID = score->m_accountID;
        this->release();
    }

    virtual void loadLevelsFailed(char const* key) {
        invalidUser();
        this->release();
    }

    void invalidUser() {
        log::info("Couldnt find {}", person);
        Notification::create(fmt::format("[BAN SOMEONE MOD] Couldn't find {}!", person), NotificationIcon::Error)->show();
        personAccountID = -1;
    }

protected:
    Ref<GJSearchObject> m_searchObject = nullptr;
};

$on_mod(Loaded) {
    auto mod = Mod::get();

    person = mod->getSettingValue<std::string>("person");
    lowerString(person);
    UserGetter::create(person);

    listenForSettingChanges<std::string>("person", [](std::string v) {
        lowerString(v);
        person = v;
        UserGetter::create(v);
    });
}

#include <Geode/modify/ProfilePage.hpp>
class $modify(ProfilePage) {

    struct Fields {
        bool loadedPerson = false;
        bool isPerson = false;
    };

    void loadPersonProfile(int accountID) {
        auto fields = m_fields.self();
        if (fields->loadedPerson)
            return;

        fields->loadedPerson = true;

        if (!isPerson(accountID))
            return;

        fields->isPerson = true;

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
        loadPersonProfile(score->m_accountID);
    }
};

#include <Geode/modify/CCLabelBMFont.hpp>
class $modify(CCLabelBMFont) {
    bool initWithString(const char *str, const char *fntFile, float width, CCTextAlignment alignment, CCPoint imageOffset) {
        std::string newStr = str == nullptr ? "" : str;
        replacePerson(newStr);
        return CCLabelBMFont::initWithString(newStr.c_str(), fntFile, width, alignment, imageOffset);
    }

    void setString(const char *newString) {
        std::string newStr = newString == nullptr ? "" : newString;
        replacePerson(newStr);
        CCLabelBMFont::setString(newStr.c_str());
    }

    void replacePerson(std::string& string) {
        if (personAccountID <= 0)
            return;

        auto personLen = person.length();
        if (personLen == 0)
            return;

        std::string lowered = string;
        lowerString(lowered);

        size_t pos = 0;
        while ((pos = lowered.find(person, pos)) != std::string::npos) {
            if (personLen > 2) {
                string.replace(pos + 2, 2, "**");
            } else {
                string.replace(pos, 1, "*");
            }
            pos += personLen;
        }
    }
};

#include <Geode/modify/CommentCell.hpp>
class $modify(CommentCell) {
    void loadFromComment(GJComment* comment) {
        CommentCell::loadFromComment(comment);
        fixPersonValues();
    }

    void fixPersonValues() {
        if (!isPerson(m_comment->m_accountID))
            return;

        if (m_comment->m_likeCount <= 0)
            return;

        int newLikeCount = 0 - m_comment->m_likeCount * ((rand() % 6) + 1);

        if (m_likeLabel) {
            m_likeLabel->setString(fmt::format("{}", newLikeCount).c_str());
            m_likeLabel->limitLabelWidth(m_compactMode ? 15 : 28, m_compactMode ? 0.3f : 0.4f, 0.f);
        }

        auto newFrame = CCSpriteFrameCache::get()->spriteFrameByName("GJ_dislikesIcon_001.png");
        m_iconSprite->setDisplayFrame(newFrame);
    }

    void updateLabelValues() {
        CommentCell::updateLabelValues();
        fixPersonValues();
    }
};

#include <Geode/modify/LevelCell.hpp>
class $modify(LevelCell) {
    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
        fixPersonValues();
    }

    void fixPersonValues() {
        if (!isPerson(m_level->m_accountID))
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
        bool tryingToEnterStupidChudPersonLevel = false;
    };

    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();
        fixPersonValues();
    }

    void fixPersonValues() {
        if (!isPerson(m_level->m_accountID))
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
};
