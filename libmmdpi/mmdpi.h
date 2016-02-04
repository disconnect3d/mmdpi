﻿
#include "pmd/h/pmd.h"
#include "pmx/h/pmx.h"
#include "vmd/vmd.h"

class mmdpi
{
protected :

	mmdpiModel*				pmm;
	mmdpiPmx*				pmx;
	mmdpiPmd*				pmd;
	vector< mmdpiVmd* >		vmd;
	
public :

	virtual int			load( const char* model_name );
	virtual int			vmd_load( const char *file_name );
	virtual mmdpiVmd*	get_vmd( int index );

	virtual void		draw( void );
	virtual void		set_bone_matrix( uint bone_index, mmdpiMatrix& matrix );
	virtual void		set_fps( int fps );
	virtual void		set_projection_matrix( const GLfloat* p_projection_matrix );	

	mmdpi();
	~mmdpi();
} ;