﻿
#include "pmx_load.h"
#include "../../model/h/mmdpi_model.h"

#pragma once

class mmdpiPmxAnalyze : public mmdpiPmxLoad, public mmdpiModel
{
private :
	
	MMDPI_MATERIAL_PTR		adjust_material;
	MMDPI_BLOCK_VERTEX_PTR	adjust_vertex;

	MMDPI_IMAGE*			texture00;
	MMDPI_IMAGE*			toon_texture00;
	MMDPI_IMAGE*			texture;
	MMDPI_IMAGE*			toon_texture;
	
	void					load_texture( void );
	int						analyze( void );
	
protected:
	int						create_bone( MMDPI_PMX_BONE_INFO_PTR pbone, uint pbone_len );
	
public :
	virtual int				load( const char *file_name );

	mmdpiPmxAnalyze();
	~mmdpiPmxAnalyze();
} ;