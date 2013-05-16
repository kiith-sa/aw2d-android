#ifndef AW2DGLWIDGET_H
#define AW2DGLWIDGET_H

#include "cstring"

#include <QGLFunctions>
#include <QGLWidget>
#include <QImage>
#include <QElapsedTimer>
#include <QEvent>
#include <QTimer>
#include <QGLShaderProgram>


#include "camera2d.hpp"
#include "lightuniforms.h"
#include "lights.h"
#include "sprite.h"


//TODO ushort texcoords
//TODO interleaved vattributes. Ensure at least 4-byte alignment
//TODO Sprite pages (texture packing etc.)
//TODO full sprite parsing; use http://yaml-online-parser.appspot.com/ to convert YAML to JSON
//TODO map + benchmark from desktop


/// This widget manages the GL viewport as well as the GL context used for all GL operations.
class AW2DGLWidget : public QGLWidget, protected QGLFunctions
{
    Q_OBJECT

public:
    /// Construct with specified parent widget.
    ///
    /// Initializes shader, camera and most GL state.
    explicit AW2DGLWidget(QWidget *parent = 0);

    /// Destroy the GL widget, destoying the GL context and all graphics resources.
    ~AW2DGLWidget();

    virtual bool event(QEvent* event);

protected:
    virtual void initializeGL();

    virtual void paintGL();

    virtual void resizeGL(int w, int h);

    virtual void paintEvent(QPaintEvent* paintEvent);

signals:

private slots:

private:
    /// When this times out, a new frame is started.
    QTimer* renderTimer_;

    /// Used to determine time taken by a frame with high precision (nanoseconds).
    QElapsedTimer fpsTimer_;

    /// FPS (1.0 / frametime) averaged over the last second (-1 at startup).
    double fps_;

    /// How many frames have been rendered since the last FPS update?
    unsigned framesSinceFPSUpdate_;

    /// Camera used to view the scene.
    Camera2D camera_;

    /// Sprite used for benchmarking.
    Sprite testSprite_;

    /// Sprite used to display light sources.
    Sprite lightSprite_;

    /// Directional light (1) used for testing.
    DirectionalLight directional1_;
    /// Directional light (2) used for testing.
    DirectionalLight directional2_;
    /// Point light (1) used for testing.
    PointLight point1_;
    /// Point light (2) used for testing.
    PointLight point2_;

    /// True if the initialization has failed.
    ///
    /// Blue screen will be drawn if true.
    bool initFailed_;

    /// Shader program to draw 3D lit sprites with.
    QGLShaderProgram shaderProgram_;

    /// Manages uniforms used to upload the lights.
    LightingUniforms* lightingUniforms_;

    /// Position vertex attribute handle.
    int attributePosition_;
    /// Texture coordinate vertex attribute handle.
    int attributeTexCoord_;

    /// Draw all 3D-lit sprites.
    void drawScene();

    /// Draw a 3D-lit sprite.
    ///
    /// Must be called while shaderProgram_ is bound.
    ///
    /// @param sprite   Sprite to draw.
    /// @param position Position in 3D world space to draw at.
    void drawSprite(Sprite* sprite, const QVector3D& position);
};

#endif // AW2DGLWIDGET_H
