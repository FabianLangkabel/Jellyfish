#include "VisualizationRenderWidget.h"

VisualizationRenderWidget::VisualizationRenderWidget(
        VisualizationRenderParameterWidget* RenderParameterWidget,
        VisualizationGridWidget* GridWidget,
        std::shared_ptr<std::vector<QC::Pointcharge>> pointcharges,
		VisualizationLogWidget* LogWidget,
        QWidget *parent) : QWidget(parent)
{
	this->RenderParameterWidget = RenderParameterWidget;
	this->GridWidget = GridWidget;
	this->pointcharges = pointcharges;
	this->LogWidget = LogWidget;

	// needed to ensure appropriate OpenGL context is created for VTK rendering.
	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
	screen = new QVTKOpenGLNativeWidget;
	
	// Central Widget
	// Render Window
	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
	std::array<unsigned char, 4> bkg{ {255, 255, 255, 255} };
	colors->SetColor("BkgColor", bkg.data());

	// Create the renderer, the render window, and the interactor. The renderer
	// draws into the render window, the interactor enables mouse- and
	// keyboard-based interaction with the scene.
	ren = vtkSmartPointer<vtkRenderer>::New();
	//vtkNew<vtkRenderer> ren;

	// Create an image data
	imageData = vtkSmartPointer<vtkImageData>::New();

    //set camera
	ren->GetActiveCamera()->SetViewUp(0.0, 0.0, 1.0);
	ren->GetActiveCamera()->SetPosition(1, 0, 0);
	ren->GetActiveCamera()->SetFocalPoint(0, 0, 0);

	//ren->ResetCamera();
	ren->GetActiveCamera()->Azimuth(30.0);
	ren->GetActiveCamera()->Elevation(30.0);

	// Set a background color for the renderer
	ren->SetBackground(colors->GetColor3d("BkgColor").GetData());



	// execute render
	screen->renderWindow()->AddRenderer(ren);
	screen->renderWindow()->SetWindowName("RenderWindow");
	screen->setMinimumWidth(800);
	screen->setMinimumHeight(600);
	//anti alialising
	screen->renderWindow()->SetMultiSamples(4);
	screen->renderWindow()->SetPointSmoothing(true);
	screen->renderWindow()->SetLineSmoothing(true);
	screen->renderWindow()->SetPolygonSmoothing(true);

	//Additional Actors
	ren->AddActor(Coordsystem);
	ren->AddActor(MoleculeActor);
	ren->AddActor2D(scalarBar);

    QVBoxLayout* Layout = new QVBoxLayout;
	Layout->addWidget(screen);
	setLayout(Layout);
}

VisualizationRenderWidget::~VisualizationRenderWidget()
{

}

void VisualizationRenderWidget::UpdateScreen()
{
	//Isosurfaces
	if (IsoSurfaceActor1 != nullptr) { IsoSurfaceActor1->SetVisibility(false); ren->RemoveActor(IsoSurfaceActor1); }
	if (IsoSurfaceActor2 != nullptr) { IsoSurfaceActor2->SetVisibility(false); ren->RemoveActor(IsoSurfaceActor2); }
	if (IsoSurfaceActor3 != nullptr) { IsoSurfaceActor3->SetVisibility(false); ren->RemoveActor(IsoSurfaceActor3); }
	if (IsoSurfaceActor4 != nullptr) { IsoSurfaceActor4->SetVisibility(false); ren->RemoveActor(IsoSurfaceActor4); }
	if (IsoSurfaceActor5 != nullptr) { IsoSurfaceActor5->SetVisibility(false); ren->RemoveActor(IsoSurfaceActor5); }
	if (IsoSurfaceActor6 != nullptr) { IsoSurfaceActor6->SetVisibility(false); ren->RemoveActor(IsoSurfaceActor6); }

	if(Coordsystem != nullptr) {Coordsystem->SetVisibility(false); ren->RemoveActor(Coordsystem);}
	if(MoleculeActor != nullptr) {MoleculeActor->SetVisibility(false); ren->RemoveActor(MoleculeActor);}
	if(scalarBar != nullptr) {scalarBar->SetVisibility(false); ren->RemoveActor2D(scalarBar);}
	if(LabelActor != nullptr) {LabelActor->SetVisibility(false); ren->RemoveActor2D(LabelActor);}

	double isosurfacethreshold = RenderParameterWidget->isosurfacethreshold;
	double* color1 = RenderParameterWidget->color1;
	double* color2 = RenderParameterWidget->color2;

	IsoSurfaceActor1 = addIsoSurface(isosurfacethreshold, color1[0], color1[1], color1[2], 0.9);
	IsoSurfaceActor1->SetVisibility(true);
	ren->AddActor(IsoSurfaceActor1);

	IsoSurfaceActor2 = addIsoSurface(-isosurfacethreshold, color2[0], color2[1], color2[2], 0.9);
	ren->AddActor(IsoSurfaceActor2);
	IsoSurfaceActor2->SetVisibility(true);

	if(RenderParameterWidget->isosurfacenumber > 1)
	{
		IsoSurfaceActor3 = addIsoSurface(0.4 * isosurfacethreshold, .1*(9*color1[0] + color2[0]), .1 * (9 * color1[1] + color2[1]), .1 * (9 * color1[2] + color2[2]), 0.4);
		IsoSurfaceActor3->SetVisibility(true);
		ren->AddActor(IsoSurfaceActor3);

		IsoSurfaceActor4 = addIsoSurface(-0.4 * isosurfacethreshold, .1 * (color1[0] + 9 * color2[0]), .1 * (color1[1] + 9 * color2[1]), .1 * (color1[2] + 9 * color2[2]), 0.4);
		ren->AddActor(IsoSurfaceActor4);
		IsoSurfaceActor4->SetVisibility(true);
	}

	if(RenderParameterWidget->isosurfacenumber > 2)
	{
		IsoSurfaceActor5 = addIsoSurface(0.1*isosurfacethreshold, .1 * (8*color1[0] + 2 * color2[0]), .2 * (8 * color1[1] + 2 * color2[1]), .1 * (8 * color1[2] + 2 * color2[2]), .2);
		ren->AddActor(IsoSurfaceActor5);
		IsoSurfaceActor5->SetVisibility(true);

		IsoSurfaceActor6 = addIsoSurface(-0.1*isosurfacethreshold, .1 * (2 * color1[0] + 8 * color2[0]), .1 * (2 * color1[1] + 8 * color2[1]), .1 * (2 * color1[2] + 8 * color2[2]), .2);
		ren->AddActor(IsoSurfaceActor6);
		IsoSurfaceActor6->SetVisibility(true);
	}

	//Rest
	if (RenderParameterWidget->drawcoordsystem)
	{
		constructCoordsystem();
		ren->AddActor(Coordsystem);
	}
	if (RenderParameterWidget->drawmolecule)
	{		
		createMoleculeViewer();
		ren->AddActor(MoleculeActor);
		ren->AddActor2D(scalarBar);
	}
	if(Label != "")
	{
		LabelActor = vtkSmartPointer<vtkTextActor>::New();
		LabelActor->SetPosition(15, 0.9*screen->renderWindow()->GetSize()[1]);
		vtkSmartPointer<vtkTextProperty> AnnotationStyle = vtkSmartPointer<vtkTextProperty>::New();
		AnnotationStyle->SetColor(0.1, 0.1, 0.1);
		AnnotationStyle->SetOpacity(1);
		AnnotationStyle->SetFontFamilyToArial();
		AnnotationStyle->SetFontSize(int(screen->renderWindow()->GetSize()[1] * 0.07));

		LabelActor->SetTextProperty(AnnotationStyle);
		LabelActor->SetInput(Label.c_str());

		ren->AddActor2D(LabelActor);
	}

	screen->renderWindow()->Render();
}

vtkSmartPointer<vtkActor> VisualizationRenderWidget::addIsoSurface(double threshold, double r, double g, double b, double opacity)
{
	// Create a 3D model using marching cubes
	vtkSmartPointer<vtkMarchingCubes> mc = vtkSmartPointer<vtkMarchingCubes>::New();
	mc->SetInputDataObject(imageData);
	mc->ComputeNormalsOn();
	mc->ComputeGradientsOn();
	mc->SetValue(0, threshold);  // second value acts as threshold

	// Create a mapper
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(mc->GetOutputPort());
	mapper->ScalarVisibilityOff();    // utilize actor's property I set

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->GetProperty()->SetColor(r, g, b);
	actor->GetProperty()->SetOpacity(opacity);
	actor->SetMapper(mapper);
	actor->SetPosition(0, 0, 0);
	return actor;
}

void VisualizationRenderWidget::UpdateScreenWithExternalImageData(double* ImageData)
{
	int xdim = GridWidget->UsedGrid->xdim;
	int ydim = GridWidget->UsedGrid->ydim;
	int zdim = GridWidget->UsedGrid->zdim;

	/*
	vtkNew<vtkImageImport> imageImport;
  	//imageImport->SetDataSpacing(1, 1, 1);
  	//imageImport->SetDataOrigin(0, 0, 0);
	imageImport->SetWholeExtent(0, xdim, 0, ydim, 0, zdim);
  	//imageImport->SetDataExtentToWholeExtent();
  	imageImport->SetDataScalarTypeToDouble();
  	//imageImport->SetNumberOfScalarComponents(1);
  	imageImport->SetImportVoidPointer(PictureData);
	imageImport->Update();
	*/

	vtkSmartPointer<vtkImageImport> imageImport = vtkSmartPointer<vtkImageImport>::New();

    imageImport->SetImportVoidPointer(ImageData);
    imageImport->SetDataScalarTypeToDouble();
    imageImport->SetNumberOfScalarComponents(1);
    imageImport->SetDataSpacing(1, 1, 1);
    imageImport->SetDataOrigin(0, 0, 0);
    imageImport->SetDataExtent(0, xdim - 1, 0, ydim - 1, 0, zdim - 1);
    imageImport->SetWholeExtent(0, xdim - 1, 0, ydim - 1, 0, zdim - 1);
    imageImport->Update();

	imageData = imageImport->GetOutput();

	UpdateScreen();
}

void VisualizationRenderWidget::constructCoordsystem()
{
	int xdim = GridWidget->UsedGrid->xdim;
	int ydim = GridWidget->UsedGrid->ydim;
	int zdim = GridWidget->UsedGrid->zdim;
	double resolution = GridWidget->UsedGrid->resolution;
	Coordsystem = vtkSmartPointer<vtkAxesActor>::New();
	Coordsystem->SetPosition(0,0,0);
	Coordsystem->SetTotalLength(xdim, ydim, zdim);
	// Arrow Tip
	Coordsystem->SetTipTypeToCone();
	Coordsystem->SetConeRadius(.02*resolution);
	// Axis
	Coordsystem->SetShaftTypeToCylinder();
	Coordsystem->SetCylinderRadius(.004*resolution);
	//AxisLabel
	Coordsystem->SetXAxisLabelText("x");
	Coordsystem->SetYAxisLabelText("y");
	Coordsystem->SetZAxisLabelText("z");
	Coordsystem->SetAxisLabels(true);
	//Style of AxisLabels
	vtkSmartPointer<vtkTextProperty> AxisLabelStyle = vtkSmartPointer<vtkTextProperty>::New();
	AxisLabelStyle->SetColor(0.1, 0.1, 0.1);
	AxisLabelStyle->SetOpacity(0.9);
	AxisLabelStyle->SetFontFamilyToArial();
	AxisLabelStyle->SetFontSize(0.4 * resolution);
	//set Style
	vtkSmartPointer<vtkCaptionActor2D> XActor = Coordsystem->GetXAxisCaptionActor2D();
	vtkSmartPointer<vtkCaptionActor2D> YActor = Coordsystem->GetYAxisCaptionActor2D();
	vtkSmartPointer<vtkCaptionActor2D> ZActor = Coordsystem->GetZAxisCaptionActor2D();
	XActor->SetCaptionTextProperty(AxisLabelStyle);
	YActor->SetCaptionTextProperty(AxisLabelStyle);
	ZActor->SetCaptionTextProperty(AxisLabelStyle);

}

void VisualizationRenderWidget::createMoleculeViewer()
{
	// needed to ensure appropriate OpenGL context is created for VTK rendering.
	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
	QVTKOpenGLNativeWidget* widget = new QVTKOpenGLNativeWidget;

	// scale indirectly proportional to resolution
	double bohr_to_scale = 1 / GridWidget->UsedGrid->resolution;
	double A_to_bohr = 1.889725989;
	// Molecule
	vtkNew<vtkMolecule> AMolecule;
	vtkNew<vtkMolecule> auMolecule;
	// list of used elements (first element is ignored - therefore push_back 0)
	std::vector<int> elements;
	elements.push_back(0);

	//Load Atoms/Pointcharges
	std::vector<QC::Pointcharge> Pointcharges = *(this->pointcharges);
	for (int i = 0; i < Pointcharges.size(); i++)
	{
		QC::Pointcharge pointcharge = Pointcharges[i];
		double pos[3] = { (pointcharge.get_center_x() - GridWidget->UsedGrid->realxmin) / bohr_to_scale ,(pointcharge.get_center_y() - GridWidget->UsedGrid->realymin) / bohr_to_scale, (pointcharge.get_center_z() - GridWidget->UsedGrid->realzmin) / bohr_to_scale };
		auto charge = pointcharge.get_charge();
		auMolecule->AppendAtom(charge, pos);
		double Apos[3] = { pos[0] * bohr_to_scale / A_to_bohr, pos[1] * bohr_to_scale / A_to_bohr, pos[2] * bohr_to_scale / A_to_bohr };
		AMolecule->AppendAtom(charge, Apos);

		// check if element is listed
		if (std::find(std::begin(elements), std::end(elements), charge) == std::end(elements))
		{
			elements.push_back(charge);
		}
	}

	//Compute Bonds
	vtkNew<vtkSimpleBondPerceiver> bond_calculater;
	bond_calculater->SetInputData(AMolecule);

	//TODO: Wie  bessere Toleranz???
	//bond_calculater->SetTolerance(0.45*resolution);

	bond_calculater->Update();
	vtkMolecule* new_Molecule = bond_calculater->GetOutput();
	for (int i = 0; i < Pointcharges.size(); i++)
	{
		new_Molecule->SetAtomPosition(i, auMolecule->GetAtomPosition(i));
	}

	//CreateMapper
	vtkNew<vtkMoleculeMapper> MoleculeMapper;
	MoleculeMapper->SetInputData(new_Molecule);
	MoleculeMapper->UseBallAndStickSettings();
	if (RenderParameterWidget->draw_bonds)
	{
		MoleculeMapper->RenderBondsOn();
	}
	else
	{
		MoleculeMapper->RenderBondsOff();
	}
	MoleculeMapper->SetBondRadius(0.05 * GridWidget->UsedGrid->resolution);
	MoleculeMapper->SetAtomicRadiusScaleFactor(0.2 * GridWidget->UsedGrid->resolution);

	//Legend
	//Test1
	vtkPeriodicTable* periodictable = MoleculeMapper->GetPeriodicTable();
	vtkNew<vtkLookupTable> Lookuptable;

	Lookuptable->IndexedLookupOn();
	Lookuptable->SetNumberOfTableValues(elements.size());

	Lookuptable->Build();

	for (int i = 1; i < elements.size(); i++)
	{
		vtkColor3f color = periodictable->GetDefaultRGBTuple(elements[i]);
		Lookuptable->SetTableValue(i - 1, color.GetRed(), color.GetGreen(), color.GetBlue());
		Lookuptable->SetAnnotation(i, periodictable->GetElementName(elements[i]));
	}


	//scalar bar as periodic table legend
	scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
	scalarBar->SetLookupTable(Lookuptable);
	// scalarBar->SetTitle("Elements");
	//scalar Bar initially full window height
	scalarBar->SetWidth((scalarBar->GetHeight()) * 0.08);
	scalarBar->SetHeight((scalarBar->GetHeight()) * (0.08 * elements.size()));
	scalarBar->SetBarRatio(1);
	scalarBar->SetPosition(.02, 0.04);
	scalarBar->SetTextPositionToPrecedeScalarBar();

	vtkSmartPointer<vtkProperty2D> bkgproperty = vtkSmartPointer<vtkProperty2D>::New();
	bkgproperty->SetColor(.95, .95, .95);
	scalarBar->SetBackgroundProperty(bkgproperty);
	scalarBar->DrawBackgroundOn();

	// frame of legend
	vtkSmartPointer<vtkProperty2D> frameproperty = vtkSmartPointer<vtkProperty2D>::New();
	frameproperty->SetColor(0.1, 0.1, 0.1);
	scalarBar->SetFrameProperty(frameproperty);
	scalarBar->DrawFrameOn();

	//Define Text Properties for Label
	vtkSmartPointer<vtkTextProperty> LabelStyle = vtkSmartPointer<vtkTextProperty>::New();
	LabelStyle->SetColor(0.1, 0.1, 0.1);
	LabelStyle->SetOpacity(0.9);
	LabelStyle->SetFontFamilyToArial();
	LabelStyle->SetFontSize(20);
	vtkSmartPointer<vtkTextProperty> AnnotationStyle = vtkSmartPointer<vtkTextProperty>::New();
	AnnotationStyle->SetColor(0.1, 0.1, 0.1);
	AnnotationStyle->SetOpacity(0.9);
	AnnotationStyle->SetFontFamilyToArial();
	AnnotationStyle->SetFontSize(int(screen->renderWindow()->GetSize()[0] * 0.025));
	//Set Textproperties
	scalarBar->SetTitleTextProperty(LabelStyle);
	scalarBar->SetAnnotationTextProperty(AnnotationStyle);
	

	//Create Actor
	MoleculeActor = vtkSmartPointer<vtkActor>::New();
	MoleculeActor->SetMapper(MoleculeMapper);

}

void VisualizationRenderWidget::SavePictureToFile(std::string filenamestr, double Scaling)
{
	//Path \ Filename . filetype
	int end = filenamestr.find_last_of(".");
	std::string filetype = "";
	for (int i = end; i < filenamestr.length(); i++)
	{
		filetype.push_back(filenamestr[i]);
	}

	if (filetype == std::string(".jpg"))
	{
		//JPEG Writer
		vtkSmartPointer<vtkJPEGWriter> jpegwriter = vtkSmartPointer<vtkJPEGWriter>::New();
		jpegwriter->SetFileName(filenamestr.c_str());
		//convert screen to an exportable image
		vtkSmartPointer<vtkWindowToImageFilter> toImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
		toImageFilter->SetInput(screen->renderWindow());
		toImageFilter->SetScale(Scaling, Scaling);
		//export image
		jpegwriter->SetInputConnection(toImageFilter->GetOutputPort());
		//recreate molecule, because upscaling of image is not corect to 2D labels
		UpdateScreen();
		jpegwriter->Write();
	}
	else if (filetype == std::string(".png"))
	{
		//JPEG Writer
		vtkSmartPointer<vtkPNGWriter> pngwriter = vtkSmartPointer<vtkPNGWriter>::New();
		pngwriter->SetFileName(filenamestr.c_str());
		//convert screen to an exportable image
		vtkSmartPointer<vtkWindowToImageFilter> toImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
		toImageFilter->SetInput(screen->renderWindow());
		toImageFilter->SetScale(Scaling, Scaling);
		//export image
		pngwriter->SetInputConnection(toImageFilter->GetOutputPort());
		//recreate molecule, because upscaling of image is not corect to 2D labels
		UpdateScreen();
		pngwriter->Write();
	}
	else
	{
		filenamestr += ".jpg";
		//JPEG Writer
		vtkSmartPointer<vtkJPEGWriter> jpegwriter = vtkSmartPointer<vtkJPEGWriter>::New();
		jpegwriter->SetFileName((filenamestr).c_str());
		//convert screen to an exportable image
		vtkSmartPointer<vtkWindowToImageFilter> toImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
		toImageFilter->SetInput(screen->renderWindow());
		toImageFilter->SetScale(Scaling, Scaling);
		//export image
		jpegwriter->SetInputConnection(toImageFilter->GetOutputPort());
		//recreate molecule, because upscaling of image is not corect to 2D labels
		UpdateScreen();
		jpegwriter->Write();
	}
	LogWidget->AddToLog(0, "Saved as: " + filenamestr);
}