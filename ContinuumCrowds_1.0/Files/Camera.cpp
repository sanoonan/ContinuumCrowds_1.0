#include "Camera.h"

using namespace std;

Camera :: Camera()
{
	m_position = m_focus = glm::vec3(0.0f);
}

Camera :: Camera(glm::vec3 position)
{
	m_position = position;
}

Camera :: Camera(glm::vec3 position, glm::vec3 focus)
{
	m_position = position;
	m_focus = focus;
}

glm::mat4 Camera :: getRotationMat()
{
	glm::mat4 ret = glm::lookAt(m_position, m_focus, glm::vec3(0.0f, 1.0f, 0.0f));
	return ret;
}


void Camera :: addTBar(TwBar *bar)
{
	TwAddVarRW(bar, "Camera Position", TW_TYPE_DIR3F, &m_position, "");
	TwAddVarRW(bar, "Camera Focus", TW_TYPE_DIR3F, &m_focus, "");
}
