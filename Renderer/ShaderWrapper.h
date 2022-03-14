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

public:
    ShaderWrapper(const QString& vsFilepath, const QString& fsFilename);
    ~ShaderWrapper();

    void Bind();
    void Release();
    void Create();
    void Destroy();

    template <typename T>
    void SetUniform(const std::string& uniformName, const T& value)
    {
        int id = -1;
        auto id_it = uniformLocations.find(uniformName);
        if (id_it == uniformLocations.end())
        {
            id = shader->uniformLocation(uniformName.c_str());
            if (id < 0)
            {
                qDebug() << "Error on searching for uniform " << QString(uniformName.c_str()) << ". " << shader->log();
                return;
            }
            uniformLocations.insert(std::make_pair(uniformName, id));
        }

        Bind();
        shader->setUniformValue(id, value);
        Release();
    }
};


#endif //SMOCAD_SHADERWRAPPER_H
