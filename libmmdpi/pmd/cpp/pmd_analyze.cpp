﻿
#include "../h/pmd_analyze.h"


// 読み込み
int mmdpiPmdAnalyze::load( const char *file_name )
{
	if( mmdpiPmdLoad::load( file_name ) )
		return -1;
	return analyze();
}

//	PMD解析
int mmdpiPmdAnalyze::analyze( void )
{
	adjust_material = new MMDPI_MATERIAL[ material_num ];
	for( dword i = 0; i < material_num; i ++ )
		adjust_material[ i ].face_num = material[ i ].face_vert_count;

	adjust_vertex = new MMDPI_BLOCK_VERTEX();
	adjust_vertex->alloc( mmdpiPmdLoad::vertex_num );

	for( dword i = 0; i < mmdpiPmdLoad::vertex_num; i ++ )
	{
		MMDPI_PMD_VERTEX_PTR	ver = &vertex[ i ];

		adjust_vertex->ver[ i ].x = ver->pos[ 0 ];
		adjust_vertex->ver[ i ].y = ver->pos[ 1 ];
		adjust_vertex->ver[ i ].z = ver->pos[ 2 ];

		adjust_vertex->uv[ i ].x = ver->uv[ 0 ];
		adjust_vertex->uv[ i ].y = 1 - ver->uv[ 1 ];
		adjust_vertex->uv[ i ].z = 0;
		adjust_vertex->uv[ i ].w = 0;

		adjust_vertex->nor[ i ].x = ver->nor[ 0 ];
		adjust_vertex->nor[ i ].y = ver->nor[ 1 ];
		adjust_vertex->nor[ i ].z = ver->nor[ 2 ];

		//	ボーン
		adjust_vertex->index[ i ].x = ( float )ver->bone_num[ 0 ];
		adjust_vertex->index[ i ].y = ( float )ver->bone_num[ 1 ];
		adjust_vertex->index[ i ].z = ( float )0;
		adjust_vertex->index[ i ].w = ( float )0;
			
		//	重み
		adjust_vertex->weight[ i ].x = ver->bone_weight / 100.0f;
		adjust_vertex->weight[ i ].y = 1.0f - adjust_vertex->weight[ i ].x;
		adjust_vertex->weight[ i ].z = 0;
		adjust_vertex->weight[ i ].w = 0;
	}

	//	最適化
	mmdpiAdjust::adjust_material_bone( material_num, adjust_material, mmdpiPmdLoad::bone_num, face, adjust_vertex );
	mmdpiAdjust::adjust_polygon( face, face_num, adjust_vertex, mmdpiPmdLoad::vertex_num );
	mmdpiAdjust::adjust_face( face, face_num, mmdpiPmdLoad::vertex_num );
	mmdpiAdjust::adjust_bone();
	
	//	テクスチャ
	load_texture();
		
	//skin = new MMDPI_SKIN_INFO[ skin_num ];
	//for( uint i = 0; i < skin_num; i ++ )
	//{
	//	a_skin[ i ].skin_flag = 0;
	//	a_skin[ i ].skin_name = skin[ i ].skin_name;
	//}

	//	マテリアル情報の保存
	for( dword j = 0; j < get_face_num(); j ++ )
	{
		MMDPI_BLOCK_FACE_PTR	f = &get_face_block()[ j ];
		for( dword i = 0; i < f->material_num; i ++ )
		{
			MMDPI_MATERIAL_PTR		m		= f->material[ i ];
			MMDPI_PMD_MATERIAL_PTR	mpmx	= &material[ m->pid ];
		
			m->edge_size = mpmx->edge_flag * 0.02f;

			m->edge_color.r = 0;
			m->edge_color.g = 0;
			m->edge_color.b = 0;
			m->edge_color.a = 1;

			m->opacity = mpmx->alpha;
		}
	}

	return 0;
}

void mmdpiPmdAnalyze::load_texture( void )
{
	//	テクスチャ
	dword	fver_num_base = 0;
	long	ppid = -1;
	dword	pi = 0;

	//	一時的に読み込み
	texture			= new MMDPI_IMAGE[ get_material_num() ];
	toon_texture	= new MMDPI_IMAGE[ get_material_num() ];

	for( dword j = 0; j < get_face_num(); j ++ )
	{
		MMDPI_BLOCK_FACE_PTR	f = &get_face_block()[ j ];
		for( dword i = 0; i < f->material_num; i ++ )
		{
			MMDPI_MATERIAL_PTR		m		= f->material[ i ];
			MMDPI_PMD_MATERIAL_PTR	mpmx	= &material[ m->pid ];
			char*	texture_file_name;

			texture_file_name = mpmx->texture_file_name;

			char	texture_file_name_full[ 0xffff ];
			int		j, k;	
			for( k = 0; directory[ k ]; k ++ )
				texture_file_name_full[ k ] = directory[ k ];

			for( j = 0;
				texture_file_name[ j ] 
				&& texture_file_name[ j ] != '*';
				j ++, k ++ 
			)
				texture_file_name_full[ k ] = texture_file_name[ j ];
			texture_file_name_full[ k ] = '\0';
	
			//	テクスチャの関連付け
			m->texture.load( texture_file_name_full );
			//m->toon_texture.copy( toon_texture00[ mpmx->toon_texture_number ] );
		}
	}
}

//	ボーン関係処理
int	mmdpiPmdAnalyze::create_bone( MMDPI_PMD_BONE_INFO_PTR pbone, uint pbone_len )
{
	mmdpiModel::bone_num = pbone_len;
	mmdpiModel::bone = new MMDPI_BONE_INFO[ mmdpiModel::bone_num ];
	
	for( dword i = 0; i < mmdpiModel::bone_num; i ++ )
	{
		mmdpiModel::bone[ i ].parent		= NULL;
		mmdpiModel::bone[ i ].first_child	= NULL;
		mmdpiModel::bone[ i ].sibling		= NULL;
	}

	for( dword i = 0; i < mmdpiModel::bone_num; i ++ )
	{
		mmdpiModel::bone[ i ].id = i;

		//	親ボーン設定
		if( pbone[ i ].parent_bone_index < mmdpiModel::bone_num )
			mmdpiModel::bone[ i ].parent = &mmdpiModel::bone[ pbone[ i ].parent_bone_index ];

		//	子供ボーン設定
		int child_id = -1;
		for( dword j = 0; j < mmdpiModel::bone_num && child_id == -1; j ++ )
		{
			if( pbone[ j ].parent_bone_index == i )
				child_id = j;				
		}
		if( child_id != -1 ) 
			mmdpiModel::bone[ i ].first_child = &mmdpiModel::bone[ child_id ];

		//	兄弟ボーン設定
		int sibling_id = -1;
		for( dword j = i + 1; j < mmdpiModel::bone_num && sibling_id == -1; j ++ )
		{
			if( pbone[ j ].parent_bone_index == pbone[ i ].parent_bone_index )
				sibling_id = j;				
		}
		if( sibling_id != -1 ) 
			mmdpiModel::bone[ i ].sibling = &mmdpiModel::bone[ sibling_id ];

		mmdpiModel::bone[ i ].init_mat.transelation( pbone[ i ].bone_head_pos[ 0 ], pbone[ i ].bone_head_pos[ 1 ], pbone[ i ].bone_head_pos[ 2 ] );
		mmdpiModel::bone[ i ].visible = 0;
		mmdpiModel::bone[ i ].length = 0;

	
		mmdpiModel::bone[ i ].name = new char[ strlen( pbone[ i ].bone_name ) + 1 ];
		strcpy( mmdpiModel::bone[ i ].name, pbone[ i ].bone_name );
	}

	for( dword i = 0; i < mmdpiModel::bone_num; i ++ )
		mmdpiModel::bone[ i ].offset_mat = mmdpiModel::bone[ i ].init_mat.get_inverse();

	//	init_mat 初期化
	init_mat_calc( &mmdpiModel::bone[ 0 ], 0 );

	//	初期設定
	refresh_bone_mat();
	make_matrix( &mmdpiModel::bone[ 0 ], 0 );
		
	return 0;
}

mmdpiPmdAnalyze::mmdpiPmdAnalyze()
{
	adjust_material = 0x00;
	adjust_vertex	= 0x00;

	texture			= 0x00;
	toon_texture	= 0x00;
}

mmdpiPmdAnalyze::~mmdpiPmdAnalyze()
{
	delete[] adjust_material;
	delete adjust_vertex;

	delete[] texture;
	delete[] toon_texture;
}