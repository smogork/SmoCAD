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
        Length tool, global;
        std::tie(tool, global) = sim->GetToolSubmersions();
        ui->toolSubmersion->setValue(tool.GetMilimeters());
        ui->globalSubmersion->setValue(global.GetMilimeters());
        
        ui->pathsShow->setChecked(sim->GetPathsHide());
        ui->simProgress->setValue(sim->SimulationProgress);
        
        ui->pushButton_3->setVisible(false);
        ui->pushButton_4->setVisible(false);
    
        onSimulatorStateChange(sim->m_state);
        
        progressNotifier = sim->SimulationProgress.addNotifier(
                [this]()
                {
                    if (this->ignoreNotifier) return;
                    
                    auto simulation = m_sim.lock();
                    if (!simulation)
                        return;
    
                    UPDATE_UI_IGNORING_VALUE_CHANGED(
                            ui->simProgress->setValue(simulation->SimulationProgress)
                            );
                });
    
        stateNotifier = sim->m_state.addNotifier(
                [this]()
                {
                    if (this->ignoreNotifier) return;
                
                    auto simulation = m_sim.lock();
                    if (!simulation)
                        return;
    
                    onSimulatorStateChange(simulation->m_state);
                });

    }
    ignoreValueChanged = false;
}

Simulator3CControl::~Simulator3CControl()
{
    delete ui;
}

void Simulator3CControl::on_blockSizeX_valueChanged(double arg1)
{
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeBlockSize(Length::FromCentimeters(ui->blockSizeX->value()),
                                                            Length::FromCentimeters(ui->blockSizeY->value()),
                                                            Length::FromCentimeters(ui->blockHeight->value())));
    }
}

void Simulator3CControl::on_blockSizeY_valueChanged(double arg1)
{
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeBlockSize(Length::FromCentimeters(ui->blockSizeX->value()),
                                                            Length::FromCentimeters(ui->blockSizeY->value()),
                                                            Length::FromCentimeters(ui->blockHeight->value())));
    }
}

void Simulator3CControl::on_blockHeight_valueChanged(double arg1)
{
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeBlockSize(Length::FromCentimeters(ui->blockSizeX->value()),
                                                            Length::FromCentimeters(ui->blockSizeY->value()),
                                                            Length::FromCentimeters(ui->blockHeight->value())));
    }
}

void Simulator3CControl::on_blockVerticesX_valueChanged(int arg1)
{
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeBlockVertices(ui->blockVerticesX->value(),
                                                                ui->BlockVerticesY->value()));
    }
}

void Simulator3CControl::on_BlockVerticesY_valueChanged(int arg1)
{
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeBlockVertices(ui->blockVerticesX->value(),
                                                                ui->BlockVerticesY->value()));
    }
}

void Simulator3CControl::on_heightmapSize_valueChanged(int arg1)
{
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeTextureSize(arg1));
    }
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
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeToolSubmersions(Length::FromMilimeters(ui->toolSubmersion->value()),
                                                                  Length::FromMilimeters(
                                                                          ui->globalSubmersion->value())));
    }
}

void Simulator3CControl::on_globalSubmersion_valueChanged(double arg1)
{
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->ChangeToolSubmersions(Length::FromMilimeters(ui->toolSubmersion->value()),
                                                                  Length::FromMilimeters(
                                                                          ui->globalSubmersion->value())));
    }
}

void Simulator3CControl::on_pathsShow_toggled(bool checked)
{
    if (auto sim = m_sim.lock())
    {
        UPDATE_VALUE_IGNORING_NOTIFIER(sim->HidePathsOnScene(checked));
    }
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
            }
        }
        catch (std::runtime_error &e)
        {
            QMessageBox msgBox;
            msgBox.setText(e.what());
            msgBox.exec();
        }
        
        RequestRepaint();
        /*if (auto sim = m_sim.lock())
            RequestControlsUpdate(sim->GetAttachedObjectID());*/
    }
}

void Simulator3CControl::on_pushButton_2_clicked()
{
    if (auto sim = m_sim.lock())
    {
        sim->SkipPathToEnd();
    }
}

void Simulator3CControl::onSimulatorStateChange(SimulatorState state)
{
    switch (state)
    {
        case IDLE:
            //nie mozna uzywac przyciskow od symulacji
            //blok
            ui->blockSizeX->setEnabled(true);
            ui->blockSizeY->setEnabled(true);
            ui->blockHeight->setEnabled(true);
            ui->blockVerticesX->setEnabled(true);
            ui->BlockVerticesY->setEnabled(true);
            ui->heightmapSize->setEnabled(true);
            
            //narzedzie
            ui->toolCylinder->setEnabled(true);
            ui->toolSphere->setEnabled(true);
            ui->toolDiameter->setEnabled(true);
            ui->toolSubmersion->setEnabled(true);
            ui->globalSubmersion->setEnabled(true);
            
            //sterowanie symulacja
            ui->simPlayPause->setEnabled(false);
            ui->pushButton_2->setEnabled(false);//skip
            
            //sciezki
            ui->pushButton->setEnabled(true);//wczytywanie sciezek
            ui->pushButton_3->setEnabled(false);//wyczysczenie sciezek
            ui->pushButton_4->setEnabled(true);//restart symulatora
            
            ui->simGroupBox->setTitle("Simulator3C[IDLE]");
            break;
        case PAUSED:
            //nie mozna zmieniac bloku!!
            //blok
            ui->blockSizeX->setEnabled(false);
            ui->blockSizeY->setEnabled(false);
            ui->blockHeight->setEnabled(false);
            ui->blockVerticesX->setEnabled(false);
            ui->BlockVerticesY->setEnabled(false);
            ui->heightmapSize->setEnabled(false);
        
            //narzedzie
            ui->toolCylinder->setEnabled(true);
            ui->toolSphere->setEnabled(true);
            ui->toolDiameter->setEnabled(true);
            ui->toolSubmersion->setEnabled(true);
            ui->globalSubmersion->setEnabled(true);
        
            //sterowanie symulacja
            ui->simPlayPause->setEnabled(true);
            ui->pushButton_2->setEnabled(true);//skip
        
            //sciezki
            ui->pushButton->setEnabled(true);//wczytywanie sciezek
            ui->pushButton_3->setEnabled(true);//wyczysczenie sciezek
            ui->pushButton_4->setEnabled(true);//restart symulatora
            
            ui->simGroupBox->setTitle("Simulator3C[PAUSED]");
            break;
        case MILLING:
            //nie mozna zmieniac bloku i freza oraz usuwac sciezek ani restartcowac symulator
            //blok
            ui->blockSizeX->setEnabled(false);
            ui->blockSizeY->setEnabled(false);
            ui->blockHeight->setEnabled(false);
            ui->blockVerticesX->setEnabled(false);
            ui->BlockVerticesY->setEnabled(false);
            ui->heightmapSize->setEnabled(false);
        
            //narzedzie
            ui->toolCylinder->setEnabled(false);
            ui->toolSphere->setEnabled(false);
            ui->toolDiameter->setEnabled(false);
            ui->toolSubmersion->setEnabled(false);
            ui->globalSubmersion->setEnabled(false);
        
            //sterowanie symulacja
            ui->simPlayPause->setEnabled(true);
            ui->pushButton_2->setEnabled(true);//skip
        
            //sciezki
            ui->pushButton->setEnabled(false);//wczytywanie sciezek
            ui->pushButton_3->setEnabled(false);//wyczysczenie sciezek
            ui->pushButton_4->setEnabled(false);//restart symulatora
            
            ui->simGroupBox->setTitle("Simulator3C[MILLING]");
            break;
    }
}

void Simulator3CControl::on_simPlayPause_clicked()
{
    if (auto sim = m_sim.lock())
    {
        sim->PlayPauseSimulation();
    }
}

void Simulator3CControl::on_pushButton_3_clicked()
{
    if (auto sim = m_sim.lock())
    {
        sim->ClearPaths();
    }
}

void Simulator3CControl::on_pushButton_4_clicked()
{
    if (auto sim = m_sim.lock())
    {
        sim->Reset();
    }
}

