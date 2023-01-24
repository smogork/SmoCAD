//
// Created by ksm on 3/13/22.
//

#include <QOpenGLFunctions_4_4_Core>
#include "ShaderWrapper.h"

ShaderWrapper::ShaderWrapper(const QString &vsFilepath, const QString &fsFilepath) :
        fsFilepath(fsFilepath), vsFilepath(vsFilepath), shader(nullptr)
{
    Create();
}

ShaderWrapper::ShaderWrapper(const QString &vsFilepath, const QString &fsFilename, const QString &tessFilepath,
                             const QString &evalFilename) :
        fsFilepath(fsFilename), vsFilepath(vsFilepath), shader(nullptr), tessFilepath(tessFilepath),
        evalFilepath(evalFilename)
{
    Create();
}

ShaderWrapper::ShaderWrapper(const QString &csFilepath): csFilepath(csFilepath)
{
    Create();
}

ShaderWrapper::~ShaderWrapper()
{}

void ShaderWrapper::Bind()
{
    if (!shader->bind())
        qDebug() << "Error on binding m_shader " << shader->log();
}

void ShaderWrapper::Release()
{
    shader->release();
}

void ShaderWrapper::Create()
{
    shader = std::make_unique<QOpenGLShaderProgram>();

    // read the m_shader programs from the resource
    if (vsFilepath != "")
        if (!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, vsFilepath))
            qDebug().noquote() << "Vertex m_shader errors:\n" << shader->log();

    if (fsFilepath != "")
        if (!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, fsFilepath))
            qDebug().noquote() << "Fragment m_shader errors:\n" << shader->log();

    if (tessFilepath != "")
    {
        if (!shader->addShaderFromSourceFile(QOpenGLShader::TessellationControl, tessFilepath))
            qDebug().noquote() << "Tesselation control m_shader errors:\n" << shader->log();
    }

    if (evalFilepath != "")
    {
        if (!shader->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, evalFilepath))
            qDebug().noquote() << "Tesselation evaluation m_shader errors:\n" << shader->log();
    }

    if (csFilepath != "")
    {
        if (!shader->addShaderFromSourceFile(QOpenGLShader::Compute, csFilepath))
            qDebug().noquote() << "Compute m_shader errors:\n" << shader->log();
    }

    if (!shader->link())
        qDebug().noquote() << "Shader linker errors:\n" << shader->log();
}



