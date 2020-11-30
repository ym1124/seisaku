#include "ray_camera.h"
#include "calculator.h"

ray pinhole_camera::getRay(float u, float v)
{
	DirectX::XMFLOAT3 focus = getFocus();
	orthoNormalBasis(focus, right, up);
	//ピンホールの位置
	DirectX::XMFLOAT3 pinhole_pos = DirectX::XMFLOAT3(this->getEye().x + this->getFocus().x*pinhole_dist, this->getEye().y + this->getFocus().y*pinhole_dist, this->getEye().z + this->getFocus().z*pinhole_dist);
	//u,vに対応するセンサーの位置
	DirectX::XMFLOAT3 sensor_pos = DirectX::XMFLOAT3(this->getEye().x + u*right.x + v*up.x, this->getEye().y + u*right.y + v*up.y, this->getEye().z + u*right.z + v*up.z);

	return ray(sensor_pos, normalize(DirectX::XMFLOAT3(pinhole_pos.x - sensor_pos.x, pinhole_pos.y - sensor_pos.y, pinhole_pos.z - sensor_pos.z)));
}
