#include "ResourceManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFontDatabase>
#include <QUrl>
#include <QDebug>

ResourceManager::~ResourceManager()
{
    qDeleteAll(sounds_);
}

bool ResourceManager::load()
{
    missingTexture_ = QPixmap(32, 32);
    missingTexture_.fill(Qt::magenta);

    const QString root = resourcesRoot();
    const int fontId = QFontDatabase::addApplicationFont(root + "/TEXTS.ttf");
    if (fontId >= 0) {
        const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty()) {
            fontFamily_ = families.first();
        }
    }
    if (fontFamily_.isEmpty()) {
        fontFamily_ = QStringLiteral("Consolas");
        qWarning() << "Failed to load TEXTS.ttf; falling back to Consolas.";
    }

    loadTexture("layerGround", "Textures/LAYER_GROUND.png");
    loadTexture("layerCloud", "Textures/LAYER_CLOUD.png");
    loadTexture("layerMoon", "Textures/LAYER_MOON.png");
    loadTexture("layerStar", "Textures/LAYER_STAR.png");

    loadTexture("run1", "Textures/Run1.png");
    loadTexture("run2", "Textures/Run2.png");
    loadTexture("sprint1", "Textures/Sprint1.png");
    loadTexture("sprint2", "Textures/Sprint2.png");
    loadTexture("idle", "Textures/Idle.png");
    loadTexture("fail", "Textures/Fail.png");

    loadTexture("bird1", "Textures/Bird1.png");
    loadTexture("bird2", "Textures/Bird2.png");
    loadTexture("cactusBig1", "Textures/Cactus_BIG1.png");
    loadTexture("cactusBig2", "Textures/Cactus_BIG2.png");
    loadTexture("cactusMix", "Textures/Cactus_MIX.png");
    loadTexture("cactusSmall1", "Textures/Cactus_SMALL1.png");
    loadTexture("cactusSmall2", "Textures/Cactus_SMALL2.png");
    loadTexture("cactusSmall3", "Textures/Cactus_SMALL3.png");
    loadTexture("energy", "Textures/Energyball.png");
    loadTexture("fireball", "Textures/Fireball.png");

    loadTexture("heart1", "Textures/Heart1.png");
    loadTexture("heart2", "Textures/Heart2.png");
    loadTexture("heart3", "Textures/Heart3.png");
    loadTexture("stamia0", "Textures/Stamia0.png");
    loadTexture("stamia1", "Textures/Stamia1.png");
    loadTexture("stamia2", "Textures/Stamia2.png");
    loadTexture("stamia3", "Textures/Stamia3.png");

    loadSound("god", "Audios/GOD.wav");
    loadSound("hurt", "Audios/HURT.wav");
    loadSound("jump", "Audios/JUMP.wav");
    loadSound("score", "Audios/SCORE.wav");
    loadSound("shoot", "Audios/SHOOT.wav");

    return true;
}

const QPixmap &ResourceManager::texture(const QString &name) const
{
    auto it = textures_.constFind(name);
    if (it == textures_.constEnd() || it->isNull()) {
        return missingTexture_;
    }
    return it.value();
}

QString ResourceManager::fontFamily() const
{
    return fontFamily_;
}

void ResourceManager::play(const QString &name)
{
    QSoundEffect *effect = sounds_.value(name, nullptr);
    if (!effect) {
        return;
    }
    if (effect->isPlaying()) {
        effect->stop();
    }
    effect->play();
}

QString ResourceManager::resourcesRoot() const
{
    const QString appRoot = QCoreApplication::applicationDirPath() + "/Resources";
    if (QDir(appRoot).exists()) {
        return appRoot;
    }

    const QString sourceRoot = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../Resources");
    if (QDir(sourceRoot).exists()) {
        return sourceRoot;
    }

    return QDir::current().absoluteFilePath("Resources");
}

void ResourceManager::loadTexture(const QString &name, const QString &relativePath)
{
    QPixmap pixmap(resourcesRoot() + "/" + relativePath);
    if (pixmap.isNull()) {
        qWarning() << "Failed to load texture" << relativePath;
    }
    textures_.insert(name, pixmap);
}

void ResourceManager::loadSound(const QString &name, const QString &relativePath)
{
    auto *effect = new QSoundEffect;
    effect->setSource(QUrl::fromLocalFile(resourcesRoot() + "/" + relativePath));
    effect->setVolume(0.75f);
    sounds_.insert(name, effect);
}
