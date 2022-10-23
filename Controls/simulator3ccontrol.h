#ifndef SIMULATOR3CCONTROL_H
#define SIMULATOR3CCONTROL_H

#include <QWidget>
#include "ComponentControl.h"
#include "Scene/Components/Simulator3CComponent.h"

namespace Ui
{
    class Simulator3CControl;
}

class Simulator3CControl : public ComponentControl
{
Q_OBJECT

public:
    explicit Simulator3CControl(std::shared_ptr<Simulator3CComponent> sim, QWidget *parent = nullptr);
    ~Simulator3CControl();

private slots:
    void on_blockSizeX_valueChanged(double arg1);
    void on_blockSizeY_valueChanged(double arg1);
    void on_blockHeight_valueChanged(double arg1);
    void on_blockVerticesX_valueChanged(int arg1);
    void on_BlockVerticesY_valueChanged(int arg1);
    void on_heightmapSize_valueChanged(int arg1);
    
    void on_toolSphere_toggled(bool checked);
    void on_toolCylinder_toggled(bool checked);
    void on_toolDiameter_valueChanged(double arg1);
    void on_toolSubmersion_valueChanged(double arg1);
    void on_globalSubmersion_valueChanged(double arg1);
    
    void on_simSpeed_valueChanged(int value);
    void on_pathsShow_toggled(bool checked);
    
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    
    void onSimulatorStateChange(SimulatorState state);

private:
    Ui::Simulator3CControl *ui;
    std::weak_ptr<Simulator3CComponent> m_sim;
};

#endif // SIMULATOR3CCONTROL_H
