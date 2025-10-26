#include "loaders/objLoader.hpp"
#include "logs.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>

void LIA::ObjLoader::load(ModelData& object, std::string dir, std::string path) {
	load(object, dir.c_str(), path.c_str());
}

/***
 * Kd - color [done]
 * map_Kd - color texture [done]
 * 
 * Ka - ambient reflectivity [done]
 * 
 * Ni - optical density - index of refraction (glass)
 * 
 * map_Bump - bump texture [done]
 * 
 * Ns - shininess exponent [done]
 * map_Ns - shininess texture
 * 
 * Ks - Specular Color [done]
 * map_Ks - specular texture 
 * 
 * Pr - Roughness
 * map_Pr - roughness texture
 * 
 * Ke - emissive color
 * map_Ke - emissive texture [done]
 */
void LIA::ObjLoader::load(ModelData& object, const char * dir, const char * path) {
    object._complete = false;

    int nFace = 0;
	int nVertex = 0;
	char info[500];
	
	sprintf(info,"[1]Loading OBJ file '%s'", path);
    LIA_info(info);
	
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec3> temp_colours;
	
	float temp_min_x=MAX_VALUE;
	float temp_min_y=MAX_VALUE;
 	float temp_min_z=MAX_VALUE;
	float temp_max_x=MIN_VALUE;
	float temp_max_y=MIN_VALUE;
	float temp_max_z=MIN_VALUE;
	
	
	FILE * file = fopen(path, "r");
	if( file == NULL )
	{
		sprintf(info,"ERROR: Impossible to open the file '%s' ! ",path);
        LIA_error(info);
		return;
	}
	char usemtlLib[255];
	char lineHeader[128];
	
	int res=0;
	bool mtlUsed=false;
	while(1)
	{
		res = fscanf(file, "%s", lineHeader);
		if ( strcmp( lineHeader, "mtllib" ) == 0 )
		{
			fscanf(file, "%s \n",usemtlLib);	
			mtlUsed=true;
			break;	
		}
		else
		if ( strcmp( lineHeader, "#" ) == 0 )
		{
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);	
		}
		else
		{
			break;
		}
	}
	
	int mtlSize=0;
	int last_vertex=0;
	int last_vertices_out=0;
	
	FILE *mtlFile= NULL;
	if(mtlUsed)
	{
		char  mtl_path[1000]="";
		strcat(mtl_path,dir);
		strcat(mtl_path,usemtlLib);
		mtlFile= fopen(mtl_path, "r");
		if( mtlFile == NULL )
		{
			sprintf(info,"ERROR: Impossible to open the material file %s ! ",mtl_path);
            LIA_error(info);
			mtlUsed=false;
		}
	}
	if(mtlUsed)
	{
		char stupidBuffer[1000];
		fgets(stupidBuffer, 1000, mtlFile);
		int number=-1;
		fscanf(mtlFile, "# Material Count: %d",&number);
		
		if(number > 0)
		{
			mtlSize=number;
            LIA_debug(std::vformat("#mtl {}", std::make_format_args(mtlSize)));
		}
		else
		{
			LIA_warn("MTL WARN : Not declared the count of materials !!!");
		}
	}
	object.materials.clear();
	object.materials.reserve(20);
	object.materialIds.clear();

	int LastMtl=-1;
	if(mtlUsed)
	{
 	
		while(!feof(mtlFile))
		{
			fscanf(mtlFile, "%s", lineHeader);
			LIA_trace(lineHeader);
			if(strcmp(lineHeader,"newmtl")==0)
			{
				char text[255];
				fscanf(mtlFile,"%s\n",&text);
				LIA_trace(text);

				LastMtl++;
//				if (mtlSize < LastMtl + 1) {
					LIA_trace("make new material");
					object.materials.emplace_back();
					gMaterial& smt = object.materials[object.materials.size() - 1];
					smt.Kd[0] = 0; smt.Kd[1] = 0; smt.Kd[2] = 0;
					smt.Ka[0] = 0; smt.Ka[1] = 0; smt.Ka[2] = 0;
					smt.Ks[0] = 0; smt.Ks[1] = 0; smt.Ks[2] = 0;
					smt.Ke[0] = 0; smt.Ke[1] = 0; smt.Ke[2] = 0;
					LIA_trace("push new material");
//					object.materials.emplace_back(smt);
					glm::vec3 d_c;
					d_c.x = 1;
					d_c.y = 0;
					d_c.z = 0;
					LIA_trace("push new color");
					temp_colours.push_back(d_c);
//				}
				LIA_trace("copy material name");
				strcpy(object.materials[LastMtl].name, text);
				LIA_trace("name copied");
			}
			else
			if(strcmp(lineHeader,"Ns")==0)
			{
				float read=0;
				fscanf(mtlFile," %f\n",&read);
				object.materials[LastMtl].Ns=read;
			}
			else
			if(strcmp(lineHeader,"Ka")==0)
			{
				glm::vec3 read;
				fscanf(mtlFile," %f %f %f\n",&read.x,&read.y,&read.z);
				object.materials[LastMtl].Ka[0]=read.x;
				object.materials[LastMtl].Ka[1]=read.y;
				object.materials[LastMtl].Ka[2]=read.z;
			}
			else
			if(strcmp(lineHeader,"Kd")==0)
			{
				glm::vec3 read;
				fscanf(mtlFile," %f %f %f\n",&read.x,&read.y,&read.z);
				object.materials[LastMtl].Kd[0]=read.x;
				object.materials[LastMtl].Kd[1]=read.y;
				object.materials[LastMtl].Kd[2]=read.z;
				if(temp_colours.size() < LastMtl + 1) {
					temp_colours.push_back(glm::vec3(1,0,0));
				}
				temp_colours[LastMtl][0] = read.x;
				temp_colours[LastMtl][1] = read.y;
				temp_colours[LastMtl][2] = read.z;
			}
			else
			if(strcmp(lineHeader,"Ks")==0)
			{
				float x,y,z;
				fscanf(mtlFile," %f %f %f\n",&x,&y,&z);
				object.materials[LastMtl].Ks[0]=x;
				object.materials[LastMtl].Ks[1]=x;
				object.materials[LastMtl].Ks[2]=x;
			}
			else if(strcmp(lineHeader,"Ke")==0)
			{
				float x,y,z;
				fscanf(mtlFile," %f %f %f\n",&x,&y,&z);
				object.materials[LastMtl].Ke[0]=x;
				object.materials[LastMtl].Ke[1]=x;
				object.materials[LastMtl].Ke[2]=x;
			}
			else
			if(strcmp(lineHeader,"Ni")==0)
			{
				float read=0;
				fscanf(mtlFile," %f\n",&read);
				object.materials[LastMtl].Ni=read;
			}
			else
			if(strcmp(lineHeader,"illum")==0)
			{
				int read=0;
				fscanf(mtlFile," %d\n",&read);
				object.materials[LastMtl].illum=read;
			}
			else if(strcmp(lineHeader,"map_Kd")==0) {
				char text[255];
				fscanf(mtlFile,"%s\n",&text);
				strcpy(object.materials[LastMtl].texture, text);
				object.materials[LastMtl].hasTexture = true;
			}
			else if(strcmp(lineHeader,"map_Ke")==0) {
				char text[255];
				fscanf(mtlFile,"%s\n",&text);
				strcpy(object.materials[LastMtl].texture_em, text);
				object.materials[LastMtl].hasEm = true;
			}
			else if (strcmp(lineHeader,"map_Bump")==0) {
				char text[255];
				fscanf(mtlFile," -bm 1.000000 %s\n",&text);
				strcpy(object.materials[LastMtl].texture_bump, text);
				object.materials[LastMtl].hasBump = true;
			}
			else
			{
				char stupidBuffer[1000];
				fgets(stupidBuffer, 1000, mtlFile);
			}
			
		}
	}
	if(usemtlLib)
	{
		fclose(mtlFile);
	}
    
    LIA_trace("mtl loaded");
	LIA_trace("loading obj");

	int numberMtL=-1;
	bool texture_loaded=false;
	bool vector_loaded=false;
	while(!feof(file) )
	{
		res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			break; // EOF = End Of File. Quit the loop.
		}
		if ( strcmp( lineHeader, "o" ) == 0 )
		{
			char text[255];
			fscanf(file, "%s\n",text);
			texture_loaded=false;
			vector_loaded=false;
		}
		else
		if ( strcmp( lineHeader, "usemtl" ) == 0 )
		{
			if(!mtlUsed)
			{
				LIA_error("ERROR : using material with missing material file header !!");
			    return;
			}
			
			char usemtlString[255];
			fscanf(file, "%s \n",usemtlString);
			numberMtL=-1;
			for(int m=0;m<LastMtl+1;m++)
			{
				if(strcmp(usemtlString, object.materials[m].name)==0)
				{
					numberMtL=m;
					break;
				}
			}
			if(numberMtL!=-1)
			{
				sprintf(info, "using mtl :  %s = %s since %d",usemtlString, object.materials[numberMtL].texture, object.vertices.size());
				LIA_debug(info);
			}
			else
			{
				sprintf(info, "WARNING: using mtl :  %s FALSE since %d",usemtlString, object.vertices.size());
				LIA_warn(info);
				return;
			}
			object.offsets.push_back(object.vertices.size());
			LIA_debug_f("Offset {}", object.vertices.size());
		}
		else
		if ( strcmp( lineHeader, "v" ) == 0 )
		{
			glm::vec3 read;
			fscanf(file, "%f %f %f\n", &read.x, &read.y, &read.z );

			float x=read.x;
			float y=read.y;
			float z=read.z;
			
			if(x<temp_min_x) {
				temp_min_x=x;
			}
			if(y<temp_min_y) {
				temp_min_y=y;
			}
			if(z<temp_min_z) {
				temp_min_z=z;
			}
			
			if(x>temp_max_x) {
				temp_max_x=x;
			}
			if(y>temp_max_y) {
				temp_max_y=y;
			}
			
			if(z>temp_max_z) {
				temp_max_z=z;
			}
			temp_vertices.push_back(read);
			nVertex++;
		}
		else
		if ( strcmp( lineHeader, "vt" ) == 0 )
		{
			glm::vec2 read;
			fscanf(file, "%f %f\n", &read.x, &read.y);
			temp_uvs.push_back(read);
			texture_loaded=true;
//			std::cout << "(vt)" << read.x << "," << read.y << std::endl;
		}
		else
		if ( strcmp( lineHeader, "vn" ) == 0 )
		{
			glm::vec3 read;
			fscanf(file, "%f %f %f\n", &read.x, &read.y, &read.z );
			temp_normals.push_back(read);
			vector_loaded=true;
		}
		else
		if ( strcmp( lineHeader, "f" ) == 0 )
		{
			int read_vertex[3];
			int read_texture[3];
			int read_normal_vector[3];
			
			for(int i=0;i<3;i++)
			{
				read_vertex[i] = 0;
				read_texture[i] = 0;
				read_normal_vector[i] = 0;

				fscanf(file,"%d",&read_vertex[i]);
				if(texture_loaded)
				{
					fscanf(file,"/%d",&read_texture[i]);
				}
				if(vector_loaded)
				{
					if(!texture_loaded)
					{
						fscanf(file,"/");
					}
					fscanf(file,"/%d",&read_normal_vector[i]);
				}
				fscanf(file,"\n");
			}
			
			glm::vec3 vertex = temp_vertices[read_vertex[0]-1];
			object.vertices.push_back(vertex);
			last_vertices_out++;
			
			vertex = temp_vertices[read_vertex[1]-1];
			object.vertices.push_back(vertex);
			last_vertices_out++;
			
			vertex = temp_vertices[read_vertex[2]-1];
			object.vertices.push_back(vertex);
			last_vertices_out++;
			
			if (temp_colours.size() < numberMtL) {
				LIA_error(std::vformat("ERROR FATAL: [mtl is out of bounds] : {} < {}",std::make_format_args(temp_colours.size(), numberMtL)));
				return;
			}

			vertex = temp_colours[numberMtL];
			object.colours.push_back(vertex);
			object.colours.push_back(vertex);
			object.colours.push_back(vertex);

			if (texture_loaded) {
				object.materialIds.push_back(numberMtL);
				object.materialIds.push_back(numberMtL);
				object.materialIds.push_back(numberMtL);
				object.uvs.push_back(temp_uvs[read_texture[0]-1]);
				object.uvs.push_back(temp_uvs[read_texture[1]-1]);
				object.uvs.push_back(temp_uvs[read_texture[2]-1]);
			}
			
			if(vector_loaded)
			{
				vertex = temp_normals[read_normal_vector[0]-1];
				object.normals.push_back(vertex);
				vertex = temp_normals[read_normal_vector[1]-1];
				object.normals.push_back(vertex);
				vertex = temp_normals[read_normal_vector[2]-1];
				object.normals.push_back(vertex);
			}
			nFace++;
		}
		else
		{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	
	object.min_x=temp_min_x;
	object.min_y=temp_min_y;
	object.min_z=temp_min_z;
	
	object.max_x=temp_max_x;
	object.max_y=temp_max_y;
	object.max_z=temp_max_z;
	char result[500];
	sprintf(result, "%d vertices, %d faces, %d vertices for faces",nVertex, nFace, object.vertices.size());
	fclose(file);
    LIA_trace(result);
	sprintf(result, "%d vertices %d normals %d uvs", object.vertices.size(), object.normals.size(), object.uvs.size());
	LIA_trace(result);
	LIA_debug("done");
    object._complete = true;
    return;
}