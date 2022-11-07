#include <QFileDialog>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "Scene/Systems/SelectableSystem.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/Entities/Torus.h"
#include "Scene/Components/TransformCollection.h"
#include "Scene/Systems/TransformCollectionSystem.h"
#include "Scene/Entities/Curves/BezierC2.h"
#include "Scene/Entities/Cube.h"
#include "Scene/Entities/Curves/BezierC0.h"

#include "Renderer/Options.h"
#include "Scene/Entities/Curves/InterpolationC2.h"
#include "Scene/Entities/Planes/PlaneCreator.h"

#include "Serializer.h"
#include "Scene/Entities/SelectRectangle.h"
#include "Controls/EntityContextMenu.h"
#include "Scene/Entities/Simulator/Simulator3C.h"

#include <QOpenGLFramebufferObject>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->anaglyphWidget->setVisible(Options::RenderStereoscopy);
    
    QObject::connect(&Renderer::controller, &InputController::SceneMouseClicked,
                     this, &MainWindow::MouseRaycastSlot);
    QObject::connect(&Renderer::controller, &InputController::UpdateSelectRectangle,
                     this, &MainWindow::UpdateSelectRectangle);
    QObject::connect(&Renderer::controller, &InputController::RequestControlsUpdate,
                     this, &MainWindow::UpdateComponentUI);
    
    //register signals to cursorControl
    QObject::connect(ui->sceneWidget, &GLWidget::WidgetResized,
                     ui->cursorPosWidget, &CursorControl::ViewportResized);
    QObject::connect(ui->cursorPosWidget, &CursorControl::RequestRepaint,
                     ui->sceneWidget, &GLWidget::RedrawScreen);
    
    //register signals to SceneListElements
    QObject::connect(ui->sceneElementsWIdget, &SceneElementsList::RequestControlsUpdate,
                     this, &MainWindow::UpdateComponentUI);
    QObject::connect(ui->sceneElementsWIdget, &SceneElementsList::RequestRepaint,
                     ui->sceneWidget, &GLWidget::RedrawScreen);
    
    //Register signals to AnaglyphsConfig control
    QObject::connect(ui->anaglyphWidget, &StereoscopicConfig::RequestRepaint,
                     ui->sceneWidget, &GLWidget::RedrawScreen);
    
    //Rejestracja wspomagacza do tworzenia menu kontekstowych - straszne workaround braku mozliwosci
    //podlaczenia systemow bezposrednio do slotow
    QObject::connect(EntityContextMenu::GetInstance().get(), &EntityContextMenu::RequestRepaint,
                     ui->sceneWidget, &GLWidget::RedrawScreen);
    QObject::connect(EntityContextMenu::GetInstance().get(), &EntityContextMenu::RequestControlsUpdate,
                     this, &MainWindow::UpdateComponentUI);
    
    componentSpacer.reset();
    componentSpacer = std::make_unique<QSpacerItem>(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout->insertSpacerItem(0, componentSpacer.get());
}

MainWindow::~MainWindow()
{
    ui->verticalLayout->takeAt(0);
    componentSpacer.reset();
    componentControls.clear();
    delete ui;
}

void MainWindow::UpdateComponentUI(unsigned int oid)
{
    
    if (auto scene = SceneECS::Instance().lock())
    {
        componentControls = scene->CreateUIForObject(oid);
        for (const std::unique_ptr<ComponentControl> &widget: componentControls)
        {
            widget->setParent(ui->scrollAreaWidgetContents);
            ui->verticalLayout->addWidget(widget.get());
            
            QObject::connect(widget.get(), &ComponentControl::RequestRepaint,
                             ui->sceneWidget, &GLWidget::RedrawScreen);
            QObject::connect(widget.get(), &ComponentControl::RequestControlsUpdate,
                             this, &MainWindow::UpdateComponentUI);
        }
    }
}

void MainWindow::MouseRaycastSlot(std::shared_ptr<SceneMouseClickEvent> event)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        unsigned int oid = scene->MouseClicked(event);
        UpdateComponentUI(oid);
        ui->sceneWidget->update();
    }
}

void MainWindow::UpdateSelectRectangle(std::shared_ptr<SelectRectangleUpdateEvent> event)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        unsigned int oid = scene->UpdateSelectRectangle(event);
        UpdateComponentUI(oid);
        ui->sceneWidget->update();
    }
}

#pragma region Creating new objects

void MainWindow::on_actionTorus_triggered()
{
    std::shared_ptr<Torus> t = std::make_shared<Torus>("NewTorus");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(t);
    ui->sceneWidget->update();
}

void MainWindow::on_actionPoint_triggered()
{
    std::shared_ptr<Point> p = std::make_shared<Point>("NewPoint", QVector3D(0, 0, 0));
    if (auto scene = SceneECS::Instance().lock())
    {
        scene->AddObject(p);
        
        //Jezeli aktualnie wybrany obiekt jest kolekcja punktow - dodaj do niej
        if (auto select = scene->GetSystem<SelectableSystem>().lock())
        {
            if (auto obj = select->GetSelectedObject())
            {
                auto oid = obj->GetAttachedObjectID();
                if (auto colection = scene->GetComponentOfSystem<TransformCollectionSystem, TransformCollection>(oid)
                        .lock())
                {
                    colection->AddPoint(p->p_CollectionAware);
                }
            }
        }
    }
    ui->sceneWidget->update();
}

void MainWindow::on_actionCube_triggered()
{
    std::shared_ptr<Cube> c = std::make_shared<Cube>("NewCube");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(c);
    ui->sceneWidget->update();
}

void MainWindow::on_actionBezierC0_triggered()
{
    
    std::shared_ptr<BezierC0> b0 = std::make_shared<BezierC0>("NewBezierC0");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(b0);
    ui->sceneWidget->update();
}

void MainWindow::on_actionBezierC2_triggered()
{
    std::shared_ptr<BezierC2> b2 = std::make_shared<BezierC2>("NewBezierC2");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(b2);
    ui->sceneWidget->update();
}

void MainWindow::on_actionInterpolationC2_triggered()
{
    std::shared_ptr<InterpolationC2> i2 = std::make_shared<InterpolationC2>("NewInterpolationC2");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(i2);
    ui->sceneWidget->update();
}

void MainWindow::on_actionPlaneC0_triggered()
{
    std::shared_ptr<PlaneCreator> pcr = std::make_shared<PlaneCreator>("PlaneC0Creator", PLANEC0_CLASS);
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(pcr);
    ui->sceneWidget->update();
}

void MainWindow::on_actionPlaneC2_triggered()
{
    std::shared_ptr<PlaneCreator> pcr = std::make_shared<PlaneCreator>("PlaneC2Creator", PLANEC2_CLASS);
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(pcr);
    ui->sceneWidget->update();
}

void MainWindow::on_actionNew_triggered()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        scene->CleanScene();
        componentControls.clear();
        ui->sceneWidget->update();
    }
}

void MainWindow::on_action3C_Simulator_triggered()
{
    std::shared_ptr<Simulator3C> sim = std::make_shared<Simulator3C>("Simulator3C");
    if (auto scene = SceneECS::Instance().lock())
        scene->AddObject(sim);
    ui->sceneWidget->update();
}

void MainWindow::on_actionOpen_triggered()
{
    QFileDialog dialog(this, "Open saved scene file");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter("Json Files (*.json)");
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    
    if (dialog.exec())
    {
        QString fileName = dialog.selectedFiles().first();
        
        on_actionNew_triggered();
        if (auto scene = SceneECS::Instance().lock())
            scene->LoadSceneFromFile(fileName);
        ui->sceneWidget->update();
    }
}

void MainWindow::on_actionSave_triggered()
{
    QFileDialog dialog(this, "Save scene to file");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter("Json Files (*.json)");
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    
    if (dialog.exec())
    {
        QString fileName = dialog.selectedFiles().first();
        if (auto scene = SceneECS::Instance().lock())
            scene->SaveSceneToFile(fileName);
    }
}

void MainWindow::on_actionExit_triggered()
{
    exit(0);
}

#pragma endregion

void MainWindow::on_actionShow_Bezier_polygon_toggled(bool arg1)
{
    Options::DrawBezierPolygon = arg1;
    ui->sceneWidget->update();
}

void MainWindow::on_actionShow_BSpline_polygon_toggled(bool arg1)
{
    Options::DrawDeBoorPolygon = arg1;
    ui->sceneWidget->update();
}

void MainWindow::on_actionAnaglyphic_3D_view_toggled(bool arg1)
{
    Options::RenderStereoscopy = arg1;
    ui->anaglyphWidget->setVisible(Options::RenderStereoscopy);
    ui->sceneWidget->update();
}

void MainWindow::on_actionShow_Bezier_mesh_triggered(bool checked)
{
    Options::DrawPlainMesh = checked;
    ui->sceneWidget->update();
}

void MainWindow::on_actionGenerate_routes_triggered()
{
    //https://amin-ahmadi.com/2019/07/12/using-opengl-in-qt-for-processing-images/
    QOpenGLContext context;
    if(!context.create())
    {
        qDebug() << "Can't create GL context.";
        return;
    }

    QOffscreenSurface surface;
    surface.setFormat(context.format());
    surface.create();
    if(!surface.isValid())
    {
        qDebug() << "Surface not valid.";
        return;
    }

    if(!context.makeCurrent(&surface))
    {
        qDebug() << "Can't make context current.";
        return;
    }

    QImage image({1024, 1024}, QImage::Format_ARGB32);
    image.fill(Qt::red);

    QOpenGLFramebufferObject fbo(image.size());
    context.functions()->glViewport(0, 0, image.width(), image.height());

    auto sh = Renderer::GetShader(DEFAULT_SHADER).lock();

    /*QOpenGLTexture texture(QOpenGLTexture::Target2D);
    texture.setData(image);

    texture.bind();
    if(!texture.isBound())
    {
        qDebug() << "Texture not bound.";
        return;
    }*/

    Cube c("offscreenCube");
    c.p_Drawing->Render(&context);

    auto output = fbo.toImage();
    output.save("offscreenOutput.png");
}




