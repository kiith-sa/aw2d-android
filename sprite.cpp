#include "sprite.h"
#include "aw2dmath.h"

Sprite::Sprite(const QString& name, QGLWidget& glWidget)
   : imageDiffuse_("assets:/demo_data/sprites/" + name + "_diffuse_0.png")
   , imageNormal_("assets:/demo_data/sprites/" + name + "_normal_0.png")
   , imageOffset_("assets:/demo_data/sprites/" + name + "_offset_0.png")
   , valid_(false)
   , creatorWidget_(glWidget)
{
    // All images must be successfully loaded
    if(imageDiffuse_.isNull())
    {
        qDebug() << "Failed to load diffuse image for sprite " << name;
        return;
    }
    if(imageNormal_.isNull())
    {
        qDebug() << "Failed to load normal image for sprite " << name;
        return;
    }
    if(imageOffset_.isNull())
    {
        qDebug() << "Failed to load offset image for sprite " << name;
        return;
    }

    // Image sizes must match
    if(!(imageDiffuse_.size() == imageNormal_.size() &&
         imageNormal_.size() == imageOffset_.size()))
    {
        qDebug() << "Image sizes in sprite " << name << " don't match";
        return;
    }
    // Image sizes must be powers of two
    if(!isPowerOfTwo(imageDiffuse_.size().width()) ||
       !isPowerOfTwo(imageDiffuse_.size().height()))
    {
        qDebug() << "Non-power of two texture size for sprite " << name 
                 << " - not supported yet";
        return;
    }

    // Upload to GL textures.
    textureDiffuse_ = glWidget.bindTexture(imageDiffuse_, GL_TEXTURE_2D, GL_RGBA, 
                                           QGLContext::InvertedYBindOption);
    textureNormal_  = glWidget.bindTexture(imageNormal_,  GL_TEXTURE_2D, GL_RGB,
                                           QGLContext::InvertedYBindOption);
    textureOffset_  = glWidget.bindTexture(imageOffset_,  GL_TEXTURE_2D, GL_RGB,
                                           QGLContext::InvertedYBindOption);


    // Check that the textures were uploaded successfully.
    if(textureDiffuse_ == 0)
    {
        qDebug() << "Failed to create diffuse texture for sprite " << name;
        return;
    }
    if(textureNormal_ == 0)
    {
        qDebug() << "Failed to create normal texture for sprite " << name;
        return;
    }
    if(textureOffset_ == 0)
    {
        qDebug() << "Failed to create offset texture for sprite " << name;
        return;
    }

    valid_ = true;
}
