// File generated by "Obj2Header.exe" [Version 1.7] Author: L.Norri CD CGS Fullsail University.
// Data is converted to left-handed coordinate system and UV data is V flipped for Direct3D/Vulkan.
/************************************************/
/*  This section contains the model's size data */
/************************************************/
#ifndef _bush_vertexcount_
const unsigned bush_vertexcount = 32;
#define _bush_vertexcount_
#endif
#ifndef _bush_indexcount_
const unsigned bush_indexcount = 48;
#define _bush_indexcount_
#endif
#ifndef _bush_materialcount_
const unsigned bush_materialcount = 1;
#define _bush_materialcount_
#endif
#ifndef _bush_meshcount_
const unsigned bush_meshcount = 2;
#define _bush_meshcount_
#endif
/************************************************/
/*  This section contains the raw data to load  */
/************************************************/
#ifndef __OBJ_VERT__
typedef struct _OBJ_VERT_
{
	float pos[3]; // Left-handed +Z forward coordinate w not provided, assumed to be 1.
	float uvw[3]; // D3D/Vulkan style top left 0,0 coordinate.
	float nrm[3]; // Provided direct from obj file, may or may not be normalized.
}OBJ_VERT;
#define __OBJ_VERT__
#endif
#ifndef _bush_data_
// Raw Vertex Data follows: Positions, Texture Coordinates and Normals.
const OBJ_VERT bush_data[32] =
{
	{	{ -7.419246f, 4.871798f, -2.318891f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.499466f, 0.707107f, -0.500533f }	},
	{	{ 2.334740f, 4.871798f, 7.414306f },	{ 1.000000f, 0.000000f, 0.000000f },	{ 0.499466f, 0.707107f, -0.500533f }	},
	{	{ 9.217150f, -4.871798f, 0.517195f },	{ 1.000000f, 1.000000f, 0.000000f },	{ 0.499466f, 0.707107f, -0.500533f }	},
	{	{ -0.536836f, -4.871798f, -9.216002f },	{ 0.000000f, 1.000000f, 0.000000f },	{ 0.499466f, 0.707107f, -0.500533f }	},
	{	{ 7.291674f, 4.871800f, 2.446786f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.499466f, 0.707107f, 0.500533f }	},
	{	{ -2.462311f, 4.871800f, -7.286413f },	{ 1.000000f, 0.000000f, 0.000000f },	{ -0.499466f, 0.707107f, 0.500533f }	},
	{	{ -9.344723f, -4.871797f, -0.389304f },	{ 1.000000f, 1.000000f, 0.000000f },	{ -0.499466f, 0.707107f, 0.500533f }	},
	{	{ 0.409262f, -4.871797f, 9.343895f },	{ 0.000000f, 1.000000f, 0.000000f },	{ -0.499466f, 0.707107f, 0.500533f }	},
	{	{ -2.318065f, 4.871798f, 7.292503f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.500533f, 0.707107f, -0.499466f }	},
	{	{ 7.415133f, 4.871799f, -2.461485f },	{ 1.000000f, 0.000000f, 0.000000f },	{ -0.500533f, 0.707107f, -0.499466f }	},
	{	{ 0.518022f, -4.871798f, -9.343895f },	{ 1.000000f, 1.000000f, 0.000000f },	{ -0.500533f, 0.707107f, -0.499466f }	},
	{	{ -9.215176f, -4.871799f, 0.410092f },	{ 0.000000f, 1.000000f, 0.000000f },	{ -0.500533f, 0.707107f, -0.499466f }	},
	{	{ 2.447613f, 4.871801f, -7.418419f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.500533f, 0.707107f, 0.499466f }	},
	{	{ -7.285587f, 4.871800f, 2.335568f },	{ 1.000000f, 0.000000f, 0.000000f },	{ 0.500533f, 0.707107f, 0.499466f }	},
	{	{ -0.388478f, -4.871797f, 9.217979f },	{ 1.000000f, 1.000000f, 0.000000f },	{ 0.500533f, 0.707107f, 0.499466f }	},
	{	{ 9.344723f, -4.871796f, -0.536007f },	{ 0.000000f, 1.000000f, 0.000000f },	{ 0.500533f, 0.707107f, 0.499466f }	},
	{	{ -3.972848f, 4.871799f, -7.790614f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.707107f, 0.707107f, 0.000000f }	},
	{	{ -3.972849f, 4.871798f, 5.988915f },	{ 1.000000f, 0.000000f, 0.000000f },	{ 0.707107f, 0.707107f, 0.000000f }	},
	{	{ 5.770750f, -4.871799f, 5.988914f },	{ 1.000000f, 1.000000f, 0.000000f },	{ 0.707107f, 0.707107f, 0.000000f }	},
	{	{ 5.770751f, -4.871798f, -7.790615f },	{ 0.000000f, 1.000000f, 0.000000f },	{ 0.707107f, 0.707107f, 0.000000f }	},
	{	{ 3.044804f, 4.871800f, 5.988915f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.707107f, 0.707107f, -0.000000f }	},
	{	{ 3.044807f, 4.871801f, -7.790613f },	{ 1.000000f, 0.000000f, 0.000000f },	{ -0.707107f, 0.707107f, -0.000000f }	},
	{	{ -6.698791f, -4.871797f, -7.790615f },	{ 1.000000f, 1.000000f, 0.000000f },	{ -0.707107f, 0.707107f, -0.000000f }	},
	{	{ -6.698795f, -4.871798f, 5.988914f },	{ 0.000000f, 1.000000f, 0.000000f },	{ -0.707107f, 0.707107f, -0.000000f }	},
	{	{ -7.173148f, 4.871798f, 2.609341f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.000000f, 0.707107f, -0.707107f }	},
	{	{ 6.606381f, 4.871799f, 2.609340f },	{ 1.000000f, 0.000000f, 0.000000f },	{ -0.000000f, 0.707107f, -0.707107f }	},
	{	{ 6.606382f, -4.871798f, -7.134260f },	{ 1.000000f, 1.000000f, 0.000000f },	{ -0.000000f, 0.707107f, -0.707107f }	},
	{	{ -7.173147f, -4.871799f, -7.134258f },	{ 0.000000f, 1.000000f, 0.000000f },	{ -0.000000f, 0.707107f, -0.707107f }	},
	{	{ 6.606383f, 4.871802f, -4.408314f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.000000f, 0.707107f, 0.707107f }	},
	{	{ -7.173146f, 4.871800f, -4.408315f },	{ 1.000000f, 0.000000f, 0.000000f },	{ -0.000000f, 0.707107f, 0.707107f }	},
	{	{ -7.173149f, -4.871798f, 5.335284f },	{ 1.000000f, 1.000000f, 0.000000f },	{ -0.000000f, 0.707107f, 0.707107f }	},
	{	{ 6.606381f, -4.871797f, 5.335285f },	{ 0.000000f, 1.000000f, 0.000000f },	{ -0.000000f, 0.707107f, 0.707107f }	},
};
#define _bush_data_
#endif
#ifndef _bush_indicies_
// Index Data follows: Sequential by mesh, Every Three Indicies == One Triangle.
const unsigned int bush_indicies[48] =
{
	 0, 1, 2,
	 3, 0, 2,
	 4, 5, 6,
	 7, 4, 6,
	 8, 9, 10,
	 11, 8, 10,
	 12, 13, 14,
	 15, 12, 14,
	 16, 17, 18,
	 19, 16, 18,
	 20, 21, 22,
	 23, 20, 22,
	 24, 25, 26,
	 27, 24, 26,
	 28, 29, 30,
	 31, 28, 30,
};
#define _bush_indicies_
#endif
// Part of an OBJ_MATERIAL, the struct is 16 byte aligned so it is GPU register friendly.
#ifndef __OBJ_ATTRIBUTES__
typedef struct _OBJ_ATTRIBUTES_
{
	float       Kd[3]; // diffuse reflectivity
	float	    d; // dissolve (transparency) 
	float       Ks[3]; // specular reflectivity
	float       Ns; // specular exponent
	float       Ka[3]; // ambient reflectivity
	float       sharpness; // local reflection map sharpness
	float       Tf[3]; // transmission filter
	float       Ni; // optical density (index of refraction)
	float       Ke[3]; // emissive reflectivity
	unsigned    illum; // illumination model
}OBJ_ATTRIBUTES;
#define __OBJ_ATTRIBUTES__
#endif
// This structure also has been made GPU register friendly so it can be transfered directly if desired.
// Note: Total struct size will vary depenedening on CPU processor architecture (string pointers).
#ifndef __OBJ_MATERIAL__
typedef struct _OBJ_MATERIAL_
{
	// The following items are typically used in a pixel/fragment shader, they are packed for GPU registers.
	OBJ_ATTRIBUTES attrib; // Surface shading characteristics & illumination model
	// All items below this line are not needed on the GPU and are generally only used during load time.
	const char* name; // the name of this material
	// If the model's materials contain any specific texture data it will be located below.
	const char* map_Kd; // diffuse texture
	const char* map_Ks; // specular texture
	const char* map_Ka; // ambient texture
	const char* map_Ke; // emissive texture
	const char* map_Ns; // specular exponent texture
	const char* map_d; // transparency texture
	const char* disp; // roughness map (displacement)
	const char* decal; // decal texture (lerps texture & material colors)
	const char* bump; // normal/bumpmap texture
	void* padding[2]; // 16 byte alignment on 32bit or 64bit
}OBJ_MATERIAL;
#define __OBJ_MATERIAL__
#endif
#ifndef _bush_materials_
// Material Data follows: pulled from a .mtl file of the same name if present.
const OBJ_MATERIAL bush_materials[1] =
{
	{
		{{ 0.600000f, 0.600000f, 0.600000f },
		1.000000f,
		{ 0.200000f, 0.200000f, 0.200000f },
		500.000000f,
		{ 1.000000f, 1.000000f, 1.000000f },
		60.000000f,
		{ 1.000000f, 1.000000f, 1.000000f },
		1.500000f,
		{ 0.000000f, 0.000000f, 0.000000f },
		2},
		"default",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
	},
};
#define _bush_materials_
#endif
/************************************************/
/*  This section contains the model's structure */
/************************************************/
#ifndef _bush_batches_
// Use this conveinence array to batch render all geometry by like material.
// Each entry corresponds to the same entry in the materials array above.
// The two numbers provided are the IndexCount and the IndexOffset into the indicies array.
// If you need more fine grained control(ex: for transformations) use the OBJ_MESH data below.
const unsigned int bush_batches[1][2] =
{
	{ 48, 0 },
};
#define _bush_batches_
#endif
#ifndef __OBJ_MESH__
typedef struct _OBJ_MESH_
{
	const char* name;
	unsigned    indexCount;
	unsigned    indexOffset;
	unsigned    materialIndex;
}OBJ_MESH;
#define __OBJ_MESH__
#endif
#ifndef _bush_meshes_
// Mesh Data follows: Meshes are .obj groups sorted & split by material.
// Meshes are provided in sequential order, sorted by material first and name second.
const OBJ_MESH bush_meshes[2] =
{
	{
		"1_-_Default",
		48,
		0,
		0,
	},
	{
		"default",
		0,
		48,
		0,
	},
};
#define _bush_meshes_
#endif
