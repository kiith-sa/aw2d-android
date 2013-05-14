#ifndef LIGHTS_H
#define LIGHTS_H

#include <QVector3D>

/// A directional light source.
///
/// A directional light models a very distant light source (e.g. the Sun).
/// The source has no actual position, just a direction.
class DirectionalLight
{
private:
    /// Direction towards the light.
    QVector3D direction_;
    /// Diffuse color of the light.
    QVector3D diffuse_;

public:
    /// Construct a black directional light shining from upwards.
    DirectionalLight()
        : direction_(QVector3D(0, 0, 1))
        , diffuse_(QVector3D(0, 0, 0))
    {
    }

    /// Set the direction towards (NOT from) the light source.
    void setDirection(const QVector3D& rhs) {direction_ = rhs;}

    /// Set diffuse color of the light.
    void setDiffuse(const QVector3D& rhs) {diffuse_ = rhs;}

    /// Get the direction towards the light source.
    QVector3D direction() const {return direction_;}

    /// Get diffuse color of the light.
    QVector3D diffuse() const {return diffuse_;}
};

/// A point light source.
///
/// A point light has a position in 3D space and lits surrounding objects in a
/// sphere. The lighting is stronger on closer objects than on those that are more distant.
class PointLight
{
private:
    /// Positon of the light source.
    QVector3D position_;
    /// Diffuse color of the light.
    QVector3D diffuse_;
    /// Attenuation factor of the light.
    float attenuation_;

public:
    /// Construct a black point light at the origin with attenuation factor of 1.
    PointLight()
        : position_(QVector3D(0, 0, 0))
        , diffuse_(QVector3D(0, 0, 0))
        , attenuation_(1.0f)
    {
    }

    /// Set the position of the light source.
    void setPosition(const QVector3D& rhs) {position_ = rhs;}

    /// Set diffuse color of the light.
    void setDiffuse(const QVector3D& rhs) {diffuse_ = rhs;}

    /// Set the attenuation factor of the light.
    ///
    /// Increasing this value causes the light to weaken faster with distance.
    /// Values between 0 and 1 result in a larger area light. 0 completely removes
    /// the attenuation effect, making the light affect all objects, regardless of distance.
    ///
    /// Negative values will result in undefined behavior.
    void setAttenuation(const float rhs) {attenuation_ = rhs;}

    /// Get the position of the light source.
    QVector3D position() const {return position_;}

    /// Get diffuse color of the light.
    QVector3D diffuse() const {return diffuse_;}

    /// Get the attenuation factor of the light.
    float attenuation() const {return attenuation_;}
};

#endif // LIGHTS_H
