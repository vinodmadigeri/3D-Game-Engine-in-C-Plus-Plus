
#include "PreCompiled.h"
#include "AABB.h"

namespace Engine
{
	/******************************************************************************
	 Function     : AABB COnstructor
	 Description  :  
	 Input        : const Vector3 &i_Center, float i_halfX, float i_halfY, float i_halfZ
	 Output       : 
	 Return Value :   
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	AABB::AABB(const Vector3 &i_Center, float i_halfX, float i_halfY, float i_halfZ):
		m_Center(i_Center),
		m_HalfX(i_halfX),
		m_HalfY(i_halfY),
		m_HalfZ(i_halfZ)
	{

	}
}