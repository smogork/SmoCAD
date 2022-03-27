//
// Created by ksm on 3/13/22.
//

#ifndef SMOCAD_SHADERWRAPPER_H
#define SMOCAD_SHADERWRAPPER_H


#include <QOpenGLShader>

class ShaderWrapper
{
private:
    std::unique_ptr<QOpenGLShaderProgram> shader;
    std::map<std::string, int> uniformLocations;
    QString vsFilepath;
    QString fsFilepath;
    QString tessFilepath;
    QString evalFilepath;

    void Create();

public:
    ShaderWrapper(const QString& vsFilepath, const QString& fsFilename);
    ShaderWrapper(const QString& vsFilepath, const QString& fsFilename,
                  const QString& tessFilepath, const QString& evalFilename);
    ~ShaderWrapper();

    void Bind();
    void Release();

    QOpenGLShaderProgram* GetRawProgram() { return shader.get(); }

    template <typename T>
    void SetUniform(const std::string& uniformName, const T& value)
    {
        int id = -1;

        Bind();

        auto id_it = uniformLocations.find(uniformName);
        if (id_it == uniformLocations.end())
        {
            id = shader->uniformLocation(uniformName.c_str());
            if (id < 0)
            {
                qDebug() << "Error on searching for uniform " << QString(uniformName.c_str()) << ". " << shader->log();
                Release();
                return;
            }
            uniformLocations.insert(std::make_pair(uniformName, id));
        }
        else
            id = id_it->second;

        shader->setUniformValue(id, value);
        Release();
    }
};


#endif //SMOCAD_SHADERWRAPPER_H
