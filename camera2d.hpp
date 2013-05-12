#ifndef CAMERA2D_HPP
#define CAMERA2D_HPP

#include <QVector2D>
#include <QMatrix4x4>


/// Camera used in a 2D scene.
class Camera2D
{
    /// Position of the center of the camera in pixels (the point we're looking at).
    QVector2D center_;
    /// Size of the viewport in pixels.
    QVector2D viewportSize_;
    /// Current zoom level.
    float zoom_;

    /// Projection matrix to transform vertices from camera space to screen space.
    QMatrix4x4 projection_;

public:
    /// Construct a Camera2D with default parameters (center at (0,0), no zoom, 640x480).
    Camera2D()
        : center_(0, 0)
        , viewportSize_(640, 480)
        , zoom_(1.0f)
    {
        updateProjection();
    }

    /// Set position of the center of the camera in pixels (the point we're looking at).
    void center(const QVector2D rhs)
    {
        center_ = rhs;
        updateProjection();
    }

    /// Set size of the viewport in pixels. Both dimensions must be greater than zero.
    void viewportSize(const QVector2D rhs)
    {
        viewportSize_ = rhs;
        updateProjection();
    }

    /// Set camera zoom.
    ///
    /// Values greater than 1 result in a "closer" (pixelated) view.
    /// Values between 0 and 1 result in zooming out.
    /// Must be greater than zero.
    void zoom(const float rhs)
    {
        zoom_ = rhs;
        updateProjection();
    }

    /// Return the current projection matrix.
    const QMatrix4x4& projection() const
    {
        return projection_;
    }

private:
    /// Update the projection matrix (called when a camera parameter changes).
    void updateProjection()
    {
        const QVector2D zoomedHalfSize = (viewportSize_ * 0.5f) * (1.0f / zoom_);
        projection_.setToIdentity();
        projection_.ortho
            (center_.x() - zoomedHalfSize.x(), center_.x() + zoomedHalfSize.x(),
             center_.y() - zoomedHalfSize.y(), center_.y() + zoomedHalfSize.y(),
             -100.0f, 100.0f);
    }
};

#endif // CAMERA2D_HPP
