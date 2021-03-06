// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Quantize.cpp - Quantize class
//
// Written by: George Wolberg, 2016
// ======================================================================

#include "MainWindow.h"
#include "Quantize.h"
#include "hw1/HW_quantize.cpp"

extern MainWindow *g_mainWindowP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantize::Quantize:
//
// Constructor.
//
Quantize::Quantize(QWidget *parent) : ImageFilter(parent)
{
    m_levels = new float(3.0);
    m_param_levels.name = "levels";
    m_param_levels.type = ParamType::Float;
    m_param_levels.value = m_levels;

    m_dither = new float(0.0);
    m_param_dither.name = "dither";
    m_param_dither.type = ParamType::Float;
    m_param_dither.value = m_dither;

    //image width and height
    m_img_width  = new int(0);
    m_param_width.name = "width";
    m_param_width.type = ParamType::Int;
    m_param_width.value = m_img_width;

    m_img_height = new int(0);
    m_param_height.name = "height";
    m_param_height.type = ParamType::Int;
    m_param_height.value = m_img_height;

    m_params = new QVector<ShaderParameter>();
    m_params->append(m_param_levels);
    m_params->append(m_param_dither);
    m_params->append(m_param_width);
    m_params->append(m_param_height);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantize::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Quantize::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Quantize");

	// create label
	QLabel *label = new QLabel("Levels");

	// create slider
	m_slider  = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_slider->setRange(2, MXGRAY>>1);
	m_slider->setValue(MXGRAY>>6);
	m_slider->setTickPosition(QSlider::TicksBelow);
	m_slider->setTickInterval(25);

	// create spinbox
	m_spinBox = new QSpinBox(m_ctrlGrp);
	m_spinBox->setRange(2, MXGRAY>>1);
	m_spinBox->setValue(MXGRAY>>6);

	// create checkbox
	m_checkBox = new QCheckBox("Dither Signal");
	m_checkBox-> setCheckState( Qt::Unchecked );

	// init signal/slot connections for Quantize
	connect(m_slider  , SIGNAL(valueChanged(int)), this, SLOT(changeLevels(int)));
	connect(m_spinBox , SIGNAL(valueChanged(int)), this, SLOT(changeLevels(int)));
	connect(m_checkBox, SIGNAL(stateChanged(int)), this, SLOT(ditherSignal(int)));

	// assemble dialog
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(  label  , 0, 0);
	layout->addWidget(m_slider , 0, 1);
	layout->addWidget(m_spinBox, 0, 2);
	layout->addWidget(m_checkBox,1, 0, 1, 3, Qt::AlignLeft);

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantize::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Quantize::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	// get threshold value
	int levels = m_slider->value();

	// error checking
	if(levels < 2 || levels > MXGRAY) return 0;

	// get dither flag
	bool dither = 0;
	if(m_checkBox->checkState() == Qt::Checked)
		dither = 1;

	// apply filter
	quantize(I1, levels, dither, I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantize::quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//
void
Quantize::quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{   
    if(!g_mainWindowP->harwareAccelOn()){
        //if in CPU mode
        HW_quantize(I1, levels, dither, I2);
    }else{
        //if in GPU mode
        //change shader parameter
        *m_levels =  levels - 1;
        *m_dither =  (float) dither;

        *m_img_width  = I1->width();
        *m_img_height = I1->height();
        qDebug() <<"width:" << *m_img_width;
        qDebug() <<"height:"<< *m_img_height;

        //the shader will take care of the rest when the
        //image canvas filter is repainted
    }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantize::changeLevels:
//
// Slot to process change in levels caused by moving the slider.
//
void
Quantize::changeLevels(int levels)
{
	// set slider and spinbox to same value
	m_slider ->blockSignals(true);
	m_slider ->setValue    (levels);
	m_slider ->blockSignals(false);
	m_spinBox->blockSignals(true);
	m_spinBox->setValue    (levels);
	m_spinBox->blockSignals(false);

	// apply filter and display output
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantize::ditherSignal:
//
// Slot to process the checkbox that enables/disables the use of a
// dither signal prior to quantization.
//
void
Quantize::ditherSignal(int flag)
{
	int checkState = m_checkBox->checkState();

	if(flag && checkState != Qt::Checked)
		m_checkBox->setCheckState (Qt::Checked);
	else if(!flag && checkState == Qt::Checked)
		m_checkBox->setCheckState (Qt::Unchecked);

	// apply filter and display output
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantize::reset:
//
// Reset parameters.
//
void
Quantize::reset()
{
	// reset parameters
	int levels = MXGRAY>>6;
	m_slider ->blockSignals(true);
	m_slider ->setValue    (levels);
	m_slider ->blockSignals(false);
	m_spinBox->blockSignals(true);
	m_spinBox->setValue    (levels);
	m_spinBox->blockSignals(false);
	m_checkBox->setCheckState(Qt::Unchecked);

	// apply filter and display output
	g_mainWindowP->preview();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantize::shaderFileName:
//
QLatin1String
Quantize::shaderFileName() const{
    return QLatin1String(":/quantize.frag");
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantize::parameters:
//
//returns a vector of paramters that are used to define the
//uniform values for the shader
QVector<ShaderParameter>*
Quantize::parameters()   const{
    return m_params;
}
