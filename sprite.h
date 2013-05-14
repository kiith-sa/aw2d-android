#ifndef SPRITE_H

#define SPRITE_H

#include <QGLContext>
#include <QtOpenGL>
#include <QImage>
#include <QVector2D>
#include <QVector3D>


/// A simple 2D sprite with lighting support.
///
/// No animations or multiple facings.
class Sprite
{
private:
    /// Color layer image.
    QImage imageDiffuse_;
    /// Normal layer image.
    QImage imageNormal_;
    /// Offset layer image.
    QImage imageOffset_;

    /// Color layer texture.
    GLuint textureDiffuse_;
    /// Normal layer texture.
    GLuint textureNormal_;
    /// Offset layer texture.
    GLuint textureOffset_;

    /// Minimum 3D bounds of the sprite (for offsets).
    QVector3D boundsMin_;

    /// Maximum 3D bounds of the sprite (for offsets).
    QVector3D boundsMax_;

    /// Size of the sprite in pixels.
    QVector2D pixelSize_;

    /// Is this sprite valid (i.e. did loading succeed?)
    bool valid_;

    /// Widget that created this sprite. Should only be used for texture deletion in destructor.
    QGLWidget& creatorWidget_;

public:
    /// Load a sprite.
    ///
    /// @param name      Base name of sprite layer images in 'demo_data/sprites/'.
    /// @param boundsMin Minimum 3D bounds of the sprite (for offsets).
    /// @param boundsMax Maximum 3D bounds of the sprite (for offsets).
    /// @param glWidget  GL widget to create textures with. The sprite MUST be destroyed before
    ///                  this GL widget.
    Sprite(const QString& name, const QVector3D& boundsMin, const QVector3D& boundsMax,
           QGLWidget& glWidget);

    /// Destroy the sprite.
    ~Sprite()
    {
        if(textureDiffuse_ != 0){creatorWidget_.deleteTexture(textureDiffuse_);}
        if(textureNormal_  != 0){creatorWidget_.deleteTexture(textureNormal_);}
        if(textureOffset_  != 0){creatorWidget_.deleteTexture(textureOffset_);}
    }

    /// Is the sprite valid; i.e. has it been succesfully loaded?
    bool isValid() const {return valid_;}

    /// Get minimum 3D bounds of the sprite (for offsets).
    QVector3D boundsMin() const {return boundsMin_;}

    /// Get maximum 3D bounds of the sprite (for offsets).
    QVector3D boundsMax() const {return boundsMax_;}

    /// Get the size of the sprite in pixels.
    QVector2D pixelSize() const {return pixelSize_;}

    /// Bind the sprite. 
    ///
    /// The GL context of the widget used when loading this sprite must be current.
    void bind()
    {
        if(!valid_){return;}
        glActiveTexture(GL_TEXTURE0 + 0); glBindTexture(GL_TEXTURE_2D, textureDiffuse_);
        glActiveTexture(GL_TEXTURE0 + 1); glBindTexture(GL_TEXTURE_2D, textureNormal_);
        glActiveTexture(GL_TEXTURE0 + 2); glBindTexture(GL_TEXTURE_2D, textureOffset_);
    }
};

#endif // SPRITE_H
