#include "TransformWidget.h"
#include "EchoEngine.h"
#include <engine/core/geom/Ray.h>

namespace Studio
{
	static float AresTwoLineAngle(const Echo::Vector3& lineFirst, const Echo::Vector3& lineSecond)
	{
		float length = lineFirst.len() * lineSecond.len();
		if (length == 0.0f)
		{
			return 0.0f;
		}

		length = lineFirst.dot(lineSecond) / length;
        length = std::max<float>(-1.0f, length);
        length = std::min<float>(1.0f, length);

		return acos(length);
	}

	static float TranslateOnAxis(const Echo::Vector3& rayPos0, const Echo::Vector3& rayDir0, const Echo::Vector3& rayPos1, const Echo::Vector3& rayDir1, const Echo::Vector3& entityPos, const Echo::Vector3& translateAxis)
	{
		// 结果
		float result = 0.0f;

		// 求面法线
		Echo::Vector3 rayAssistant;
		rayAssistant = rayDir0.cross(rayDir1);
		rayAssistant.normalize();
		float test = rayAssistant.dot(translateAxis) / (rayAssistant.len() * translateAxis.len());

		// 严重的精度问题
		if (test > 0.3f || test < -0.3f)
		{
			return result;
		}

		rayAssistant = rayAssistant.cross(translateAxis);
		rayAssistant.normalize();

		// 如果平行
		if (!rayAssistant.len())
		{
			return result;
		}

		//// 根据点与法线确定面
		//Plane3   axisPlane(entityPos, rayAssistant);
		//axisPlane.Normalize();

		//// 求出面与射线相交点
		//Line3 line0(rayPos0, rayDir0);
		//Line3 line1(rayPos1, rayDir1);
		//IntrLine3Plane3 intrLP0(line0, axisPlane);
		//IntrLine3Plane3 intrLP1(line1, axisPlane);
		//if (!intrLP0.Test() || !intrLP1.Test())
		//	return result;

		//Vector3 pointBegin = intrLP0.m_intrPoint;
		//Vector3 pointEnd = intrLP1.m_intrPoint;

		//// 求在移动轴上投影长度
		//Vector3 rayMove = pointEnd - pointBegin;
		//result = Vector3Dot(rayMove, translateAxis) / translateAxis.Length();

		return result;
	}

	// 在平面上移动, 若射与平面不相交 return NULL
	//static Echo::Vector3* TranslateOnPlane(Vector3* pOut, const Plane3& plane, const Vector3& rayPos0, const Vector3& rayDir0, const Vector3& rayPos1, const Vector3& rayDir1)
	//{
	//	// 求出面与射线相交点
	//	Line3 line0(rayPos0, rayDir0);
	//	Line3 line1(rayPos1, rayDir1);
	//	IntrLine3Plane3 intrLP0(line0, plane);
	//	IntrLine3Plane3 intrLP1(line1, plane);
	//	if (!intrLP0.Test() || !intrLP1.Test())
	//	{
	//		*pOut = Vector3::Zero;

	//		return NULL;
	//	}

	//	Vector3 pointBegin = intrLP0.m_intrPoint;
	//	Vector3 pointEnd = intrLP1.m_intrPoint;

	//	// 求在移动轴上投影长度
	//	*pOut = pointEnd - pointBegin;

	//	return pOut;
	//}

	//// 在平面上旋转,返回相对旋转矩阵(第一个参数返回角度,)
	//static float RotateOnPlane(const Vector3& planePoint, const Vector3& planeNormal, const Vector3& rayPos0, const Vector3& rayDir0, const Vector3& rayPos1, const Vector3& rayDir1)
	//{
	//	// 根据点与射线确定面
	//	Plane3   rotatePlane(planePoint, planeNormal);
	//	Line3 line0(rayPos0, rayDir0);
	//	Line3 line1(rayPos1, rayDir1);
	//	IntrLine3Plane3 intrLP0(line0, rotatePlane);
	//	IntrLine3Plane3 intrLP1(line1, rotatePlane);
	//	if (!intrLP0.Test() || !intrLP1.Test())
	//		return 0.f;

	//	Vector3 pointBegin = intrLP0.m_intrPoint;
	//	Vector3 pointEnd = intrLP1.m_intrPoint;

	//	// 求旋转角度
	//	Matrix44 pOut;
	//	float tpAngle = AresTwoLineAngle(pointBegin - planePoint, pointEnd - planePoint);
	//	Matrix44RotationAxisAngle(pOut, planeNormal, tpAngle);

	//	// 角度正负
	//	Vector3 normal;
	//	normal = Vector3Cross(planePoint - pointBegin, pointEnd - planePoint);
	//	if (Vector3Dot(normal, planeNormal) < 0.0f)
	//	{
	//		tpAngle = -tpAngle;
	//	}

	//	return tpAngle;
	//}

	TransformWidget::TransformWidget()
	{
		m_fScale = 1.f;

		m_editType = EditType::Translate;
		m_rotateType = RotateType::None;
		m_moveType = MoveType::None;
		m_position = Echo::Vector3::ZERO;

		m_axis = ECHO_DOWN_CAST<Echo::Gizmos*>(Echo::Class::create("Gizmos"));
		m_axis->setParent(EchoEngine::instance()->getInvisibleEditorNode());
		m_axis->setVisible(true);

		draw();
	}

	void TransformWidget::draw()
	{
		using namespace Echo;

		// color
		const Echo::Color White(1.f, 1.f, 1.f, 1.f);

		m_axis->clear();
		m_axis->setRenderType("3d");

		// axis line
		m_axis->drawLine(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), isMoveType(MoveType::XAxis) ? White : Echo::Color::RED);
		m_axis->drawLine(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), isMoveType(MoveType::YAxis) ? White : Echo::Color::GREEN);
		m_axis->drawLine(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), isMoveType(MoveType::ZAxis) ? White : Echo::Color::BLUE);

		// plane
		m_axis->drawLine(Vector3(0.4f, 0.0f, 0.0f), Vector3(0.4f, 0.4f, 0.0f), isMoveType(MoveType::XYPlane) ? White : Color::RED);
		m_axis->drawLine(Vector3(0.4f, 0.4f, 0.0f), Vector3(0.0f, 0.4f, 0.0f), isMoveType(MoveType::XYPlane) ? White : Color::GREEN);
		m_axis->drawLine(Vector3(0.0f, 0.4f, 0.0f), Vector3(0.0f, 0.4f, 0.4f), isMoveType(MoveType::YZPlane) ? White : Color::GREEN);
		m_axis->drawLine(Vector3(0.0f, 0.4f, 0.4f), Vector3(0.0f, 0.0f, 0.4f), isMoveType(MoveType::YZPlane) ? White : Color::BLUE);
		m_axis->drawLine(Vector3(0.4f, 0.0f, 0.4f), Vector3(0.0f, 0.0f, 0.4f), isMoveType(MoveType::XZPlane) ? White : Color::BLUE);
		m_axis->drawLine(Vector3(0.4f, 0.0f, 0.0f), Vector3(0.4f, 0.0f, 0.4f), isMoveType(MoveType::XZPlane) ? White : Color::RED);



		// cones
		drawCone(0.1f, 0.6f, Transform(Vector3::UNIT_X, Vector3::ONE, Quaternion::IDENTITY), isMoveType(MoveType::XAxis) ? White : Color::RED);
		drawCone(0.1f, 0.6f, Transform(Vector3::UNIT_Y, Vector3::ONE, Quaternion::fromVec3ToVec3(Echo::Vector3::UNIT_X, Echo::Vector3::UNIT_Y)), isMoveType(MoveType::YAxis) ? White : Echo::Color::GREEN);
		drawCone(0.1f, 0.6f, Transform(Vector3::UNIT_Z, Vector3::ONE, Quaternion::fromVec3ToVec3(Echo::Vector3::UNIT_X, Echo::Vector3::UNIT_Z)), isMoveType(MoveType::ZAxis) ? White : Echo::Color::BLUE);

		//// 三个圆
		//for (int i = 0; i < 3; i++)
		//{
		//	m_pCycle[i] = visualShapeMgr.CreateCycle(5);
		//	m_pCycle[i]->SetRadius(1.0f);
		//	m_pCycle[i]->SetFace(m_vAxisDir[i]);
		//	m_pCycle[i]->SetVisible(false);
		//	//m_pCycle[i]->SetSizeFixed( true);
		//}

		//m_pCycle[0]->SetColor(0xFFFF0000);
		//m_pCycle[1]->SetColor(0xFF00FF00);
		//m_pCycle[2]->SetColor(0xFF0000FF);

		//// 缩放
		//m_pScale = visualShapeMgr.CreateVisualShape(5);
		//Vector3 positions[] =
		//{
		//	Vector3(0.5f, 0.0f, 0.0f),
		//	Vector3(0.0f, 0.5f, 0.0f),
		//	Vector3(0.0f, 0.0f, 0.5f),
		//};

		//WORD  indices[] = { 0, 1, 2 };

		//m_pScale->Set(RenderLayout::TT_TriangleList, positions, sizeof(positions), indices, sizeof(indices), EF_R16UI);
		//m_pScale->SetColor(0xFF33AAFF);
		//m_pScale->SetVisible(false);
	}

	void TransformWidget::drawCone(float radius, float height, const Echo::Transform& transform, const Echo::Color& color)
	{
		// init positions
		Echo::Vector3 positions[27];
		for (int i = 0; i < 25; i++)
		{
			float theta = (2 * Echo::Math::PI * i) / 24.0f;

			positions[i].x = 0.0f;
			positions[i].y = sinf(theta) * radius;
			positions[i].z = cosf(theta) * radius;
		}
		positions[25] = Echo::Vector3(0.0f, 0.0f, 0.0f);
		positions[26] = Echo::Vector3(height, 0.0f, 0.0f);

		// transform
		for (int i = 0; i < 27; i++)
		{
			positions[i] = transform.transformVec3(positions[i]);
		}

		// draw triangles
		for (int i = 0; i < 25; i++)
		{
			float theta = (2 * Echo::Math::PI * i) / 24.0f;

			m_axis->drawTriangle(positions[i], positions[i + 1], positions[25], color);
			m_axis->drawTriangle(positions[i], positions[26], positions[i+1], color);
		}
	}

	//void Axis3D::OnMouseMove(const Vector3& rayOrig0, const Vector3& rayDir0, const Vector3& rayOrig1, const Vector3& rayDir1, POINT* ptPre/*=NULL*/, POINT* ptCurr/*=NULL*/)
	//{
	//	if (m_bVisible)
	//	{
	//		switch (m_editType)
	//		{
	//		case EM_EDIT_TRANSLATE:
	//		{
	//			// 移动
	//			float       fDist = 0.0f;
	//			Echo::Vector3 relaTrans;
	//			switch (m_moveType)
	//			{
	//			case EM_MOVE_X:
	//			{
	//				fDist = TranslateOnAxis(rayOrig0, rayDir0, rayOrig1, rayDir1, m_vPosition, m_vAxisDir[EM_MOVE_X]);
	//				relaTrans = fDist * m_vAxisDir[EM_MOVE_X];
	//			}
	//			break;
	//			case EM_MOVE_Y:
	//			{
	//				fDist = TranslateOnAxis(rayOrig0, rayDir0, rayOrig1, rayDir1, m_vPosition, m_vAxisDir[EM_MOVE_Y]);
	//				relaTrans = fDist * m_vAxisDir[EM_MOVE_Y];
	//			}
	//			break;
	//			case EM_MOVE_Z:
	//			{
	//				fDist = TranslateOnAxis(rayOrig0, rayDir0, rayOrig1, rayDir1, m_vPosition, m_vAxisDir[EM_MOVE_Z]);
	//				relaTrans = fDist * m_vAxisDir[EM_MOVE_Z];
	//			}
	//			break;
	//			case EM_MOVE_XYPLANE:
	//			{
	//				Plane3 plane(m_vPosition, m_vAxisDir[EM_MOVE_Z]);
	//				TranslateOnPlane(&relaTrans, plane, rayOrig0, rayDir0, rayOrig1, rayDir1);
	//			}
	//			break;
	//			case EM_MOVE_YZPLANE:
	//			{
	//				Plane3 plane(m_vPosition, m_vAxisDir[EM_MOVE_X]);
	//				TranslateOnPlane(&relaTrans, plane, rayOrig0, rayDir0, rayOrig1, rayDir1);
	//			}
	//			break;
	//			case EM_MOVE_XZPLANE:
	//			{
	//				Plane3 plane(m_vPosition, m_vAxisDir[EM_MOVE_Y]);
	//				TranslateOnPlane(&relaTrans, plane, rayOrig0, rayDir0, rayOrig1, rayDir1);
	//			}
	//			break;
	//			default:
	//			{
	//				return;
	//			}
	//			}

	//			Translate(relaTrans);
	//		}
	//		break;
	//		case EM_EDIT_ROTATE:
	//		{
	//			float fAngle0 = 0.0f;
	//			float fAngle1 = 0.0f;
	//			float fAngle2 = 0.0f;

	//			switch (m_rotateType)
	//			{
	//			case EM_ROTATE_X:
	//			{
	//				fAngle0 = RotateOnPlane(m_vPosition, m_vAxisDir[0], rayOrig0, rayDir0, rayOrig1, rayDir1);
	//			}
	//			break;
	//			case EM_ROTATE_Y:
	//			{
	//				fAngle1 = RotateOnPlane(m_vPosition, m_vAxisDir[1], rayOrig0, rayDir0, rayOrig1, rayDir1);
	//			}
	//			break;
	//			case EM_ROTATE_Z:
	//			{
	//				fAngle2 = RotateOnPlane(m_vPosition, m_vAxisDir[2], rayOrig0, rayDir0, rayOrig1, rayDir1);
	//			}
	//			break;
	//			}

	//			// 旋转所有模型
	//			for (size_t i = 0; i < m_entityList.size(); i++)
	//			{
	//				Transform tranform = m_entityList[i]->GetTransform();
	//				tranform.AddRotation(m_vAxisDir[0], fAngle0);
	//				tranform.AddRotation(m_vAxisDir[1], fAngle1);
	//				tranform.AddRotation(m_vAxisDir[2], fAngle2);

	//				m_entityList[i]->SetTransform(tranform);
	//			}

	//			// 旋转所有模型
	//			for (size_t i = 0; i < m_transforms.size(); i++)
	//			{
	//				m_transforms[i]->AddRotation(m_vAxisDir[0], fAngle0);
	//				m_transforms[i]->AddRotation(m_vAxisDir[1], fAngle1);
	//				m_transforms[i]->AddRotation(m_vAxisDir[2], fAngle2);
	//			}
	//		}
	//		break;

	//		case EM_EDIT_SCALE:
	//		{
	//			// 缩放所有模型
	//			for (size_t i = 0; i < m_entityList.size(); i++)
	//			{
	//				Transform tranform = m_entityList[i]->GetTransform();
	//				float scale = tranform.GetScale().x - 0.002f * (ptPre->y - ptCurr->y);
	//				tranform.SetScale(scale);

	//				m_entityList[i]->SetTransform(tranform);
	//			}

	//			// 旋转所有模型
	//			for (size_t i = 0; i < m_transforms.size(); i++)
	//			{
	//				float scale = m_transforms[i]->GetScale().x - 0.002f * (ptPre->y - ptCurr->y);
	//				m_transforms[i]->SetScale(scale);
	//			}
	//		}
	//		break;
	//		}
	//	}
	//}

	void TransformWidget::updateTranslateCollisionBox()
	{
		using namespace Echo;

		m_moveBoxs[int(MoveType::XAxis)].set(m_position + Vector3(1.f, 0.f, 0.f), Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z, 0.60f, 0.15f, 0.15f);
		m_moveBoxs[int(MoveType::YAxis)].set(m_position + Vector3(0.f, 1.f, 0.f), Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z, 0.15f, 0.60f, 0.15f);
		m_moveBoxs[int(MoveType::ZAxis)].set(m_position + Vector3(0.f, 0.f, 1.f), Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z, 0.15f, 0.15f, 0.60f);
		m_moveBoxs[int(MoveType::XYPlane)].set(m_position + Vector3(0.2f, 0.2f, 0.0f), Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z, 0.2f, 0.2f, 0.05f);
		m_moveBoxs[int(MoveType::YZPlane)].set(m_position + Vector3(0.0f, 0.2f, 0.2f), Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z, 0.05f, 0.2f, 0.2f);
		m_moveBoxs[int(MoveType::XZPlane)].set(m_position + Vector3(0.2f, 0.0f, 0.2f), Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z, 0.2f, 0.05f, 0.2f);
	}

	bool TransformWidget::onMouseDown(const Echo::Vector2& localPos)
	{
		Echo::Camera* camera = Echo::NodeTree::instance()->get3dCamera();
		if (camera)
		{
			// get camera ray
			Echo::Ray ray;
			camera->getCameraRay(ray, localPos);

			// update collision box
			updateTranslateCollisionBox();

			// hit operate
			if (m_isVisible)
			{
				switch (m_editType)
				{
				case EditType::Translate:
				{
					m_moveType = MoveType::None;
					for (int i = int(MoveType::XAxis); i <= int(MoveType::XZPlane); i++)
					{
						if (ray.hitBox3(m_moveBoxs[i]))
						{
							m_moveType = MoveType(i);
							break;
						}
					}

					// update display
					draw();
				}
				break;
				//case EM_EDIT_ROTATE:
				//{
				//	float fLastDist = 1e30f;
				//	float fDist;
				//	Vector3 intersectPos;
				//	Plane3 plane;
				//	m_rotateType = EM_ROTATE_NULL;

				//	// 选择
				//	for (int i = 0; i < 3; i++)
				//	{
				//		Line3 ray(rayOrig, rayDir);
				//		Plane3 plane(m_vPosition, m_vAxisDir[i]);
				//		IntrLine3Plane3 intrLP(ray, plane);
				//		if (intrLP.Test())
				//		{
				//			fDist = intrLP.m_distance;
				//			intersectPos = intrLP.m_intrPoint;
				//			if ((intersectPos - m_vPosition).Length() > 0.8f*m_fScale && (intersectPos - m_vPosition).Length() < 1.2f*m_fScale)
				//			{
				//				if (i == 0) m_rotateType = EM_ROTATE_X;
				//				if (i == 1) m_rotateType = EM_ROTATE_Y;
				//				if (i == 2) m_rotateType = EM_ROTATE_Z;

				//				fLastDist = fDist;

				//				break;
				//			}
				//		}
				//	}

				//	// 显示
				//	switch (m_rotateType)
				//	{
				//	case EM_ROTATE_X:
				//		m_pCycle[0]->SetColor(0xFFFFFFFF);
				//		break;
				//	case EM_ROTATE_Y:
				//		m_pCycle[1]->SetColor(0xFFFFFFFF);
				//		break;
				//	case EM_ROTATE_Z:
				//		m_pCycle[2]->SetColor(0xFFFFFFFF);
				//		break;
				//	}
				//}
				//break;
				//case EM_EDIT_SCALE:
				//{
				//	Triangle3 triangle(m_vPosition, m_vPosition, m_vPosition);
				//	triangle.m_v[0].x += 0.5f*m_fScale; triangle.m_v[1].y += 0.5f*m_fScale; triangle.m_v[2].z += 0.5f*m_fScale;
				//	Line3 line3(rayOrig, rayDir);
				//	float fdist;

				//	if (Intersect(line3, triangle, fdist))
				//	{
				//		m_pScale->SetColor(0xFFFFFFFF);
				//	}
				//}
				//break;
				}
			}
		}

		return false;
	}

	void TransformWidget::onMouseUp()
	{
		m_moveType = MoveType::None;
		m_rotateType = RotateType::None;

		draw();
	}

	void TransformWidget::SetPosition(float _posX, float _posY, float _posZ)
	{/*
		m_vPosition.x = _posX;
		m_vPosition.y = _posY;
		m_vPosition.z = _posZ;

		for (int i = 0; i < 3; i++)
		{
			m_pAxes[i]->GetTransform()->SetTrans(_posX, _posY, _posZ);
			m_pPlaneLine[i]->GetTransform()->SetTrans(_posX, _posY, _posZ);
			m_pPlaneLine[i + 3]->GetTransform()->SetTrans(_posX, _posY, _posZ);
			m_pCycle[i]->GetTransform()->SetTrans(_posX, _posY, _posZ);
		}

		// 设置位置
		m_pCone[0]->GetTransform()->SetTrans(m_fScale + m_vPosition.x, m_vPosition.y, m_vPosition.z);
		m_pCone[1]->GetTransform()->SetTrans(m_vPosition.x, m_fScale + m_vPosition.y, m_vPosition.z);
		m_pCone[2]->GetTransform()->SetTrans(m_vPosition.x, m_vPosition.y, m_fScale + m_vPosition.z);

		m_pScale->GetTransform()->SetTrans(_posX, _posY, _posZ);

		// 更新碰撞检测盒子位置
		UpdateTranslateCollBox();*/
	}

	void TransformWidget::SetPosition(const Echo::Vector3& pos)
	{
		this->SetPosition(pos.x, pos.y, pos.z);
	}

	void TransformWidget::setVisible(bool visible)
	{
		/*
		// 本地记录
		m_bVisible = visible;

		switch (m_editType)
		{
		case EM_EDIT_TRANSLATE:
		{
			for (int i = 0; i < 3; i++)
			{
				m_pAxes[i]->SetVisible(visible);
				m_pPlaneLine[i]->SetVisible(visible);
				m_pPlaneLine[3 + i]->SetVisible(visible);
				m_pCone[i]->SetVisible(visible);
			}
		}
		break;
		case EM_EDIT_ROTATE:
		{
			for (int i = 0; i < 3; i++)
			{
				m_pCycle[i]->SetVisible(visible);
			}
		}
		break;
		case EM_EDIT_SCALE:
		{
			for (int i = 0; i < 3; i++)
			{
				m_pAxes[i]->SetVisible(visible);
				m_pCone[i]->SetVisible(visible);
				m_pScale->SetVisible(visible);
			}
		}
		break;
		}*/
	}

	void  TransformWidget::Translate(const Echo::Vector3& trans)
	{
		/*
		if (trans != Echo::Vector3::ZERO)
		{
			Echo::Vector3 pos = m_vPosition + trans;

			// 更改自身位置
			this->SetPosition(pos);

			// 更改所有模型位置
			for (size_t i = 0; i < m_entityList.size(); i++)
			{
				Transform transform = m_entityList[i]->GetTransform();
				Vector3 newPosition = transform.GetTrans() + trans;
				//newPosition.x = (int)(newPosition.x * 50) * 0.02f;
				//newPosition.y = (int)(newPosition.y * 50) * 0.02f;
				//newPosition.z = (int)(newPosition.z * 50) * 0.02f;
				transform.SetTrans(newPosition);
				m_entityList[i]->SetTransform(transform);
			}

			for (size_t i = 0; i < m_transforms.size(); i++)
			{
				Echo::Vector3 newPosition = m_transforms[i]->GetTrans() + trans;
				m_transforms[i]->SetTrans(newPosition);
			}
		}*/
	}

	void TransformWidget::SetEditType(EditType type)
	{
		this->setVisible(false);
		m_editType = type;
		this->setVisible(true);
	}

	void  TransformWidget::SetScale(float fScale)
	{
		/*
		m_fScale = fScale;
		for (int i = 0; i < 3; i++)
		{
			//m_pAxes[i]->GetTransform()->SetScale( fScale);
			m_pPlaneLine[i]->GetTransform()->SetScale( fScale);
			m_pPlaneLine[3+i]->GetTransform()->SetScale( fScale);
			m_pCone[i]->GetTransform()->SetScale( fScale);
			m_pCycle[i]->GetTransform()->SetScale( fScale);
		}

		// 设置位置
		m_pCone[0]->GetTransform()->SetTrans(m_fScale + m_vPosition.x, m_vPosition.y, m_vPosition.z);
		m_pCone[1]->GetTransform()->SetTrans(m_vPosition.x, m_fScale + m_vPosition.y, m_vPosition.z);
		m_pCone[2]->GetTransform()->SetTrans(m_vPosition.x, m_vPosition.y, m_fScale + m_vPosition.z);
		*/
	}
}
