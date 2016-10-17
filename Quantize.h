// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Quantize.h - Quantize widget
//
// Written by: George Wolberg, 2016
// ======================================================================

#ifndef QUANTIZE_H
#define QUANTIZE_H

#include "ImageFilter.h"


class Quantize : public ImageFilter {
	Q_OBJECT

public:
	Quantize			(QWidget *parent = 0);	// constructor
	QGroupBox*	controlPanel	();			// create control panel
	bool		applyFilter	(ImagePtr, ImagePtr);	// apply filter to input
	void		reset		();			// reset parameters
	void		quantize	(ImagePtr, int, bool, ImagePtr);

    QLatin1String shaderFileName() const;//returns shader source
    QVector<ShaderParameter>* parameters()   const;

protected slots:
	void		changeLevels	(int);
	void		ditherSignal	(int);

private:
	// widgets
	QSlider		*m_slider ;	// Quantize sliders
	QSpinBox	*m_spinBox;	// Quantize spin boxes
	QCheckBox	*m_checkBox;	// Quantize checkbox
	QGroupBox	*m_ctrlGrp;	// Groupbox for panel

    //fragment shader parameters
    ShaderParameter           m_param_levels;
    ShaderParameter           m_param_dither;
    ShaderParameter           m_param_width;
    ShaderParameter           m_param_height;
    QVector<ShaderParameter>* m_params;
    float       *m_levels;
    float       *m_dither;
    int         *m_img_width;
    int         *m_img_height;
};

#endif	// QUANTIZE_H
