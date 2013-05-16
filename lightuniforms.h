#ifndef LIGHTUNIFORMS_H
#define LIGHTUNIFORMS_H

#include <QGLShaderProgram>

#include "uniform.h"
#include "lights.h"

/// Uniforms used for lighting in a 2D scene.
///
/// Manages light sources, sprite position, etc.
class LightingUniforms
{
public:
    /// Maximum number of directional lights supported.
    static const unsigned maxDirectionalLights = 2;

    /// Maximum number of point lights supported.
    static const unsigned maxPointLights = 2;

private:
    /// Shader program to use the uniforms with.
    QGLShaderProgram& shaderProgram_;

    /// Ambient light color.
    Uniform<QVector3D> ambientLight_;

    /// Directions of the directional light sources.
    Uniform<QVector3D, maxDirectionalLights> directionalDirections_;

    /// Diffuse colors of the directional light sources.
    Uniform<QVector3D, maxDirectionalLights> directionalDiffuse_;

    /// Positions of the point light sources.
    Uniform<QVector3D, maxPointLights> pointPositions_;

    /// Diffuse colors of the point light sources.
    Uniform<QVector3D, maxPointLights> pointDiffuse_;

    /// Attenuation factors of the point light sources.
    Uniform<float, maxPointLights> pointAttenuations_;

    /// Minimum object-space bounds of the current sprite.
    Uniform<QVector3D> spriteBoundsMin_;
    /// Maximum object-space bounds of the current sprite.
    Uniform<QVector3D> spriteBoundsMax_;

    /// 3D world-space sprite position.
    Uniform<QVector3D> spritePosition3D_;

    /// Is this LightUniforms correctly initialized?
    bool valid_;

public:
    /// Construct a LightUnifoorms using specified shader program to upload uniforms.
    ///
    /// @param shaderProgram Shader program to use. Must be bound when this is called.
    LightingUniforms(QGLShaderProgram& shaderProgram)
        : shaderProgram_(shaderProgram)
        , ambientLight_(shaderProgram.uniformLocation("ambientLight"))
        , directionalDirections_(shaderProgram.uniformLocation("directionalDirections"))
        , directionalDiffuse_(shaderProgram.uniformLocation("directionalDiffuse"))
        , pointPositions_(shaderProgram.uniformLocation("pointPositions"))
        , pointDiffuse_(shaderProgram.uniformLocation("pointDiffuse"))
        , pointAttenuations_(shaderProgram.uniformLocation("pointAttenuations"))
        , spriteBoundsMin_(shaderProgram.uniformLocation("spriteBoundsMin"))
        , spriteBoundsMax_(shaderProgram.uniformLocation("spriteBoundsMax"))
        , spritePosition3D_(shaderProgram.uniformLocation("spritePosition3D"))
        , valid_(false)
    {
        if(ambientLight_.handle() == -1) {return;}
        valid_ = true;
    }

    /// Has this LightUniforms structure been succesfully initialized?
    ///
    /// Might be false if a uniform could not be found in the shader program.
    ///
    /// Calls to all operations will be ignored if false.
    bool isValid() const 
    {
        return valid_;
    }

    /// Set the ambient light color (using 0.0 - 1.0 float values).
    void setAmbientLight(const QVector3D& color)
    {
        if(!isValid()){return;}
        ambientLight_.setValue(color);
    }

    /// Set the directional light at specified index.
    ///
    /// The number of directional light supported is specified by maxDirectionalLights.
    ///
    /// (Pass default-constructed DirectionalLight to disable the light.)
    void setDirectionalLight(const DirectionalLight& light, const unsigned index)
    {
        Q_ASSERT_X(index < maxDirectionalLights, "LightingUniforms::setDirectionalLight",
                   "Directional light index out of range");
        directionalDirections_.setValue(light.direction().normalized(), index);
        directionalDiffuse_.setValue(light.diffuse(), index);
    }

    /// Set the point light at specified index.
    ///
    /// The number of point light supported is specified by maxPointLights.
    ///
    /// (Pass default-constructed PointLight to disable the light.)
    void setPointLight(const PointLight& light, const unsigned index)
    {
        Q_ASSERT_X(index < maxPointLights, "LightingUniforms::setPointLight",
                   "Point light index out of range");
        pointPositions_.setValue(light.position(), index);
        pointDiffuse_.setValue(light.diffuse(), index);
        pointAttenuations_.setValue(light.attenuation(), index);
    }

    /// Set minimum and maximum 3D object-space bounds of the current sprite.
    void setSpriteBounds(const QVector3D& min, const QVector3D& max)
    {
        spriteBoundsMin_.setValue(min);
        spriteBoundsMax_.setValue(max);
    }

    /// Set 3D world-space position of the current sprite.
    void setSpritePosition(const QVector3D& position)
    {
        spritePosition3D_.setValue(position);
    }

    /// Reset the uniforms; forces all uniforms to be uploaded when updateUniforms() is called.
    ///
    /// Should be called after binding the shader program used.
    void reset()
    {
        if(!isValid()){return;}
        ambientLight_.reset();
        directionalDirections_.reset();
        directionalDiffuse_.reset();
        pointPositions_.reset();
        pointDiffuse_.reset();
        pointAttenuations_.reset();
        spriteBoundsMin_.reset();
        spriteBoundsMax_.reset();
        spritePosition3D_.reset();
    }

    /// Upload the uniforms that have changed since the last upload.
    void updateUniforms()
    {
        if(!isValid()){return;}
        ambientLight_.uploadIfNeeded(shaderProgram_);
        directionalDirections_.uploadIfNeeded(shaderProgram_);
        directionalDiffuse_.uploadIfNeeded(shaderProgram_);
        pointPositions_.uploadIfNeeded(shaderProgram_);
        pointDiffuse_.uploadIfNeeded(shaderProgram_);
        pointAttenuations_.uploadIfNeeded(shaderProgram_);
        spriteBoundsMin_.uploadIfNeeded(shaderProgram_);
        spriteBoundsMax_.uploadIfNeeded(shaderProgram_);
        spritePosition3D_.uploadIfNeeded(shaderProgram_);
    }
};


#endif // LIGHTUNIFORMS_H
