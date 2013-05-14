#include "aw2dglwidget.h"


#include <QPainter>
#include <QtOpenGL>

#include <cmath>

AW2DGLWidget::AW2DGLWidget(QWidget* parent)
    : QGLWidget
      (
          QGLFormat(
            QGL::AlphaChannel |
            QGL::DoubleBuffer |
            QGL::DepthBuffer |
            QGL::Rgba
            //QGL::SampleBuffers
          ),
          parent
      )
    , fps_(-1.0f)
    , framesSinceFPSUpdate_(0)
    , camera_()
    , testSprite_("spacestation-03-sw", QVector3D(-267.132, -267.132, -267.132),
                  QVector3D(267.132, 267.132, 267.132), *this)
    , initFailed_(false)
    , shaderProgram_()
{
    setAutoBufferSwap(false);
    renderTimer_ = new QTimer(this);
    // Caps us to at most 500 FPS.
    renderTimer_->start(2);
    connect(renderTimer_,SIGNAL(timeout()), this, SLOT(update()));

    setAutoFillBackground(false);

    // Dummy parameters, will be set in resizeGL().
    camera_.viewportSize(QVector2D(640, 480));
    camera_.center(QVector2D(0, 0));
    camera_.zoom(1.0f);

    // Initialize the shader.
    if(!shaderProgram_.addShaderFromSourceCode(QGLShader::Vertex,
        "attribute highp vec2 Position;\n"
        "attribute highp vec2 TexCoord;\n"
        "varying highp vec2 frag_TexCoord;\n"

        "// Position of the sprite's origin in pixel space.\n"
        "uniform highp vec3  spritePosition3D;\n"
        "// Orthographic projection projecting the sprite to the screen.\n"
        "uniform highp mat4  projection;\n"
        "const float PI = 3.14159265358979323846264;\n"
        "const float verticalAngle = PI / 3.0;\n"

        "// Get a rotation matrix that will rotate a vertex around the X axis by specified angle.\n"
        "mat4 xRotation(in float angle)\n"
        "{\n"
            "float cosAngle = cos(angle);\n"
            "float sinAngle = sin(angle);\n"
            "mat4 xRot = mat4(1.0);\n"
            "xRot[1][1] = cosAngle;\n"
            "xRot[1][2] = -sinAngle;\n"
            "xRot[2][1] = sinAngle;\n"
            "xRot[2][2] = cosAngle;\n"
            "return xRot;\n"
        "}\n"

        "// Get a rotation matrix that will rotate a vertex around the Z axis by specified angle.\n"
        "mat4 zRotation(in float angle)\n"
        "{\n"
            "float cosAngle = cos(angle);\n"
            "float sinAngle = sin(angle);\n"
            "mat4 zRot = mat4(1.0);\n"
            "zRot[0][0] = cosAngle;\n"
            "zRot[0][1] = -sinAngle;\n"
            "zRot[1][0] = sinAngle;\n"
            "zRot[1][1] = cosAngle;\n"
            "return zRot;\n"
        "}\n"

        "void main (void)\n"
        "{\n"
            "mat4 view = xRotation(PI / 2.0 - verticalAngle) * zRotation(PI / 4.0);\n"
            "frag_TexCoord = TexCoord;\n"
            "vec2 spritePosition2D = vec2(view * vec4(spritePosition3D, 1.0));\n"
            "vec2 pos2D = Position + spritePosition2D;\n"
            "gl_Position           = projection * vec4(pos2D, 0.0, 1.0);\n"
        "}\n"
        ))
    {
        qDebug() << "Vertex shader compilation failed:" << shaderProgram_.log();
        initFailed_ = true;
        return;
    }

    if(!shaderProgram_.addShaderFromSourceCode(QGLShader::Fragment,
        "uniform sampler2D texDiffuse;\n"
        "uniform sampler2D texNormal;\n"
        "uniform sampler2D texOffset;\n"
        "// Position of the sprite's origin in pixel space.\n"
        "uniform highp vec3 spritePosition3D;\n"
        "varying highp vec2 frag_TexCoord;\n"
        "\n"
        "// Ambient light\n"
        "uniform highp vec3 ambientLight;\n"
        "\n"
        "// Directional lights\n"
        "const int directionalLightCount = 2;\n"
        "uniform highp vec3 directionalDirections[directionalLightCount];\n"
        "// Diffuse colors for unused light sources must be black.\n"
        "uniform highp vec3 directionalDiffuse[directionalLightCount];\n"
        "\n"
        "// Point lights\n"
        "const int pointLightCount = 2;\n" //"const int pointLightCount = 6;\n"
        "uniform highp vec3  pointPositions[pointLightCount];\n"
        "// Diffuse colors for unused light sources must be black.\n"
        "uniform highp vec3  pointDiffuse[pointLightCount];\n"
        "uniform highp float pointAttenuations[pointLightCount];\n"
        "\n"
        "// 3D bounding box of the sprite.\n"
        "uniform highp vec3 spriteBoundsMin;\n"
        "uniform highp vec3 spriteBoundsMax;\n"
        "\n"
        "highp vec3 directionalLighting(in int light, in highp vec3 pixelDiffuse, in highp vec3 pixelNormal)\n"
        "{\n"
            "highp vec3 reflectedColor = directionalDiffuse[light] * pixelDiffuse;\n"
            "return reflectedColor * \n"
                "max(0.0, dot(pixelNormal, directionalDirections[light]));\n"
        "}\n"
        "\n"
        "highp vec3 directionalLightingTotal(in highp vec3 pixelDiffuse, in highp vec3 pixelNormal)\n"
        "{\n"
            "highp vec3 result = vec3(0.0, 0.0, 0.0);\n"
            "// Loop unrolled to avoid branches. \n"
            "// Unused light sources have black color so they won't affect the result.\n"
            "result += directionalLighting(0, pixelDiffuse, pixelNormal);\n"
            "result += directionalLighting(1, pixelDiffuse, pixelNormal);\n"
            "return result;\n"
        "}\n"
        "\n"
        "highp vec3 pointLighting(in int light, in highp vec3 pixelDiffuse, in highp vec3 pixelNormal,\n"
                           "in highp vec3 pixelPosition)\n"
        "{\n"
            "highp vec3 posToLight = pointPositions[light] - pixelPosition;\n"
            "highp float distance  = length(posToLight);\n"
            "// Normalize lightDirection; no need to compute sqrt again.\n"
            "highp vec3 lightDirection = posToLight / distance;\n"
            "\n"
            "// 128 distance units (pixels in our case) are one lighting distance unit.\n"
            "// Avoids needing to use extremely high attenuation values.\n"
            "// Linear attenuation for now. TODO quadratic once we get HDR.\n"
            "highp float attenuationFactor = 1.0 / (1.0 + pointAttenuations[light] * (distance / 128.0));\n"
            "highp vec3 reflectedColor = pointDiffuse[light] * pixelDiffuse;\n"
            "return attenuationFactor * reflectedColor * max(0.0, dot(pixelNormal, lightDirection));\n"
        "}\n"
        "\n"
        "highp vec3 pointLightingTotal(in highp vec3 pixelDiffuse, in highp vec3 pixelNormal, in highp vec3 pixelPosition)\n"
        "\n"
        "{\n"
            "highp vec3 result = vec3(0.0, 0.0, 0.0);\n"
            "// Loop unrolled to avoid branches. \n"
            "// Unused light sources have black color so they won't affect the result.\n"
            "result += pointLighting(0, pixelDiffuse, pixelNormal, pixelPosition);\n"
            "result += pointLighting(1, pixelDiffuse, pixelNormal, pixelPosition);\n"
            "\n"
            "return result;\n"
        "}\n"
        "\n"
        "void main (void)\n"
        "{\n"
            "// Map offset coordinates from [0.0, 1.0] to [minOffsetBounds, maxOffsetBounds]\n"
            "// and add that to sprite position to get position of the pixel.\n"
            "highp vec3 offset   = vec3(texture2D(texOffset, frag_TexCoord));\n"
            "highp vec3 position = spritePosition3D +\n"
                                  "spriteBoundsMin + + (spriteBoundsMax - spriteBoundsMin) * offset;\n"
            "\n"
            "\n"
            "// Color of the sprite.\n"
            "highp vec3 diffuse  = vec3(texture2D(texDiffuse, frag_TexCoord));\n"
            "// We preserve transparency of the sprite.\n"
            "highp float alpha   = texture2D(texDiffuse, frag_TexCoord).a;\n"
            "// Map the normal coordinates from [0.0, 1.0] to [-1.0, 1.0]\n"
            "highp vec3 normal   = vec3(texture2D(texNormal, frag_TexCoord));\n"
            "normal        = normal * 2.0 - vec3(1.0, 1.0, 1.0);\n"
            "\n"
            "// Add up lighting from all types of light sources.\n"
            "highp vec3 totalLighting = ambientLight * diffuse\n"
                                     "+ directionalLightingTotal(diffuse, normal)\n"
                                     "+ pointLightingTotal(diffuse, normal, position);\n"
            "\n"
            "gl_FragColor = vec4(totalLighting, alpha);\n"
        "}\n"
        ))
    {
        qDebug() << "Fragment shader compilation failed:" << shaderProgram_.log();
        initFailed_ = true;
        return;
    }

    if(!shaderProgram_.link())
    {
        qDebug() << "Shader linking failed:" << shaderProgram_.log();
        initFailed_ = true;
        return;
    }

    // Bind to get uniform locations
    shaderProgram_.bind();

    attributePosition_ = shaderProgram_.attributeLocation("Position");
    attributeTexCoord_ = shaderProgram_.attributeLocation("TexCoord");
    if(attributePosition_  == -1 || attributeTexCoord_ == -1)
    {
        qDebug() << "Failed to get vertex attribute handles; maybe an error in shader?";
        qDebug() << shaderProgram_.log();
        initFailed_ = true;
        shaderProgram_.release();
        return;
    }

    lightingUniforms_ = new LightingUniforms(shaderProgram_);
    if(!lightingUniforms_->isValid())
    {
        qDebug() << "LightingUniforms initialization failed. Missing uniform in shader?";
        qDebug() << shaderProgram_.log();
        initFailed_ = true;
        shaderProgram_.release();
        return;
    }
    shaderProgram_.release();

    fpsTimer_.start();
}

AW2DGLWidget::~AW2DGLWidget()
{
    delete lightingUniforms_;
}

void AW2DGLWidget::initializeGL()
{
    if(initFailed_){return;}

    lightingUniforms_->setAmbientLight(QVector3D(0.0, 0.0, 0.2));

    // Set directional lights.
    DirectionalLight directional1;
    DirectionalLight directional2;
    directional1.setDirection(QVector3D(0, 0, 0.5));
    directional2.setDirection(QVector3D(0.5, 0, 0));
    directional1.setDiffuse(QVector3D(0.5, 0, 0));
    directional2.setDiffuse(QVector3D(0, 0.5, 0));
    lightingUniforms_->setDirectionalLight(directional1, 0);
    lightingUniforms_->setDirectionalLight(directional2, 1);

    // Set point lights.
    PointLight point1;
    PointLight point2;
    point1.setPosition(QVector3D(0, 0, 0));
    point2.setPosition(QVector3D(-128, -128, 128));
    point1.setDiffuse(QVector3D(1, 1, 0));
    point2.setDiffuse(QVector3D(0, 1, 1));
    point1.setAttenuation(0.5f);
    point2.setAttenuation(0.3f);
    lightingUniforms_->setPointLight(point1, 0);
    lightingUniforms_->setPointLight(point2, 1);
}

void AW2DGLWidget::paintGL()
{
    qglClearColor(QColor(255, 0, 0));
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // In case of bugs, call makeCurrent() (not calling it because overhead). 
    if(initFailed_)
    {
        // Blue signals broken GL (BSOD)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        qglClearColor(QColor(0, 0, 255));
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawScene();
}

void AW2DGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    camera_.viewportSize(QVector2D(w, h));
    camera_.center(QVector2D(0, 0));
}

void AW2DGLWidget::paintEvent(QPaintEvent* paintEvent)
{
    QGLWidget::paintEvent(paintEvent);

    QPainter painter(this);
    QPen pen(QColor(Qt::black));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setFont(QFont("Sans", 16, QFont::Black));
    painter.drawText(QRect(0, 48, width(), height()),
                     Qt::AlignTop | Qt::AlignHCenter, "FPS: " + QString::number(fps_));
    swapBuffers();

    ++framesSinceFPSUpdate_;
    const double secondsSinceFPSUpdate = fpsTimer_.nsecsElapsed() / 1000000000.0;
    if(secondsSinceFPSUpdate > 1.0)
    {
        fps_ = framesSinceFPSUpdate_ / secondsSinceFPSUpdate;
        framesSinceFPSUpdate_ = 0;
        fpsTimer_.restart();
    }
}

void AW2DGLWidget::drawScene()
{
    shaderProgram_.bind();
    lightingUniforms_->reset();

    // Set texture units and the projection matrix.
    shaderProgram_.setUniformValue("texDiffuse", 0);
    shaderProgram_.setUniformValue("texNormal",  1);
    shaderProgram_.setUniformValue("texOffset",  2);
    shaderProgram_.setUniformValue("projection", camera_.projection());

    // Draw the test sprite.
    if(testSprite_.isValid())
    {
        lightingUniforms_->setSpriteBounds(testSprite_.boundsMin(), testSprite_.boundsMax());
        lightingUniforms_->setSpritePosition(QVector3D(0, 0, 0));
        lightingUniforms_->updateUniforms();

        testSprite_.bind();
        // Set texture units.
        shaderProgram_.enableAttributeArray(attributePosition_);
        shaderProgram_.enableAttributeArray(attributeTexCoord_);

        // Create vertex data to draw the sprite with.
        // This should be part of a sprite page later.
        QVector<QVector2D> positions;
        QVector<QVector2D> texCoords;

        // Align to pixel boundaries.
        const QVector2D size = testSprite_.pixelSize();
        const float xMin = static_cast<int>(-size.x() / 2);
        const float yMin = static_cast<int>(-size.y() / 2);
        const float xMax = xMin + size.x();
        const float yMax = yMin + size.y();

        positions.append(QVector2D(xMin, yMin)); texCoords.append(QVector2D(0, 0));
        positions.append(QVector2D(xMax, yMin)); texCoords.append(QVector2D(1, 0));
        positions.append(QVector2D(xMax, yMax)); texCoords.append(QVector2D(1, 1));
        positions.append(QVector2D(xMin, yMin)); texCoords.append(QVector2D(0, 0));
        positions.append(QVector2D(xMax, yMax)); texCoords.append(QVector2D(1, 1));
        positions.append(QVector2D(xMin, yMax)); texCoords.append(QVector2D(0, 1));

        shaderProgram_.setAttributeArray(attributePosition_, positions.constData());
        shaderProgram_.setAttributeArray(attributeTexCoord_, texCoords.constData());
        glDrawArrays(GL_TRIANGLES, 0, positions.size());

        shaderProgram_.disableAttributeArray(attributeTexCoord_);
        shaderProgram_.disableAttributeArray(attributePosition_);
    }
    shaderProgram_.release();
}
