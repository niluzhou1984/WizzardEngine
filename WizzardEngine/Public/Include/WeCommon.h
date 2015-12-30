#ifndef WE_COMMON_H
#define WE_COMMON_H
struct WeQueryType{
	enum  Enum
	{
		QUERY_RAYRECAST = 0,
		QUERY_SWEEP,
		QUERY_OVERLAP,
	};
};

struct WeHitFlag{
	enum Enum

	{
		TRACE_NONE = 0, // no collision
		TRACE_START_SOLID, // start point in solid mesh
		TRACE_END_SOLID, // end point in solid mesh
		TRACE_ALL_SOLID, // start and end point in solid mesh
		TRACE_OVERLAP, // overlap 
	};
};

struct WeSceneFlag{
	enum Enum
	{
		SCENE_MULTI_THREAD = (1 << 0),
		SCENE_NO_THREAD = (1 << 1),
		SCENE_SUPPORT_GPU_PHYSX = (1 << 2),
		SCENE_RENDER_ENABLE_CUDA_INTEROP = (1 << 3),
		SCENE_RECORD_MEMORY_ALLOC = (1 << 4),
	};

};


struct WeSceneQueryFlag{
	enum Enum
	{
		QUERY_WORLD = 1, //trace world
		QUERY_OTHER_PLAYER, // trace other players, except myself
		QUERY_ALL_PLAYER, //trace all player, including all players

		QUERY_EXCEPT_SELF,  //QUERY_WORLD | QUERY_OTHER_PLAYER,
		QUERY_ALL,    //QUERY_WORLD | QUERY_ALL_PLAYER,
	};
};



typedef struct  WeQueryDesc_t
{
	float start[3];
	float end[3];
	float min[3];
	float max[3];
	float bound[3];
	int flag;
	int entityIndex;
}WeQueryDesc;

typedef struct WeQueryResult_t{
	int flag;
	float fraction;
	float end[3];
	float hitNormal[3];
	int	  surfaceFlag;
	int	  contents;
	int   entityNum;
	int   brushNum;
	char  boneName[32];

	WeQueryResult_t()
	{
		flag = 0;
		fraction = 0;
	}

} WeQueryResult;

typedef struct WeScnenDesc_t
{
	static const int MAX_FILE_LEN = 64;
	int		flag;
	float	gravity;
	float	staticFriction;
	float	dynamicFriction;
	float   restitution;

	float	bodyStaticFriction;
	float   bodyDynamicFriction;
	float	bodyRestitution;

#ifdef __CROSSBRIDGE__
	int				rawMeshLen; 
	unsigned char*  rawMesh; 

#else
	char	meshFile[MAX_FILE_LEN];
#endif
	WeScnenDesc_t()
	{
		flag = 0;
		gravity = 2034.0f;
		staticFriction = 0.5f;
		dynamicFriction = 0.5f;
		restitution = 0.1f;

		bodyStaticFriction = staticFriction;
		bodyDynamicFriction = bodyRestitution;
		bodyRestitution = restitution;
#ifdef __CROSSBRIDGE__
		rawMeshLen = 0;
		rawMesh = NULL;
#else
		meshFile[0] = '\0';
#endif

	};
} WeSceneDesc;



#endif