#include "imagecanvas.h"

#include <QMouseEvent>


ImageCanvas::ImageCanvas(QWidget *parent) : QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setMajorVersion(4);
    setFormat(format);
    m_identity = true;
    m_swapshader = false;
    m_parameters = NULL;
    m_nextparameters = NULL;

}

void
ImageCanvas::initializeGL(){
    initializeOpenGLFunctions();
    glClearColor(0,0,0,1);

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    //intialize the texture -- this will be done at runtime at a later stage
    //m_texture = new QOpenGLTexture(QImage(QString(":/images/mandrill_orig.png")).mirrored());

    m_Image = new QImage(4,4,QImage::Format_ARGB32);
    m_Image->fill(Qt::white);
    QImage tex = m_Image->convertToFormat(QImage::Format_ARGB32);


    //Create OpenGL Texture
    f->glGenTextures(1,&m_textureID);

    //bind texture
    f->glBindTexture(GL_TEXTURE_2D,m_textureID);
    f->glEnable(GL_TEXTURE_2D);

    //pass image to opengl
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width(), tex.height(), 0,
        GL_BGRA, GL_UNSIGNED_BYTE, tex.mirrored().bits());

    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glBindTexture(GL_TEXTURE_2D, 0);



    //create vertices data for the image rectangle
    createRec();

    //create opengl program
    m_program = new QOpenGLShaderProgram;


    //create and compile shaders
    m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    m_vertexShader->compileSourceFile(QLatin1String(":/glcanvas.vert"));


    m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    m_fragmentShader->compileSourceFile(QLatin1String(":/glcanvas.frag"));


    //add shaders to program
    m_program->addShader(m_vertexShader);
    m_program->addShader(m_fragmentShader);

    //bind attribute locations
    m_program->bindAttributeLocation("a_Pos",attribs::a_Pos);
    m_program->bindAttributeLocation("a_Texcoord",attribs::a_Texcoord);


    //link progam
    m_program->link();

    //bind program
    m_program->bind();//equivalent to glUseProgram

    //get uniform locations
    m_mvMatLoc       = m_program->uniformLocation("u_ModelView");
    m_projMatLoc     = m_program->uniformLocation("u_proj");
    m_texhandleLoc   = m_program->uniformLocation("texhandle");
    m_scaleRecLoc    = m_program->uniformLocation("u_scaleRec");

    m_program->release();

    //create vao
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    //allocate vertex buffer object
    m_vbo_rec = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo_rec->create();//GenBuffer
    m_vbo_rec->bind();//BindBuffer
    m_vbo_rec->allocate(m_rec.constData(), m_rec.count() * sizeof(GLfloat));//like BufferData


    //Store vertex attribute bindings for the program
    //m_vbo_rec->bind();
    //QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(attribs::a_Pos);
    f->glVertexAttribPointer(attribs::a_Pos, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    f->glEnableVertexAttribArray(attribs::a_Texcoord);
    //tex coordinates start at 2 and stride of 2 glfloats
    f->glVertexAttribPointer(attribs::a_Texcoord,2,GL_FLOAT,GL_FALSE,
                             4 * sizeof(GLfloat),(GLfloat *)NULL + 2);

    m_vbo_rec->release();


    //after finished rendering release the program
    //m_program->release();


}

void
ImageCanvas::resizeGL(int w, int h){
    m_projMat.setToIdentity();

    float ratio =1;

    //keep aspect ratio
    if(w<h){
        ratio = float(h) / float(w);
        m_projMat.ortho(-1,1,-1 * ratio,1 * ratio,0,1);
    }else{
        ratio = float(w)/ float(h);
        m_projMat.ortho(-1 * ratio,1 * ratio,-1,1,0,1);

    }

}


void
ImageCanvas::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    qDebug("Painting...");

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    //set up the model view matrix
    m_mvMat.setToIdentity();
    m_mvMat.translate(0,0,-1);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    //debugging
    if(m_swapshader)
    {
        changeFragShader();
    }
    //end debugging


    m_program->bind();//use the opengl program


    //reset uniform variables
    m_program->setUniformValue(m_mvMatLoc,m_mvMat);
    m_program->setUniformValue(m_projMatLoc,m_projMat);
    m_program->setUniformValue(m_texhandleLoc,0);//this is for the sampler
    m_program->setUniformValue(m_scaleRecLoc,getScaleMatrix());

    //set (in the GPU) parameter values (uniform values that are effect specific)
    setAllParams();


    //bind texture
    f->glBindTexture(GL_TEXTURE_2D,m_textureID);
    f->glEnable(GL_TEXTURE_2D);

    f->glDrawArrays(GL_TRIANGLE_FAN,0,4);

    //unbind texture
    f->glBindTexture(GL_TEXTURE_2D, 0);
    f->glDisable(GL_TEXTURE_2D);

    m_program->release();

}



void
ImageCanvas::createRec(){

    //vertex 1
    m_rec.append(-1.0); //x
    m_rec.append(1.0);  //y
    m_rec.append(0.0);  //s
    m_rec.append(1.0);  //t

    //vertex 2
    m_rec.append(1.0);  //x
    m_rec.append(1.0);  //y
    m_rec.append(1.0);  //s
    m_rec.append(1.0);  //t

    //vertex 3
    m_rec.append(1.0);  //x
    m_rec.append(-1.0); //y
    m_rec.append(1.0);  //s
    m_rec.append(0.0);  //t

    //vertex 4
    m_rec.append(-1.0); //x
    m_rec.append(-1.0); //y
    m_rec.append(0.0);  //s
    m_rec.append(0.0);  //t
}


///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// void setAllParams
///
///Sets (in the GPU) all the uniform values other than the texture sampler
///that are associated with the current image shader
void
ImageCanvas::setAllParams(){

    if(m_parameters == NULL)
    {
        return;
    }

    for(int i=0;i<m_parameters->count();i++)
    {
        //check parameter type and send its value to the shader
        int type = m_parameters->at(i).type;
        void* val = m_parameters->at(i).value;
        switch(type){
        case ParamType::Int:
            m_program->setUniformValue(m_parameters->at(i).location,
                                       *((int*)m_parameters->at(i).value));
        case ParamType::Uint:
            m_program->setUniformValue(m_parameters->at(i).location,
                                       *((uint*)m_parameters->at(i).value));
        case ParamType::Float:
            m_program->setUniformValue(m_parameters->at(i).location,
                                       *((float*)m_parameters->at(i).value));
            qDebug()<<"Shader Threshold: "<< *((float*)m_parameters->at(i).value);
        }

    }
}


/// ImageCanvas::setImage
///
/// uses image as the new texture and resizes rectangle
/// according to image's proportions to avoid distortion
void
ImageCanvas::setImage(QImage image){

    //get current context
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();


    //unbind texture
    f->glBindTexture(GL_TEXTURE_2D,0);
    f->glDisable(GL_TEXTURE_2D);

    //delete old texture
    f->glDeleteTextures(1,&m_textureID);

    m_Image = new QImage(image);
    QImage tex = m_Image->convertToFormat(QImage::Format_ARGB32);


    //Create OpenGL Texture
    f->glGenTextures(1,&m_textureID);

    //bind texture
    f->glBindTexture(GL_TEXTURE_2D,m_textureID);
    f->glEnable(GL_TEXTURE_2D);

    //pass image to opengl
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width(), tex.height(), 0,
        GL_BGRA, GL_UNSIGNED_BYTE, tex.mirrored().bits());

    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glBindTexture(GL_TEXTURE_2D, 0);


}

///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///getScaleMatrix
///
/// returns the scale transformation matrix to
/// change the canvas rectangle's proportions to fit
/// those of the input image
QMatrix4x4
ImageCanvas::getScaleMatrix(){
    int w = m_Image->width();
    int h = m_Image->height();

    QMatrix4x4 scaleMat;

    scaleMat.setToIdentity();

    if(w>h){
        scaleMat.scale(1.0,(float) h / (float) w,0.1);
    }else{
        scaleMat.scale((float) w / (float) h,1.0,1.0);
    }

    return scaleMat;
}


///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///void defineFragShaderSrc
///
/// creates and compiles the new fragment shader
void
ImageCanvas::defineFragShaderSrc(QLatin1String source_path){
    //create and compile the fragment shader
    m_nextfragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    m_nextfragmentShader->compileSourceFile(source_path);
}


///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///void defineFragmentShaderParamter
///
/// adds a ShaderParameter to the vector of next shader parameters
/// m_nextparameters.
void
ImageCanvas::defineFragShaderParameter(QString name, int type, void* defaulvalue){

    if(m_nextparameters == NULL){

        //create m_nextparameters
        m_nextparameters = new QVector<ShaderParameter>();
    }

    //define parameter
    ShaderParameter newparam;
    newparam.name = name;
    newparam.type = type;
    newparam.value = defaulvalue;

    //add parameter definition to m_nextparameters
    m_nextparameters->append(newparam);
}


///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// void useFragmentShader
///
/// swaps the current fragment shader with the new fragment shader
/// and it's parameters
void
ImageCanvas::useNewFragShader(){
    m_swapshader = true;

    //set new shader parameters
    QVector<ShaderParameter>* tmp = m_parameters;
    m_parameters = m_nextparameters;
    delete tmp;

    m_nextparameters = NULL;

    //channge the fragment shader

    //store in temp shader
    m_tmp = m_fragmentShader;

    //create new shader
    m_fragmentShader = m_nextfragmentShader;
    m_nextfragmentShader = NULL;

    //set identity flag to false
    m_identity = false;

    //repaint the widget so paintGL is called
    repaint();
}


///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// void setIdentityFragShader
///
/// sets the current fragment shader to one that performs an identity map
/// i.e. the output is the same as the input
void
ImageCanvas::setIdentityFragShader()
{
    defineFragShaderSrc(QLatin1String(":/glcanvas.frag"));
    //no parameters to define
    useNewFragShader();

    //set identity flag to false
    m_identity = true;
}


///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// bool isIdentitySet
///
/// returns true if the identity fragment shader is set to true, false otherwise
bool
ImageCanvas::isIdentitySet(){
    return m_identity;
}

///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
///ImageCanvas::changeFragmentShader
///
///adds new fragment shader, relinks the opengl shader program
///and gets uniform locations

void
ImageCanvas::changeFragShader(){


    m_program->removeAllShaders();

    //create and compile shaders
    m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    m_vertexShader->compileSourceFile(QLatin1String(":/glcanvas.vert"));

    //new fragment shader is created and compiled in the setFilter function


    //add shaders to program
    if(m_program->addShader(m_vertexShader)){
        qDebug("Vertex shader was added");//**DEBUG**
    }
    if(m_program->addShader(m_fragmentShader)){
    qDebug("Fragment shader was added.");//**DEBUG**
    }

    delete m_tmp;//delete old fragment shader after adding new one
    m_tmp = NULL;

    //bind attribute locations
    m_program->bindAttributeLocation("a_Pos",attribs::a_Pos);
    m_program->bindAttributeLocation("a_Texcoord",attribs::a_Texcoord);


    //link progam
    m_program->link();

    //bind program
    m_program->bind();//equivalent to glUseProgram

    //get uniform locations
    m_mvMatLoc       = m_program->uniformLocation("u_ModelView");
    m_projMatLoc     = m_program->uniformLocation("u_proj");
    m_texhandleLoc   = m_program->uniformLocation("texhandle");
    m_scaleRecLoc    = m_program->uniformLocation("u_scaleRec");


    //get parameter locations (effect specific uniform locations)
    if(m_parameters != NULL){
        ShaderParameter* param;
        for(int i=0;i<m_parameters->count();i++)
        {
            param = &(*m_parameters)[i];
            param->location = m_program->uniformLocation(param->name);
        }

        //delete param;
        //param = NULL;
    }




    m_program->release();

    m_swapshader = false;
}


ImageCanvas::~ImageCanvas()
{
    if(m_vbo_rec != NULL){
        m_vbo_rec->destroy();
        m_vbo_rec = NULL;
    }
    if(m_texture != NULL){
        delete m_texture;
        m_texture = NULL;
    }
    if(m_program != NULL){
        delete m_program;
        m_program = NULL;
    }
}
