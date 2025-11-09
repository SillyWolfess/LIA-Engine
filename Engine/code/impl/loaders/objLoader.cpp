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
	if (mtlUsed) {
		object.materialLib = {usemtlLib};
	} else {
		object.materialLib = "";
	}
	int last_vertex=0;
	int last_vertices_out=0;
	
	LIA_trace("loading obj");

	int LastMtl=-1;
	int numberMtL=-1;
	bool texture_loaded=false;
	bool vector_loaded=false;
	while(!feof(file) )
	{
		res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break; // EOF = End Of File. Quit the loop.
		}
		if ( strcmp( lineHeader, "o" ) == 0 ) {
			char text[255];
			fscanf(file, "%s\n",text);
			texture_loaded=false;
			vector_loaded=false;
		} else if ( strcmp( lineHeader, "usemtl" ) == 0 ) {
			if(!mtlUsed) {
				LIA_error("ERROR : using material with missing material file header !!");
			    return;
			}
			
			char usemtlString[255];
			fscanf(file, "%s \n", usemtlString);
			numberMtL=-1;
			for (int m=0; m < LastMtl + 1; m++) {
				if (strcmp(usemtlString, object.materialName[m].c_str()) == 0) {
					numberMtL = m;
					break;
				}
			}

			if(numberMtL!=-1) {
				sprintf(info, "using mtl : '%s' since %d", usemtlString, object.vertices.size());
				LIA_debug(info);
			} else {
				sprintf(info, "New mtl : '%s' since %d", usemtlString, object.vertices.size());
				LIA_debug(info);
			}
			object.offsets.push_back(object.vertices.size());
			std::string mtlName = {usemtlString};
		//	strcpy(mtlName, usemtlString);
			object.materialName.push_back(mtlName);
			LastMtl++;
			numberMtL = LastMtl;
			if (temp_colours.size() < LastMtl + 1) {
				temp_colours.push_back(glm::vec3(1,0,0));
			}
			LIA_debug_f("Offset {}", object.vertices.size());
		}
		else
		if ( strcmp( lineHeader, "v" ) == 0 ) {
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
				/*
				object.materialIds.push_back(numberMtL);
				object.materialIds.push_back(numberMtL);
				object.materialIds.push_back(numberMtL);
				*/
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