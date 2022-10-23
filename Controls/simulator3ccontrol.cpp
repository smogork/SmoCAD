#include "simulator3ccontrol.h"
#include "ui_simulator3ccontrol.h"

Simulator3CControl::Simulator3CControl(std::shared_ptr<Simulator3CComponent> sim, QWidget *parent) :
    ComponentControl(parent),
    ui(new Ui::Simulator3CControl), m_sim(sim)
{
    ui->setupUi(this);
    onSimulatorStateChange(m_sim.lock()->GetState());
}

Simulator3CControl::~Simulator3CControl()
{
    delete ui;
}

void Simulator3CControl::on_blockSizeX_valueChanged(double arg1)
{

}


void Simulator3CControl::on_blockSizeY_valueChanged(double arg1)
{

}


void Simulator3CControl::on_blockHeight_valueChanged(double arg1)
{

}


void Simulator3CControl::on_blockVerticesX_valueChanged(int arg1)
{

}


void Simulator3CControl::on_BlockVerticesY_valueChanged(int arg1)
{

}


void Simulator3CControl::on_heightmapSize_valueChanged(int arg1)
{

}


void Simulator3CControl::on_toolSphere_toggled(bool checked)
{

}


void Simulator3CControl::on_toolCylinder_toggled(bool checked)
{

}


void Simulator3CControl::on_toolDiameter_valueChanged(double arg1)
{

}


void Simulator3CControl::on_toolSubmersion_valueChanged(double arg1)
{

}


void Simulator3CControl::on_globalSubmersion_valueChanged(double arg1)
{

}


void Simulator3CControl::on_simSpeed_valueChanged(int value)
{

}


void Simulator3CControl::on_pathsShow_toggled(bool checked)
{

}


void Simulator3CControl::on_pushButton_clicked()
{

}


void Simulator3CControl::on_pushButton_2_clicked()
{

}

void Simulator3CControl::onSimulatorStateChange(SimulatorState state)
{

}

