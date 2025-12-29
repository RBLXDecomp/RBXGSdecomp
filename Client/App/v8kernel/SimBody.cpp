#include "v8kernel/SimBody.h"
#include "v8kernel/Constants.h"
#include "v8kernel/Body.h"
#include "util/Units.h"
namespace RBX
{

SimBody::SimBody(RBX::Body* _body)
			:body(_body),
			dirty(1),
			force(G3D::Vector3::zero()),
			torque(G3D::Vector3::zero()),
			constantForceY(0.0f)
{}

SimBody::~SimBody() {}

G3D::Vector3 vecUnkPercent(G3D::Vector3& input)
{
	return G3D::Vector3(1.0f / input.x, 1.0f / input.y, 1.0f / input.z);
}

float precentInline(float fNum)
{
	return 1.0f / fNum;
}

__forceinline void SimBody::unkSimInline(float fNum)
{
	massRecip = 1.0f / fNum;
	momentRecip = vecUnkPercent(Math::toDiagonal(body->getBranchIBody()));
}

void SimBody::update()
{
	RBXASSERT(dirty);
	{
		const G3D::Vector3 cofmOffset = body->getCofmOffset();
		pv = body->getPV().pvAtLocalOffset(cofmOffset);
	}
	qOrientation = Quaternion::Quaternion(pv.position.rotation);
	qOrientation *= precentInline(qOrientation.magnitude());
	angMomentum = pv.velocity.rotational * body->getBranchIWorld();
	float _mass = body->getBranchMass();
	unkSimInline(_mass);
	constantForceY = body->getBranchMass() * Units::kmsAccelerationToRbx(Constants::getKmsGravity()).y;
	dirty = false;
}

void mulMatrixMatrixTranspose(const G3D::Matrix3& _mat0, const G3D::Matrix3& _mat1, G3D::Matrix3& _answer)
{
	//just like matrixMulInline, these pointers are required for matching
	const float* matrix0 = &_mat0[0][0];
	const float* matrix1 = &_mat1[0][0];
	float *answer = &_answer[0][0];

	//this part is mostly copied from operator* from matrix3 and transpose function as reference
	for (int iRow = 0; iRow < 3; iRow++)
	{
		for (int iCol = 0; iCol < 3; iCol++)
		{
            answer[iRow*3+iCol] =
                matrix0[iRow*3+0] * matrix1[iCol*3+0] +
                matrix0[iRow*3+1] * matrix1[iCol*3+1] +
                matrix0[iRow*3+2] * matrix1[iCol*3+2];
		}
	}
}

//this is cursed but its the only way i got it to match
//this might be actually how roblox wrote it because pointers are used in mulMatrixMatrixTranspose too
void matrixMulInline(const G3D::Matrix3& _mat, const G3D::Vector3& _vec, G3D::Matrix3& _answer)
{
	const float* matrix = &_mat[0][0];
	float* answer = _answer[0];
	answer[0] = matrix[0]*_vec.x;
	answer[1] = matrix[1]*_vec.y;
	answer[2] = matrix[2]*_vec.z;
	answer[3] = matrix[3]*_vec.x;
	answer[4] = matrix[4]*_vec.y;
	answer[5] = matrix[5]*_vec.z;
	answer[6] = matrix[6]*_vec.x;
	answer[7] = matrix[7]*_vec.y;
	answer[8] = matrix[8]*_vec.z;
}


G3D::Vector3 computeRotVel(const G3D::Matrix3& rot, const G3D::Vector3& momentRecip, const G3D::Vector3& angMomentum)
{
	G3D::Matrix3 temp;
	G3D::Matrix3 iWorldInv;
	matrixMulInline(rot, momentRecip, temp);
	mulMatrixMatrixTranspose(temp, rot, iWorldInv);
	return iWorldInv * angMomentum;
}

//temporary for now?
G3D::Vector3& denormFixFunc()
{
	static G3D::Vector3 denormFix = G3D::Vector3(9.9999997e-21f, 9.9999997e-21f, 9.9999997e-21f);
	return denormFix;
}

void SimBody::step(float dt)
{
	//line 103
	//static G3D::Vector3 denormFix = Vector3(9.9999997e-21f, 9.9999997e-21f, 9.9999997e-21f);
	G3D::Vector3& denormFix = denormFixFunc();
	//line 109
	updateIfDirty();
	//line 115?
	float someConstant = 0.99980003f;
	//line 117
	angMomentum = torque * dt + angMomentum * someConstant;
	pv.velocity.rotational = computeRotVel(getPV().position.rotation, momentRecip, angMomentum) + denormFix;
	//line 118
	Quaternion& rotateQuat = Quaternion::Quaternion(getPV().velocity.rotational, 0) * qOrientation * 0.5 * dt;
	//line 119
	qOrientation += rotateQuat;
	// line 121
	qOrientation *= precentInline(qOrientation.magnitude());
	qOrientation.toRotationMatrix(pv.position.rotation);
	//line 123
	pv.velocity.linear += force * massRecip * dt;
	pv.position.translation += getPV().velocity.linear * dt;
	//line 129
	force = G3D::Vector3(0, constantForceY, 0);
	torque = G3D::Vector3(0, 0, 0);
	//line 131
	angMomentum += denormFix;
	//line 132
	pv.velocity.linear += denormFix;

	
}

PV SimBody::getOwnerPV()
{
	RBXASSERT(!dirty);
	G3D::Vector3 cofmOffset = body->getCofmOffset();
	return pv.pvAtLocalOffset(G3D::Vector3(-cofmOffset.x, -cofmOffset.y, -cofmOffset.z));
}

//compiler optimizes out unused functions in header
void SimBody::matchDummy()
{
	resetAccumulators();
	accumulateForce(G3D::Vector3(0.1f, 0.5f, 0.2f), G3D::Vector3(0.6f, 0.10f, 0.8f));
}

}