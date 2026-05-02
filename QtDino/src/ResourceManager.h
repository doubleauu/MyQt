#pragma once

#include <QHash>
#include <QPixmap>
#include <QSoundEffect>
#include <QString>

class ResourceManager
{
public:
    ~ResourceManager();

    bool load();

    const QPixmap &texture(const QString &name) const;
    QString fontFamily() const;
    void play(const QString &name);

private:
    QString resourcesRoot() const;
    void loadTexture(const QString &name, const QString &relativePath);
    void loadSound(const QString &name, const QString &relativePath);

    QHash<QString, QPixmap> textures_;
    QHash<QString, QSoundEffect *> sounds_;
    QString fontFamily_;
    QPixmap missingTexture_;
};
