#include <QFileDialog>
#include <QMessageBox>
#include "simulator3ccontrol.h"
#include "ui_simulator3ccontrol.h"

Simulator3CControl::Simulator3CControl(std::shared_ptr<Simulator3CComponent> sim, QWidget *parent) :
    ComponentControl(parent),
    ui(new Ui::Simulator3CControl), m_sim(sim)
{
    ui->setupUi(this);
    onSimulatorStateChange(m_sim.lock()->GetState());
    
    if (auto sim = m_sim.lock())
    {
        auto bParams = sim->GetBlockParameters();
        ui->blockSizeX->setValue(bParams.WidthX.GetCentimeters());
        ui->blockSizeY->setValue(bParams.WidthY.GetCentimeters());
        ui->blockHeight->setValue(bParams.Height.GetCentimeters());
        ui->blockVerticesX->setValue(bParams.VertexWidthX);
        ui->BlockVerticesY->setValue(bParams.VertexWidthY);
        ui->heightmapSize->setValue(bParams.TextureWidthX);
        
        auto cParams = sim->GetCutterParameters();
        ui->toolDiameter->setValue(cParams.Diameter.GetMilimeters());
        ui->toolCylinder->setChecked(cParams.Type == Cylindrical);
        ui->toolSphere->setChecked(cParams.Type == Spherical);
    }
    ignoreValueChanged = false;
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
    if (!checked)
        return;
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeCutterType(Spherical));
    }
}


void Simulator3CControl::on_toolCylinder_toggled(bool checked)
{
    if (!checked)
        return;
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeCutterType(Cylindrical));
    }
}


void Simulator3CControl::on_toolDiameter_valueChanged(double arg1)
{
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeCutterDiameter(Length::FromMilimeters(arg1)));
    }
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
    QFileDialog dialog(this, "Open paths file");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    
    if (dialog.exec())
    {
        QString filename = dialog.selectedFiles().first();
        
        try
        {
            if (auto sim = m_sim.lock())
            {
                sim->LoadPathFile(filename);
                ui->pathsInfo->setText("Paths loaded succesfully");
            }
        }
        catch (std::runtime_error &e) {
            QMessageBox msgBox;
            msgBox.setText(e.what());
            msgBox.exec();
            ui->pathsInfo->setText("Error while loading");
        }
        
        RequestRepaint();
        /*if (auto sim = m_sim.lock())
            RequestControlsUpdate(sim->GetAttachedObjectID());*/
    }
}


void Simulator3CControl::on_pushButton_2_clicked()
{

}

void Simulator3CControl::onSimulatorStateChange(SimulatorState state)
{

}
