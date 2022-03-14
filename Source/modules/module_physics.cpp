#include "mcv_platform.h"
#include "module_physics.h"
#include "components/common/comp_transform.h"
#include "skeleton/comp_skeleton.h"
#include "skeleton/game_core_skeleton.h"
#include "skeleton/cal3d2engine.h"

#include "PxPhysicsAPI.h"
#include "render/meshes/mesh_io.h"
#include "render/draw_primitives.h"

#pragma comment(lib,"PhysX3_x64.lib")
#pragma comment(lib,"PhysX3Common_x64.lib")
#pragma comment(lib,"PhysX3Extensions.lib")
#pragma comment(lib,"PxFoundation_x64.lib")
#pragma comment(lib,"PxPvdSDK_x64.lib")
#pragma comment(lib,"PhysX3CharacterKinematic_x64.lib")
#pragma comment(lib,"PhysX3Cooking_x64.lib")

using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

PxControllerManager* gControllerManager = nullptr;

// -----------------------------------------------------
CTransform toTransform(PxTransform pxtrans) {
	CTransform trans;
	trans.setPosition(PXVEC3_TO_VEC3(pxtrans.p));
	trans.setRotation(PXQUAT_TO_QUAT(pxtrans.q));
	return trans;
}

// -----------------------------------------------------
PxTransform toPxTransform(CTransform mcvtrans) {
	PxTransform trans;
	trans.p = VEC3_TO_PXVEC3(mcvtrans.getPosition());
	trans.q = QUAT_TO_PXQUAT(mcvtrans.getRotation());
	return trans;
}

CModulePhysics::FilterGroup CModulePhysics::getFilterByName(const std::string& name)
{
	if (strcmp("player", name.c_str()) == 0) {
		return CModulePhysics::FilterGroup::Player;
	}
	else if (strcmp("enemy", name.c_str()) == 0) {
		return CModulePhysics::FilterGroup::Enemy;
	}
	else if (strcmp("projectile", name.c_str()) == 0) {
		return CModulePhysics::FilterGroup::Projectile;
	}
	else if (strcmp("characters", name.c_str()) == 0) {
		return CModulePhysics::FilterGroup::Characters;
	}
	else if (strcmp("wall", name.c_str()) == 0) {
		return CModulePhysics::FilterGroup::Wall;
	}
	else if (strcmp("floor", name.c_str()) == 0) {
		return CModulePhysics::FilterGroup::Floor;
	}
	else if (strcmp("scenario", name.c_str()) == 0) {
		return CModulePhysics::FilterGroup::Scenario;
	}
	return CModulePhysics::FilterGroup::All;
}

void CModulePhysics::setupFiltering(PxShape* shape, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;	// word1 = ID mask to filter pairs that trigger a contact callback;
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);
}

void CModulePhysics::setupFilteringOnAllShapesOfActor(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	assert(filterGroup != FilterGroup::All);
	const PxU32 numShapes = actor->getNbShapes();
	std::vector<PxShape*> shapes;
	shapes.resize(numShapes);
	actor->getShapes(&shapes[0], numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
		setupFiltering(shapes[i], filterGroup, filterMask);
}

// -----------------------------------------------------
static PxGeometryType::Enum readGeometryType(const json& j) {
	PxGeometryType::Enum geometryType = PxGeometryType::eINVALID;
	if (!j.count("shape"))
		return geometryType;

	std::string jgeometryType = j["shape"];
	if (jgeometryType == "sphere") {
		geometryType = PxGeometryType::eSPHERE;
	}
	else if (jgeometryType == "box") {
		geometryType = PxGeometryType::eBOX;
	}
	else if (jgeometryType == "plane") {
		geometryType = PxGeometryType::ePLANE;
	}
	else if (jgeometryType == "convex") {
		geometryType = PxGeometryType::eCONVEXMESH;
	}
	else if (jgeometryType == "capsule") {
		geometryType = PxGeometryType::eCAPSULE;
	}
	else if (jgeometryType == "trimesh") {
		geometryType = PxGeometryType::eTRIANGLEMESH;
	}
	else {
		dbg("Unsupported shape type in collider component %s", jgeometryType.c_str());
	}
	return geometryType;
}


bool CModulePhysics::readShape(PxRigidActor* actor, const json& jcfg)
{
	// Shapes....
	PxGeometryType::Enum geometryType = readGeometryType(jcfg);
	if (geometryType == PxGeometryType::eINVALID)
		return false;

	PxShape* shape = nullptr;
	if (geometryType == PxGeometryType::eBOX)
	{
		VEC3 jhalfExtent = loadVEC3(jcfg, "half_size");
		PxVec3 halfExtent = VEC3_TO_PXVEC3(jhalfExtent);
		shape = gPhysics->createShape(PxBoxGeometry(halfExtent), *gMaterial);
	}
	else if (geometryType == PxGeometryType::ePLANE)
	{
		VEC3 jplaneNormal = loadVEC3(jcfg, "normal");
		float jplaneDistance = jcfg.value("distance", 0.f);
		PxVec3 planeNormal = VEC3_TO_PXVEC3(jplaneNormal);
		PxReal planeDistance = jplaneDistance;
		PxPlane plane(planeNormal, planeDistance);
		PxTransform offset = PxTransformFromPlaneEquation(plane);
		shape = gPhysics->createShape(PxPlaneGeometry(), *gMaterial);
		shape->setLocalPose(offset);
	}
	else if (geometryType == PxGeometryType::eSPHERE)
	{
		PxReal radius = jcfg.value("radius", 1.f);;
		shape = gPhysics->createShape(PxSphereGeometry(radius), *gMaterial);
	}
	else if (geometryType == PxGeometryType::eCAPSULE)
	{
		PxReal radius = jcfg.value("radius", 1.f);
		PxReal half_height = jcfg.value("height", 1.f) * 0.5f;
		shape = gPhysics->createShape(PxCapsuleGeometry(radius, half_height), *gMaterial);
	}
	else if (geometryType == PxGeometryType::eCONVEXMESH)
	{
		std::string col_mesh_name = jcfg.value("collision_mesh", "");
		TMeshIO mesh_io;
		CMemoryDataProvider mdp(col_mesh_name.c_str());
		bool is_ok = mesh_io.read(mdp);
		assert(is_ok);

		PxU8* cooked_data = nullptr;
		PxU32 cooked_size = 0;

		// https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/Geometry.html

		// Do we have a physics cooked data in the file???
		if (mesh_io.collision_cooked.empty()) {

			PxConvexMeshDesc meshDesc = {};
			meshDesc.points.count = mesh_io.header.num_vertices;
			meshDesc.points.data = mesh_io.vertices.data();
			meshDesc.points.stride = sizeof(PxVec3);

			meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION | PxConvexFlag::eFAST_INERTIA_COMPUTATION;
			/*
			// Physics: compute the convex for me
			meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

			//meshDesc.indices.count = mesh_io.header.num_indices;
			//meshDesc.indices.stride = mesh_io.header.bytes_per_index;
			//meshDesc.indices.data = mesh_io.indices.data();

			//if (mesh_io.header.bytes_per_index == 2)
			//	meshDesc.flags |= PxConvexFlag::e16_BIT_INDICES;

			//// My Mesh is a indexed triangular mesh
			meshDesc.polygons.count = mesh_io.header.num_indices / 3;
			meshDesc.polygons.stride = 3 * mesh_io.header.bytes_per_index;
			meshDesc.polygons.data = mesh_io.indices.data();
			if (mesh_io.header.bytes_per_index == 2)
				meshDesc.flags |= PxConvexFlag::e16_BIT_INDICES;
				*/

						// Find a physics obj to create the cooking
			PxTolerancesScale scale;
			PxCooking* cooking = PxCreateCooking(PX_PHYSICS_VERSION, gPhysics->getFoundation(), PxCookingParams(scale));

			//bool is_valid = cooking->validateConvexMesh(meshDesc);
			//assert(is_valid);

			// Place to store the results of the cooking
			PxDefaultMemoryOutputStream writeBuffer;
			PxConvexMeshCookingResult::Enum result;
			bool status = cooking->cookConvexMesh(meshDesc, writeBuffer, &result);
			assert(status);

			// Num bytes of the resulting cooking process
			cooked_size = writeBuffer.getSize();
			cooked_data = writeBuffer.getData();

			// Transfer the cooked mesh data into the mesh_io, so it's saved and the next time is used
			mesh_io.collision_cooked.resize(cooked_size);
			memcpy(mesh_io.collision_cooked.data(), cooked_data, cooked_size);

			// Regenerate the cmesh so it contains the raw collision mesh + cooked data
			CFileDataSaver fds(col_mesh_name.c_str());
			assert(fds.isValid());
			mesh_io.write(fds);
		}

		// the cooked data comes after the raw collision mesh
		cooked_size = (uint32_t)mesh_io.collision_cooked.size();
		cooked_data = mesh_io.collision_cooked.data();

		// 
		assert(cooked_data != nullptr);
		assert(cooked_size > 0);

		PxDefaultMemoryInputData readBuffer(cooked_data, cooked_size);
		PxConvexMesh* convex_mesh = gPhysics->createConvexMesh(readBuffer);
		assert(convex_mesh);

		shape = gPhysics->createShape(PxConvexMeshGeometry(convex_mesh), *gMaterial);

		// Now create a render mesh that we will use for debug
		CMesh* render_mesh = new CMesh();
		is_ok = render_mesh->create(mesh_io);
		assert(is_ok);
		char res_name[64];
		sprintf(res_name, "Convex_%p", render_mesh);
		Resources.registerResource(render_mesh, res_name, getClassResourceType<CMesh>());

		shape->userData = render_mesh;
	}
	else if (geometryType == PxGeometryType::eTRIANGLEMESH)
	{
		std::string col_mesh_name = jcfg.value("collision_mesh", "");
		
		TMeshIO mesh_io;
		CMemoryDataProvider mdp(col_mesh_name.c_str());
		bool is_ok = mesh_io.read(mdp);
		assert(is_ok);

		PxU8* cooked_data = nullptr;
		PxU32 cooked_size = 0;

		// Do we have a physics cooked data in the file???
		if (mesh_io.collision_cooked.empty()) {
			
			// No, so we need to cook it
			assert(mesh_io.header.bytes_per_vertex == sizeof(PxVec3));

			PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = mesh_io.header.num_vertices;
			meshDesc.points.data = mesh_io.vertices.data();
			meshDesc.points.stride = sizeof(PxVec3);

			// Physics is using a ccw/cw convention
			meshDesc.flags |= PxMeshFlag::eFLIPNORMALS;

			// My Mesh is a indexed triangular mesh
			meshDesc.triangles.count = mesh_io.header.num_indices / 3;
			meshDesc.triangles.stride = 3 * mesh_io.header.bytes_per_index;
			meshDesc.triangles.data = mesh_io.indices.data();
			if (mesh_io.header.bytes_per_index == 2)
				meshDesc.flags |= PxMeshFlag::e16_BIT_INDICES;

			// Find a physics obj to create the cooking
			PxTolerancesScale scale;
			PxCooking* cooking = PxCreateCooking(PX_PHYSICS_VERSION, gPhysics->getFoundation(), PxCookingParams(scale));

			bool is_valid = cooking->validateTriangleMesh(meshDesc);
			assert(is_valid);

			// Place to store the results of the cooking
			PxDefaultMemoryOutputStream writeBuffer;
			PxTriangleMeshCookingResult::Enum result;
			bool status = cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
			assert(status);

			// Num bytes of the resulting cooking process
			cooked_size = writeBuffer.getSize();
			cooked_data = writeBuffer.getData();

			// Transfer the cooked mesh data into the mesh_io, so it's saved and the next time is used
			mesh_io.collision_cooked.resize(cooked_size);
			memcpy(mesh_io.collision_cooked.data(), cooked_data, cooked_size);

			// Regenerate the cmesh so it contains the raw collision mesh + cooked data
			CFileDataSaver fds(col_mesh_name.c_str());
			assert(fds.isValid());
			mesh_io.write(fds);
		}

		// the cooked data comes after the raw collision mesh
		cooked_size = (uint32_t)mesh_io.collision_cooked.size();
		cooked_data = mesh_io.collision_cooked.data();

		// 
		assert(cooked_data != nullptr);
		assert(cooked_size > 0);

		PxDefaultMemoryInputData readBuffer(cooked_data, cooked_size);
		PxTriangleMesh* tri_mesh = gPhysics->createTriangleMesh(readBuffer);
		assert(tri_mesh);

		shape = gPhysics->createShape(PxTriangleMeshGeometry(tri_mesh), *gMaterial);

		// Now create a render mesh that we will use for debug
		CMesh* render_mesh = new CMesh();
		is_ok = render_mesh->create(mesh_io);
		assert(is_ok);
		char res_name[64];
		sprintf(res_name, "Physics_%p", render_mesh);
		Resources.registerResource(render_mesh, res_name, getClassResourceType<CMesh>());

		shape->userData = render_mesh;
	}
	else
	{
		fatal("not implemented yet");
	}

	if (jcfg.value("trigger", false))
	{
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}

	// Shape offset
	if (jcfg.count("offset")) {
		CTransform trans;
		trans.fromJson(jcfg["offset"]);
		shape->setLocalPose(toPxTransform(trans));
	}

	setupFiltering(
		shape,
		getFilterByName(jcfg.value("group", "scenario")),
		getFilterByName(jcfg.value("mask", "all"))
	);

	actor->attachShape(*shape);
	shape->release();
	return true;
}

PxRigidActor* CModulePhysics::createController(TCompCollider& comp_collider) 
{
	const json& jconfig = comp_collider.jconfig;

	PX_ASSERT(desc.mType == PxControllerShapeType::eCAPSULE);

	PxCapsuleControllerDesc capsuleDesc;
	capsuleDesc.height = jconfig.value("height", 1.f);
	capsuleDesc.radius = jconfig.value("radius", 1.f);
	capsuleDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	capsuleDesc.material = gMaterial;

	PxCapsuleController * ctrl = static_cast<PxCapsuleController*>(gControllerManager->createController(capsuleDesc));
	PX_ASSERT(ctrl);
	TCompTransform * c = comp_collider.get<TCompTransform>();
	VEC3 pos = c->getPosition();
	ctrl->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
	PxRigidActor * actor = ctrl->getActor();
	comp_collider.controller = ctrl;
	comp_collider.actor = actor;


	setupFilteringOnAllShapesOfActor(actor,
		getFilterByName(jconfig.value("group", "characters")),
		getFilterByName(jconfig.value("mask", "all"))
	);
	return actor;
}

void CModulePhysics::createRagdoll(TCompRagdoll& comp_ragdoll) {
	if (comp_ragdoll.ragdoll.created)
		return;
	CHandle h_comp_ragdoll(&comp_ragdoll);
	CEntity* e = h_comp_ragdoll.getOwner();

	TCompSkeleton* skel = e->get<TCompSkeleton>();
	TCompTransform* comp_transform = e->get<TCompTransform>();
	CTransform* entity_trans = (CTransform*)comp_transform;

	auto core_skel = (CGameCoreSkeleton*)skel->model->getCoreModel();

	for (auto& ragdoll_bone_core : core_skel->ragdoll_core.ragdoll_bone_cores) {
		auto cal_core_bone = core_skel->getCoreSkeleton()->getCoreBone(ragdoll_bone_core.bone);
		assert(cal_core_bone);
		CTransform trans;
		trans.setPosition(Cal2DX(cal_core_bone->getTranslationAbsolute()));
		trans.setRotation(Cal2DX(cal_core_bone->getRotationAbsolute()));

		//trans.setPosition(trans.getPosition() + trans.getLeft() * ragdoll_bone_core.height * 0.5f);

		PxTransform px_entity_transform = toPxTransform(*entity_trans);
		PxTransform px_transform = toPxTransform(trans);
		px_transform = px_entity_transform * px_transform;

		PxRigidActor* actor = nullptr;
		PxRigidDynamic* dynamicActor = gPhysics->createRigidDynamic(px_transform);
		PxRigidBodyExt::updateMassAndInertia(*dynamicActor, 1.f);
		actor = dynamicActor;

		PxShape* shape = gPhysics->createShape(PxCapsuleGeometry(ragdoll_bone_core.radius, ragdoll_bone_core.height * 0.5f), *gMaterial, true);
		setupFiltering(shape, CModulePhysics::FilterGroup::All, CModulePhysics::FilterGroup::All);
		CTransform offset;
		offset.setPosition(VEC3(ragdoll_bone_core.height * 0.5f, 0.f, 0.f));
		QUAT rot;
		rot.CreateFromYawPitchRoll(0, 90.f, 0);
		offset.setRotation(rot);
		PxTransform pxTranform(ragdoll_bone_core.height * 0.5f, 0.f, 0.f);
		//shape->setLocalPose(toPxTransform(offset));
		//shape->setLocalPose(pxTranform);

		actor->attachShape(*shape);
		shape->release();

		PxRigidDynamic* body = (PxRigidDynamic*)actor;
		assert(body);

		TRagdoll::TRagdollBone& ragdoll_bone = comp_ragdoll.ragdoll.bones[comp_ragdoll.ragdoll.num_bones];
		ragdoll_bone.actor = body;
		ragdoll_bone.core = &ragdoll_bone_core;

		ragdoll_bone.idx = core_skel->getCoreSkeleton()->getCoreBoneId(ragdoll_bone_core.bone);
		ragdoll_bone.core->instance_idx = comp_ragdoll.ragdoll.num_bones;
		++comp_ragdoll.ragdoll.num_bones;
	}

	for (int i = 0; i < comp_ragdoll.ragdoll.num_bones; ++i) {
		auto& ragdoll_bone = comp_ragdoll.ragdoll.bones[i];
		if (ragdoll_bone.core->parent_core) {
			ragdoll_bone.parent_idx = ragdoll_bone.core->parent_core->instance_idx;
			auto& parent_ragdoll_bone = comp_ragdoll.ragdoll.bones[ragdoll_bone.parent_idx];

			parent_ragdoll_bone.children_idxs[parent_ragdoll_bone.num_children] = i;
			++parent_ragdoll_bone.num_children;
		}
	}

	createRagdollJoints(comp_ragdoll, 0);

	comp_ragdoll.ragdoll.created = true;

}

void CModulePhysics::createRagdollJoints(TCompRagdoll& comp_ragdoll, int bone_id) {
	TRagdoll::TRagdollBone& ragdoll_bone = comp_ragdoll.ragdoll.bones[bone_id];

	for (int i = 0; i < ragdoll_bone.num_children; ++i) {
		auto child_id = ragdoll_bone.children_idxs[i];
		TRagdoll::TRagdollBone& child_ragdoll_bone = comp_ragdoll.ragdoll.bones[child_id];

		PxJoint* joint = nullptr;

		PxVec3 offset(0.1f, 0.f, 0.f);

		assert(child_ragdoll_bone.actor);
		assert(ragdoll_bone.actor);
		auto d1 = child_ragdoll_bone.actor->getGlobalPose().q.rotate(PxVec3(1, 0, 0));
		auto d2 = ragdoll_bone.actor->getGlobalPose().q.rotate(PxVec3(1, 0, 0));
		auto axis = d1.cross(d2).getNormalized();
		auto pos = ragdoll_bone.actor->getGlobalPose().p;
		auto diff = (pos - child_ragdoll_bone.actor->getGlobalPose().p).getNormalized();
		if (diff.dot(d2) < 0) d2 = -d2;

		PxShape * shape;
		if (ragdoll_bone.actor->getShapes(&shape, 1) == 1)
		{
			PxCapsuleGeometry capsule;
			if (shape->getCapsuleGeometry(capsule))
			{
				pos -= (capsule.halfHeight + capsule.radius) * d2;
			}
		}

		PxTransform tr0 = PxTransform(PxVec3(0.f, 0.f, 0.f));
		PxMat44 mat(d1, axis, d1.cross(axis).getNormalized(), pos);
		PxTransform pxTrans(mat);
		if (pxTrans.isSane())
		{
			PxTransform ragdoll_bone_actor_trans = ragdoll_bone.actor->getGlobalPose();
			if (ragdoll_bone_actor_trans.isSane())
			{
				PxTransform ragdoll_bone_actor_trans_inv = ragdoll_bone_actor_trans.getInverse();
				if (ragdoll_bone_actor_trans_inv.isSane())
				{
					tr0 = ragdoll_bone_actor_trans_inv * pxTrans;
				}
			}
		}

		PxTransform tr1 = child_ragdoll_bone.actor->getGlobalPose().getInverse() * ragdoll_bone.actor->getGlobalPose() * tr0;
		if (!tr1.isValid()) {
			tr1 = PxTransform(PxVec3(0.f, 0.f, 0.f));
		}
		/*
		//-----
		//PxSphericalJoint joint
		joint = PxSphericalJointCreate(gScene->getPhysics(), ragdoll_bone.actor, tr0, child_ragdoll_bone.actor, tr1);
		assert(joint);
		if (joint)
		{
		  auto* spherical = static_cast<PxSphericalJoint*>(joint);
		  spherical->setProjectionLinearTolerance(0.1f);
		  spherical->setLimitCone(physx::PxJointLimitCone(0.01f, 0.f, 0.01f));
		  spherical->setSphericalJointFlag(physx::PxSphericalJointFlag::eLIMIT_ENABLED, true);
		}
		*/

		/*

		//PxRevoluteJoint joint
		joint =
		PxRevoluteJointCreate(mScene->getPhysics(), ragdoll_bone.actor, tr0, child_ragdoll_bone.actor, tr1);
		if (joint)
		{
		auto* hinge = static_cast<PxRevoluteJoint*>(joint);
		hinge->setProjectionLinearTolerance(0.1f);
		hinge->setProjectionAngularTolerance(0.01f);
		hinge->setLimit(physx::PxJointAngularLimitPair(-PxPi / 4, PxPi / 4, 0.01f));
		hinge->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		}
		*/

		//fixed joint
		joint = PxFixedJointCreate(gScene->getPhysics(), ragdoll_bone.actor, tr0, child_ragdoll_bone.actor, tr1);
		assert(joint);
		if (joint)
		{
			auto* fixed_joint = static_cast<PxFixedJoint*>(joint);
			fixed_joint->setProjectionLinearTolerance(0.1f);
			fixed_joint->setProjectionAngularTolerance(0.01f);
		}

		if (joint)
		{
			joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
			joint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);
			joint->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
		}
		child_ragdoll_bone.parent_joint = joint;

		createRagdollJoints(comp_ragdoll, child_id);
	}

}

void CModulePhysics::createActor(TCompCollider& comp_collider)
{
	const json& jconfig = comp_collider.jconfig;

	TCompTransform* c = comp_collider.get<TCompTransform>();
	PxTransform transform = toPxTransform(*c);

	PxRigidActor* actor = nullptr;
	
	bool is_controller = jconfig.count("controller") > 0;

	// Controller or physics based?
	if (is_controller) 
	{
		actor = createController(comp_collider);
	}
	else
	{
		// Dynamic or static actor?
		bool isDynamic = jconfig.value("dynamic", false);
		if (isDynamic)
		{
			PxRigidDynamic* dynamicActor = gPhysics->createRigidDynamic(transform);
			PxReal density = jconfig.value("density", 1.f);
			PxRigidBodyExt::updateMassAndInertia(*dynamicActor, density);
			actor = dynamicActor;

			if (jconfig.value("kinematic", false))
				dynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		}
		else
		{
			PxRigidStatic* static_actor = gPhysics->createRigidStatic(transform);
			actor = static_actor;
		}
	}

	// The capsule was already loaded as part of the controller
	if (!is_controller) {
		// if shapes exists, try to read as an array of shapes
		if (jconfig.count("shapes")) {
			const json& jshapes = jconfig["shapes"];
			for (const json& jshape : jshapes)
				readShape(actor, jshape);
		}
		// Try to read shape directly (general case...)
		else {
			readShape(actor, jconfig);
		}
	}

	gScene->addActor(*actor);

	comp_collider.actor = actor;
	CHandle h_collider(&comp_collider);
	comp_collider.actor->userData = h_collider.asVoidPtr();
}

// ------------------------------------------------------------------
PxFilterFlags CustomFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize
)
{
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		}
		else {
			pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;
		}
		return PxFilterFlag::eDEFAULT;
	}
	return PxFilterFlag::eSUPPRESS;
}

bool CModulePhysics::start()
{
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
	if (!PxInitExtensions(*gPhysics, gPvd))
		fatal("PxInitExtensions failed!");

	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = CustomFilterShader;
	sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS | PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS | PxSceneFlag::eENABLE_KINEMATIC_PAIRS;
	gScene = gPhysics->createScene(sceneDesc);

	// Register to the callbacks
	gScene->setSimulationEventCallback(&customSimulationEventCallback);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		//pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		//pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		//pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	gControllerManager = PxCreateControllerManager(*gScene);


	//to enable render debug
	//gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	//gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	return true;
}

void CModulePhysics::stop() {
	gScene->release();
	gDispatcher->release();
	gPhysics->release();
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();

	gFoundation->release();
}

void CModulePhysics::update(float delta) {	
	gScene->simulate(delta);
	gScene->fetchResults(true);

	PxU32 nbActorsOut = 0;
	PxActor** activeActors = gScene->getActiveActors(nbActorsOut);
	for (unsigned int i = 0; i < nbActorsOut; ++i)
	{
		PxRigidActor* rigidActor = ((PxRigidActor*)activeActors[i]);
		assert(rigidActor);
		CHandle h_collider;
		h_collider.fromVoidPtr(rigidActor->userData);
		TCompCollider* c_collider = h_collider;
		if (c_collider != nullptr)
		{
			TCompTransform* c = c_collider->get<TCompTransform>();
			assert(c);
			if (c_collider->controller) 
			{
				PxExtendedVec3 pxpos_ext = c_collider->controller->getFootPosition();
				c->setPosition(VEC3((float)pxpos_ext.x, (float)pxpos_ext.y, (float)pxpos_ext.z));
			}
			else
			{
				// Do not loose the original scale of the transform component
				VEC3 c_scale = c->getScale();
				PxTransform PxTrans = rigidActor->getGlobalPose();
				CTransform MCVTrans = toTransform(PxTrans);
				MCVTrans.setScale(c_scale);
				c->set(MCVTrans);
			}
		}

	}
}

void CModulePhysics::renderInMenu() {
}

void CModulePhysics::renderDebug()
{
	const PxRenderBuffer& renderBuffer = gScene->getRenderBuffer();
	VEC4 color = VEC4(1, 0, 0, 1);

	for (PxU32 i = 0; i < renderBuffer.getNbLines(); i++)
	{
		const PxDebugLine& line = renderBuffer.getLines()[i];
		// render the line
		drawLine(PXVEC3_TO_VEC3(line.pos0), PXVEC3_TO_VEC3(line.pos1), color);
	}
}

void CModulePhysics::render() {
}

PxQueryHitType::Enum CModulePhysics::CustomQueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	const physx::PxFilterData& filterData1 = shape->getQueryFilterData();

	if ((filterData.word0 & filterData1.word1) && (filterData1.word0 & filterData.word1))
	{
		return PxQueryHitType::eBLOCK;
	}
	return PxQueryHitType::eNONE;
}


void CModulePhysics::CustomSimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (PxContactPairFlag::eREMOVED_SHAPE_0 | PxContactPairFlag::eREMOVED_SHAPE_1))
				continue;


			CHandle h_comp;
			h_comp.fromVoidPtr(pairHeader.actors[0]->userData);

			CHandle h_comp_other;
			h_comp_other.fromVoidPtr(pairHeader.actors[1]->userData);

			CEntity * e = h_comp.getOwner();
			CEntity * e_other = h_comp_other.getOwner();

			if (e && e_other)
			{
				TMsgEntityOnContact msg;

				// Notify the trigger someone entered
				msg.h_entity = h_comp_other.getOwner();
				e->sendMsg(msg);

				// Notify that someone he entered in a trigger
				msg.h_entity = h_comp.getOwner();
				e_other->sendMsg(msg);
			}
		}
	}
}

void CModulePhysics::CustomSimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		CHandle h_comp_trigger;
		h_comp_trigger.fromVoidPtr(pairs[i].triggerActor->userData);

		CHandle h_comp_other;
		h_comp_other.fromVoidPtr(pairs[i].otherActor->userData);

		CEntity * e_trigger = h_comp_trigger.getOwner();
		CEntity * e_other = h_comp_other.getOwner();

		if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			TMsgEntityTriggerEnter msg;

			// Notify the trigger someone entered
			msg.h_entity = h_comp_other.getOwner();
			e_trigger->sendMsg(msg);
		}
		else if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			TMsgEntityTriggerExit msg;

			// Notify the trigger someone exit
			msg.h_entity = h_comp_other.getOwner();
			e_trigger->sendMsg(msg);
		}
	}
}
