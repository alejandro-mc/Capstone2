// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Threshold.h - Threshold widget
//
// Written by: George Wolberg, 2016
// ======================================================================

#ifndef THRESHOLD_H
#define THRESHOLD_H

#include "ImageFilter.h"
#include "shaderparam.h"
#include <QOpenGLShader>


class Threshold : public ImageFilter {
	Q_OBJECT

public:
	Threshold			(QWidget *parent = 0);	// constructor
	QGroupBox*	controlPanel	();			// create control panel
	bool		applyFilter	(ImagePtr, ImagePtr);	// apply filter to input
	void		reset		();			// reset parameters
	void		threshold	(ImagePtr, int, ImagePtr);

    QLatin1String shaderFileName() const;//returns shader source
    QVector<ShaderParameter>* parameters()   const;

protected slots:
	void		changeThr	(int);

private:
	// widgets
	QSlider		*m_slider ;	// Threshold slider
	QSpinBox	*m_spinBox;	// Threshold spinbox
	QGroupBox	*m_ctrlGrp;	// Groupbox for panel

    //parameters
    ShaderParameter           m_param_threshold;
    QVector<ShaderParameter>* m_params;
    float       *m_threshold;
};

#endif	// THRESHOLD_H
