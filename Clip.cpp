// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Clip.cpp - Brightness/Clip widget.
//
// Written by: George Wolberg, 2016
// ======================================================================

#include "MainWindow.h"
#include "Clip.h"
#include "hw1/HW_clip.cpp"

extern MainWindow *g_mainWindowP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clip::Clip:
//
// Constructor.
//
Clip::Clip(QWidget *parent) : ImageFilter(parent)
{
    //define threshold 1 values
    m_threshold1 = new float(0.0);


    m_param_threshold1.name = "threshold1";
    m_param_threshold1.type = ParamType::Float;
    m_param_threshold1.value = m_threshold1;

    //define threshold 2 values
    m_threshold2 = new float(1.0);

    m_param_threshold2.name = "threshold2";
    m_param_threshold2.type = ParamType::Float;
    m_param_threshold2.value = m_threshold2;

    //append the parameters to parameter list or vector
    m_params = new QVector<ShaderParameter>();
    m_params->append(m_param_threshold1);
    m_params->append(m_param_threshold2);



}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clip::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Clip::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Clip");

	// layout for assembling filter widget
	QGridLayout *layout = new QGridLayout;

	// alloc array of labels
	QLabel *label[2];

	// create sliders and spinboxes
	for(int i=0; i<2; i++) {
		// create label[i]
		label[i] = new QLabel(m_ctrlGrp);
		if(!i) label[i]->setText("Thr1");
		else   label[i]->setText("Thr2");

		// create slider and spinbox
		m_slider [i] = new QSlider(Qt::Horizontal, m_ctrlGrp);
		m_spinBox[i] = new QSpinBox(m_ctrlGrp);

		// init slider range and value
		m_slider [i]->setRange(0, MaxGray);
		m_slider [i]->setValue(i* MaxGray);

		// init spinbox range and value
		m_spinBox[i]->setRange(0, MaxGray);
		m_spinBox[i]->setValue(i* MaxGray);

		// assemble dialog
		layout->addWidget(label    [i], i, 0);
		layout->addWidget(m_slider [i], i, 1);
		layout->addWidget(m_spinBox[i], i, 2);
	}

	// init signal/slot connections for Clip
	connect(m_slider [0], SIGNAL(valueChanged(int)),    this, SLOT(changeThr1(int)));
	connect(m_spinBox[0], SIGNAL(valueChanged(int)),    this, SLOT(changeThr1(int)));
	connect(m_slider [1], SIGNAL(valueChanged(int)),    this, SLOT(changeThr2(int)));
	connect(m_spinBox[1], SIGNAL(valueChanged(int)),    this, SLOT(changeThr2(int)));

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clip::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Clip::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	// collect parameters
	int thr1 = m_slider[0]->value();
	int thr2 = m_slider[1]->value();

        // apply filter
        clip(I1, thr1, thr2, I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clip::clip:
//
// Clip intensities of image I1. Output is in I2.
// If    input<t1: output = t1;
// If t1<input<t2: output = input;
// If      val>t2: output = t2;
//
void
Clip::clip(ImagePtr I1, int t1, int t2, ImagePtr I2)
{   
    if(!g_mainWindowP->harwareAccelOn()){
        //if in CPU mode
        HW_clip(I1, t1, t2, I2);
    }else{
        //if in GPU mode
        //change threshold parameters
        *m_threshold1 =  (float) t1 / (float) MXGRAY;
        *m_threshold2 =  (float) t2 / (float) MXGRAY;

        //the shader will take care of the rest when the
        //image canvas filter is repainted
    }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clip::changeThr1:
//
// Slot to process change in thr1 caused by moving the slider
//
void
Clip::changeThr1(int val)
{
	m_slider [0]->blockSignals(true);
	m_slider [0]->setValue    (val );
	m_slider [0]->blockSignals(false);
	m_spinBox[0]->blockSignals(true);
	m_spinBox[0]->setValue    (val );
	m_spinBox[0]->blockSignals(false);

	// apply filter and display output
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clip::changeThr2:
//
// Slot to process change in thr2 caused by moving the slider
//
void
Clip::changeThr2(int val)
{
	m_slider [1]->blockSignals(true);
	m_slider [1]->setValue    (val );
	m_slider [1]->blockSignals(false);
	m_spinBox[1]->blockSignals(true);
	m_spinBox[1]->setValue    (val );
	m_spinBox[1]->blockSignals(false);

	// apply filter and display output
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clip::reset:
//
// Reset parameters.
//
void
Clip::reset()
{
	for(int i=0; i<2; i++) {
		m_slider [i]->blockSignals(true);
		m_slider [i]->setValue(i*MaxGray);
		m_slider [i]->blockSignals(false);

		m_spinBox[i]->blockSignals(true);
		m_spinBox[i]->setValue(i*MaxGray);
		m_spinBox[i]->blockSignals(false);
	}

	// apply filter and display output
	g_mainWindowP->preview();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clip::shaderFileName:
//
QLatin1String
Clip::shaderFileName() const{
    return QLatin1String(":/clip.frag");
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clip::parameters:
//
//returns a vector of paramters that are used to define the
//uniform values for the shader
QVector<ShaderParameter>*
Clip::parameters()   const{
    return m_params;
}
