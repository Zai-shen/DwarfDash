#include "Geometry.h"


glm::mat4 Geometry::getModelMatrix() {
	return _modelMatrix;
}

void Geometry::setTransformMatrix(glm::mat4 transformationMatrix) {
	_transformMatrix = transformationMatrix;
}

//void Geometry::transform(glm::mat4 transformationMatrix) {
//	_modelMatrix = _modelMatrix * transformationMatrix;
//}