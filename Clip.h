// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Clip.h - Clip widget
//
// Written by: George Wolberg, 2016
// ======================================================================

#ifndef CLIP_H
#define CLIP_H

#include "ImageFilter.h"

class Clip : public ImageFilter {
	Q_OBJECT

public:
	Clip				(QWidget *parent = 0);	// constructor
	QGroupBox*	controlPanel	();			// create control panel
	bool		applyFilter	(ImagePtr, ImagePtr);	// apply filter to input
	void		reset		();			// reset parameters
	void		clip		(ImagePtr, int, int, ImagePtr);

    QLatin1String shaderFileName() const;//returns shader source
    QVector<ShaderParameter>* parameters()   const;

protected slots:
	void changeThr1	(int);
	void changeThr2	(int);

private:
	// widgets
	QSlider		*m_slider [2];	// clip sliders
	QSpinBox	*m_spinBox[2];	// clip spin boxes
	QGroupBox	*m_ctrlGrp;	// groupbox for panel

    //parameters
    ShaderParameter           m_param_threshold1;
    ShaderParameter           m_param_threshold2;
    QVector<ShaderParameter>* m_params;
    float       *m_threshold1;
    float       *m_threshold2;
};

#endif	// CLIP_H
