#pragma once

#include <QtWidgets>

#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>


#include <array>

#include <vtkActor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkRectilinearGrid.h>
#include <vtkMath.h>
#include <vtkDoubleArray.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredPointsReader.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkImageImport.h>
#include <stdlib.h>
#include <vtkStructuredPointsReader.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkWindowToImageFilter.h>
// molecule
#include <vtkScalarBarActor.h>
#include <vtkMolecule.h>
#include <vtkMoleculeMapper.h>
#include <vtkSimpleBondPerceiver.h>
#include <vtkPeriodicTable.h>
#include <vtkLookupTable.h>
#include <vtkProperty2D.h>
#include <vtkTextActor.h>

// VTK includes
#include "vtkBoxWidget.h"
#include "vtkCommand.h"
#include "vtkColorTransferFunction.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkImageResample.h"
#include "vtkMetaImageReader.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPlanes.h"
#include "vtkXMLImageDataReader.h"
#include "vtkFixedPointVolumeRayCastMapper.h"

#include "VisualizationRenderParameterWidget.h"
#include "VisualizationGridWidget.h"
#include "../header/pointcharge.h"
#include "../gui/VisualizationLogWidget.h"

class VisualizationRenderWidget : public QWidget
{
public:
    explicit VisualizationRenderWidget(
        VisualizationRenderParameterWidget* RenderParameterWidget,
        VisualizationGridWidget* GridWidget,
        std::shared_ptr<std::vector<QC::Pointcharge>> pointcharges,
        VisualizationLogWidget* LogWidget,
        QWidget *parent = 0);
    ~VisualizationRenderWidget();

public:
    QVTKOpenGLNativeWidget* screen;
    vtkSmartPointer<vtkRenderer> ren;
    vtkSmartPointer<vtkImageData> imageData; //Imagedata
    std::string Label = "";
    void UpdateScreen();
    void UpdateScreenWithExternalImageData(double* ImageData);
    void SavePictureToFile(std::string File, double Scaling);

private:
    VisualizationRenderParameterWidget* RenderParameterWidget;
    VisualizationGridWidget* GridWidget;
    std::shared_ptr<std::vector<QC::Pointcharge>> pointcharges;
    VisualizationLogWidget* LogWidget;

    vtkSmartPointer<vtkActor> IsoSurfaceActor1;
    vtkSmartPointer<vtkActor> IsoSurfaceActor2;
    vtkSmartPointer<vtkActor> IsoSurfaceActor3;
    vtkSmartPointer<vtkActor> IsoSurfaceActor4;
    vtkSmartPointer<vtkActor> IsoSurfaceActor5;
    vtkSmartPointer<vtkActor> IsoSurfaceActor6;
    vtkSmartPointer<vtkActor> addIsoSurface(double threshold, double r, double g, double b, double opacity);

    vtkSmartPointer<vtkAxesActor> Coordsystem;
    void constructCoordsystem();

    vtkSmartPointer<vtkActor> MoleculeActor;
    vtkSmartPointer<vtkScalarBarActor> scalarBar;
    vtkSmartPointer<vtkTextActor> LabelActor;
    void createMoleculeViewer();
};
