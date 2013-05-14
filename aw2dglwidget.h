#ifndef AW2DGLWIDGET_H
#define AW2DGLWIDGET_H

#include "cstring"

#include <QGLFunctions>
#include <QGLWidget>
#include <QImage>
#include <QTimer>
#include <QGLShaderProgram>


#include "camera2d.hpp"
#include "lightuniforms.h"
#include "lights.h"
#include "sprite.h"


//TODO Let Qt handle redraws.
//TODO Add a separate sprite drawing function.
//TODO FPS counter. Use QGLWidget text drawing API.
//TODO move lights with touchscreen
//TODO Move sprite

//TODO full sprite parsing; use http://yaml-online-parser.appspot.com/ to convert YAML to JSON


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
    
protected:
    void initializeGL();

    void paintGL();

    void resizeGL(int w, int h);

signals:

private slots:
    /// Trigger a repaint (new frame).
    void repaint();

private:
    /// When this times out, a new frame is started.
    QTimer* timer_;

    /// Camera used to view the scene.
    Camera2D camera_;

    /// Sprite used for testing.
    Sprite testSprite_;

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
};

#endif // AW2DGLWIDGET_H
