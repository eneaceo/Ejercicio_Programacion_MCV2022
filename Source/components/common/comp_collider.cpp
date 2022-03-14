#include "mcv_platform.h"
#include "comp_collider.h"
#include "modules/module_physics.h"
#include "engine.h"
#include "render/render.h"
#include "render/draw_primitives.h"

DECL_OBJ_MANAGER("collider", TCompCollider)

//void CObjectManager<TCompCollider>u::pdateAll(float dt) {
//	// do something special...
//}

using namespace physx;

TCompCollider::~TCompCollider()
{
	if (actor && CEngine::get().getPhysics().isActive())
	{
		if (controller)
		{
			controller->release();
			controller = nullptr;
			// No need to release the actor.
			actor = nullptr;
		}
		else
			actor->release(), actor = nullptr;
	}
}

void TCompCollider::debugInMenu()
{

	auto actor_type = actor->getType();
	if (actor_type == physx::PxActorTypeFlag::eRIGID_DYNAMIC)
	{
		ImGui::Text("Rigid Dynamic");
		PxRigidDynamic* rd = (PxRigidDynamic*)actor;
		if (rd->isSleeping())
		{
			ImGui::SameLine();
			ImGui::Text("Sleeping");
		}
	}
	else if (actor_type == physx::PxActorTypeFlag::eRIGID_STATIC)
		ImGui::Text("Rigid Static");

	if (controller)
	{
		float rad = controller->getRadius();
		if (ImGui::DragFloat("Controller Radius", &rad, 0.02f, 0.1f, 5.0f))
			controller->setRadius(rad);
		float height = controller->getHeight();
		if (ImGui::DragFloat("Controller Height", &height, 0.02f, 0.1f, 5.0f))
			controller->setHeight(height);
	}

	onEachShape(&TCompCollider::debugInMenuShape);
}

void TCompCollider::load(const json& j, TEntityParseContext& ctx)
{
	jconfig = j;
}

void TCompCollider::onEntityCreated()
{
	CEngine::get().getPhysics().createActor(*this);
}

// --------------------------------------------------------------------
void TCompCollider::debugInMenuShape(physx::PxShape* shape, physx::PxGeometryType::Enum geometry_type, const void* geom, MAT44 world)
{
	PxShapeFlags flags = shape->getFlags();
	if (flags & PxShapeFlag::eTRIGGER_SHAPE)
		ImGui::Text("Is trigger");

	switch (geometry_type)
	{
	case PxGeometryType::eSPHERE: {
		PxSphereGeometry* sphere = (PxSphereGeometry*)geom;
		ImGui::LabelText("Sphere Radius", "%f", sphere->radius);
		break;
	}
	case PxGeometryType::eBOX: {
		PxBoxGeometry* box = (PxBoxGeometry*)geom;
		ImGui::LabelText("Box", "Half:%f %f %f", box->halfExtents.x, box->halfExtents.y, box->halfExtents.z);
		break;
	}
	case PxGeometryType::ePLANE: {
		PxPlaneGeometry* plane = (PxPlaneGeometry*)geom;
		ImGui::Text("Plane");
		break;
	}
	case PxGeometryType::eCAPSULE: {
		PxCapsuleGeometry* capsule = (PxCapsuleGeometry*)geom;
		ImGui::LabelText("Capsule", "Rad:%f Height:%f", capsule->radius, capsule->halfHeight);
		break;
	}
	case PxGeometryType::eTRIANGLEMESH: {
		PxTriangleMeshGeometry* trimesh = (PxTriangleMeshGeometry*)geom;
		ImGui::LabelText("Tri mesh", "%d verts", trimesh->triangleMesh->getNbVertices());
		break;
	}
	case PxGeometryType::eCONVEXMESH: {
		PxConvexMesh* convex_mesh = (PxConvexMesh*)geom;
		ImGui::LabelText("Convex mesh", "%d verts", convex_mesh->getNbVertices());
		break;
	}
	}
}

// --------------------------------------------------------------------
void TCompCollider::renderDebugShape(physx::PxShape* shape, physx::PxGeometryType::Enum geometry_type, const void* geom, MAT44 world) {
	VEC4 color = Colors::Red;

	PxShapeFlags flags = shape->getFlags();
	if (flags & PxShapeFlag::eTRIGGER_SHAPE)
		color = VEC4(1, 1, 0, 1);

	switch (geometry_type) {
	case PxGeometryType::eSPHERE: {
		PxSphereGeometry* sphere = (PxSphereGeometry*)geom;
		drawWiredSphere(world, sphere->radius, color);
		break;
	}
	case PxGeometryType::eBOX: {
		PxBoxGeometry* box = (PxBoxGeometry*)geom;
		AABB aabb;
		aabb.Extents = PXVEC3_TO_VEC3(box->halfExtents);
		drawWiredAABB(aabb, world, color);
		break;
	}
	case PxGeometryType::ePLANE: {
		PxBoxGeometry* box = (PxBoxGeometry*)geom;
		const CMesh* grid = Resources.get("grid.mesh")->as<CMesh>();
		// To generate a PxPlane from a PxTransform, transform PxPlane(1, 0, 0, 0).
		// Our plane is 0,1,0
		MAT44 Z2Y = MAT44::CreateRotationZ((float)-M_PI_2);
		drawPrimitive(grid, Z2Y * world, VEC4(1, 1, 1, 1));
		break;
	}
	case PxGeometryType::eCAPSULE: {
		PxCapsuleGeometry* capsule = (PxCapsuleGeometry*)geom;
		// world indicates the foot position
		world = world * MAT44::CreateTranslation(0.f, capsule->halfHeight + capsule->radius, 0.f);
		drawWiredSphere(world, capsule->radius, color);
		world = world * MAT44::CreateTranslation(0.f, capsule->halfHeight- capsule->radius, 0.f);
		drawWiredSphere(world, capsule->radius, color);
		break;
	}
	case PxGeometryType::eTRIANGLEMESH: {
		const CMesh* debug_mesh = (const CMesh*)shape->userData;
		assert(debug_mesh);
		drawPrimitive(debug_mesh, world, color);
		break;
	}
	}
}

// ---------------------------------------------------------------------------
void TCompCollider::renderDebug()
{
	onEachShape(&TCompCollider::renderDebugShape);
}

// --------------------------------------------------------------------
// Iterates over all shapes of the actor, calling the given callback
// resolves the invalid cases, and gives the world matrix including the
// shape offset.
void TCompCollider::onEachShape(TShapeFn fn)
{

	// Use the phyics absolute transform to display the physics shapes in debug.
	assert(actor);
	MAT44 actor_transform = toTransform(actor->getGlobalPose()).asMatrix();

	PxU32 nshapes = actor->getNbShapes();

	static const PxU32 max_shapes = 8;
	PxShape* shapes[max_shapes];
	assert(nshapes <= max_shapes);

	// Even when the buffer is small, it writes all the shape pointers
	PxU32 shapes_read = actor->getShapes(shapes, sizeof(shapes), 0);

	// An actor can have several shapes
	for (PxU32 i = 0; i < nshapes; ++i)
	{
		PxShape* shape = shapes[i];
		assert(shape);

		// Combine physics local offset with world transform of the entity
		MAT44 world = toTransform(shape->getLocalPose()).asMatrix() * actor_transform;

		switch (shape->getGeometryType())
		{

		case PxGeometryType::eSPHERE: {
			PxSphereGeometry sphere;
			if (shape->getSphereGeometry(sphere))
				(this->*fn)(shape, PxGeometryType::eSPHERE, &sphere, world);
			break;
		}

		case PxGeometryType::eBOX: {
			PxBoxGeometry box;
			if (shape->getBoxGeometry(box))
				(this->*fn)(shape, PxGeometryType::eBOX, &box, world);
			break;
		}

		case PxGeometryType::ePLANE: {
			PxPlaneGeometry plane;
			if (shape->getPlaneGeometry(plane))
				(this->*fn)(shape, PxGeometryType::ePLANE, &plane, world);
			break;
		}

		case PxGeometryType::eCAPSULE: {
			PxCapsuleGeometry capsule;
			if (shape->getCapsuleGeometry(capsule))
				(this->*fn)(shape, PxGeometryType::eCAPSULE, &capsule, world);
			break;
		}
		case PxGeometryType::eCONVEXMESH: {
			PxConvexMeshGeometry convexmesh;
			if (shape->getConvexMeshGeometry(convexmesh))
				(this->*fn)(shape, PxGeometryType::eCONVEXMESH, &convexmesh, world);
			break;
		}

		case PxGeometryType::eTRIANGLEMESH: {
			PxTriangleMeshGeometry trimesh;
			if (shape->getTriangleMeshGeometry(trimesh))
				(this->*fn)(shape, PxGeometryType::eTRIANGLEMESH, &trimesh, world);
			break;
		}
	}
	}
}
