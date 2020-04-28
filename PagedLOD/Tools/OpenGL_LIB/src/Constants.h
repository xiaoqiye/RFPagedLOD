#pragma once
#include <string>
namespace Constants
{
	enum class ECameraMovement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	enum class EBlockType
	{
		UNDEFINED = 1,
		ATOMIC_BUFFER,
		UNIFORM_BUFFER,
		SHADER_STORAGE_BUFFER,
	};

	enum class EBufferType
	{
		UNDEFINED = 1,
		ATOMIC_BUFFER,
		UNIFORM_BUFFER,
		SHADER_STORAGE_BUFFER,
		DRAW_INDIRECT_BUFFER
	};

	enum class EBufferUsage
	{
		UNDEFINED = 1,
		STATIC_DRAW,
		DYNAMIC_DRAW,
	};

	const float DEFAULT_CAMERA_POSITION[3] = { 0.0f, 0.0f, 0.0f };
	const float DEFAULT_YAW = -180.0f;
	const float DEFAULT_SPEED = 3.0f;
	const float DEFAULT_CAMERA_PITCH = -30.0f;
	const float DEFAULT_SENSITIVITY = 0.02f;
	const float DEFAULT_ZOOM = 45.0f;
	const float DEFAULT_NEAR_PLANE = 0.1f;
	const float DEFAULT_FAR_PLANE = 100.0f;
	const float WORLD_UP_VEC3[3] = { 0.0f, 1.0f, 0.0f };
	const std::string CLUSTER_INFO_FILE_NAME = "\\clusterInfo.bin";
	const std::string CLUSTER_SIZE = "uClusterSetSize";

	const std::string JSON_MESH = "mesh";
	const std::string JSON_TEXTURE = "texture";
	const std::string JSON_ATTRIBUTE = "attribute";
	const std::string JSON_GEO_ID = "geometryId";
	const std::string JSON_MTL_ID = "materialId";
	const std::string JSON_GEO_FILE_PATH = "geometry_file_path";
	const std::string JSON_MTL_FILE_PATH = "material_file_path";
	const std::string JSON_DEFAULT_GEO_FILE_NAME = "geometryInfo.bin";
	const std::string JSON_DEFAULT_MTL_FILE_NAME = "materialInfo.json";
}