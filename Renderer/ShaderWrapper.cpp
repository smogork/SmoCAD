//
// Created by ksm on 3/13/22.
//

#include "ShaderWrapper.h"

ShaderWrapper::ShaderWrapper(const QString &vsFilepath, const QString &fsFilepath) :
        fsFilepath(fsFilepath), vsFilepath(vsFilepath), shader(nullptr) {
    Create();
}

ShaderWrapper::ShaderWrapper(const QString &vsFilepath, const QString &fsFilename, const QString &tessFilepath,
                             const QString &evalFilename) :
        fsFilepath(fsFilename), vsFilepath(vsFilepath), shader(nullptr), tessFilepath(tessFilepath),
        evalFilepath(evalFilename) {
    Create();
}

ShaderWrapper::~ShaderWrapper() {}

void ShaderWrapper::Bind() {
    if (!shader->bind())
        qDebug() << "Error on binding m_shader " << shader->log();
}

void ShaderWrapper::Release() {
    shader->release();
}

void ShaderWrapper::Create() {
    shader = std::make_unique<QOpenGLShaderProgram>();

    // read the m_shader programs from the resource
    if (!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, vsFilepath))
        qDebug() << "Vertex m_shader errors:\n" << shader->log();

    if (!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, fsFilepath))
        qDebug() << "Fragment m_shader errors:\n" << shader->log();

    if (tessFilepath != "") {
        if (!shader->addShaderFromSourceFile(QOpenGLShader::TessellationControl, tessFilepath))
            qDebug() << "tesselayion control m_shader errors:\n" << shader->log();
    }

    if (evalFilepath != "") {
        if (!shader->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, evalFilepath))
            qDebug() << "Tesselation evaluation m_shader errors:\n" << shader->log();
    }

    if (!shader->link())
        qDebug() << "Shader linker errors:\n" << shader->log();
}


