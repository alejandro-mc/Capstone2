#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QVector2D>
#include "shaderparam.h"

class ImageCanvas : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    ImageCanvas(QWidget *parent = 0);
    ~ImageCanvas();
    void setImage(QImage);

    //public functions to setup the new fragment shader to be used
    void defineFragShaderSrc(QLatin1String source_path);
    void defineFragShaderParameter(QString name, int type, void* defaulvalue);
    void useNewFragShader();//makes next shader the current shader
    //void setFragShaderParams(QVector<ShaderParameter>*);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    QOpenGLShaderProgram*     m_program;
    int                       m_mvMatLoc;   //ModelViewProjectionMatrix
    int                       m_projMatLoc;    //
    QOpenGLShader*            m_vertexShader;
    QOpenGLVertexArrayObject  m_vao;
    QOpenGLBuffer*            m_vbo_rec;        //vertex buffer for canvas rectangle
    QMatrix4x4                m_mvMat;
    QMatrix4x4                m_projMat;
    int                       m_scaleRecLoc;
    enum attribs              {a_Pos,a_Texcoord};
    QVector<GLfloat>          m_rec;           //holds the rectangle's vertex info
    QOpenGLTexture*           m_texture;
    int                       m_texhandleLoc;    //handle to the texture
    void                      createRec();
    QMatrix4x4                getScaleMatrix();
    GLuint                    m_textureID;
    QImage*                   m_Image;
    bool                      m_swapshader;
    void                      changeFragShader();
    QOpenGLShader*            m_tmp;
    void                      setAllParams();

    //current shader
    QVector<ShaderParameter>* m_parameters;
    QOpenGLShader*            m_fragmentShader;

    //next shader
    QVector<ShaderParameter>* m_nextparameters;
    QOpenGLShader*            m_nextfragmentShader;



};

#endif // IMAGECANVAS_H
