//
// Created by ksm on 3/13/22.
//

#include "ShaderWrapper.h"

ShaderWrapper::ShaderWrapper(const QString &vsFilepath, const QString &fsFilepath) :
        fsFilepath(fsFilepath), vsFilepath(vsFilepath), shader(nullptr)
{
    Create();
}

ShaderWrapper::~ShaderWrapper() {}

void ShaderWrapper::Bind()
{
    if (!shader->bind())
        qDebug() << "Error on binding shader " << shader->log();
}

void ShaderWrapper::Release()
{
    shader->release();
}

void ShaderWrapper::Create()
{
    shader = std::make_unique<QOpenGLShaderProgram>();

    // read the shader programs from the resource
    if (!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, vsFilepath))
        qDebug() << "Vertex shader errors:\n" << shader->log();

    if (!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, fsFilepath))
        qDebug() << "Fragment shader errors:\n" << shader->log();

    if (tessFilepath != "")
    {
        if (!shader->addShaderFromSourceFile(QOpenGLShader::TessellationControl, tessFilepath))
            qDebug() << "tesselayion control shader errors:\n" << shader->log();
    }

    if (evalFilepath != "")
    {
        if (!shader->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, evalFilepath))
            qDebug() << "Tesselation evaluation shader errors:\n" << shader->log();
    }

    if (!shader->link())
        qDebug() << "Shader linker errors:\n" << shader->log();
}

ShaderWrapper::ShaderWrapper(const QString &vsFilepath, const QString &fsFilename, const QString &tessFilepath,
                             const QString &evalFilename):
        fsFilepath(fsFilename), vsFilepath(vsFilepath), shader(nullptr), tessFilepath(tessFilepath), evalFilepath(evalFilename)
{
    Create();
}
