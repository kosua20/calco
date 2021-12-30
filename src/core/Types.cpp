#include "core/Types.hpp"

std::string Value::toString() const {
	switch (type) {
		case BOOL:
			return b ? "true" : "false";
		case INTEGER:
			return std::to_string(i);
		case FLOAT:
			return std::to_string(f);
		case VEC4:
		{
			std::string ms;
			ms.append("| ");
			for(int i = 0; i < 4; ++i){
				ms.append(std::to_string(vec[i]));
				if(i < 3){
					ms.append(", ");
				}
			}
			ms.append(" |");
			return ms;
		}
		case MAT4:
		{
			std::string ms;
			for(int i = 0; i < 4; ++i){
				ms.append("| ");
				for(int j = 0; j < 4; ++j){
					ms.append(std::to_string(mat[i][j]));
					if(j < 3){
						ms.append(", ");
					}
				}
				ms.append(" |");
				if(i < 3){
					ms.append("\n");
				}
			}
			return ms;
		}
		default:
			break;
	}
	return "unknown";
}

/** Type promotions;
 bool -> int, bool -> float, bool->vec4, bool->mat4
 int -> bool, int -> float, int->vec4, int->mat4
 float -> bool, float -> float, float->vec4, float->mat4

 vec4 and mat4 -> no promotion
 */
bool Value::convert(const Type& target, Value& outVal) const {

	if(target == type){
		outVal = *this;
		return true;
	}

	// String conversion is always possible.
	if(target == Value::STRING){
		outVal = toString();
		return true;
	}

	switch (type) {
		case BOOL:
			switch (target) {
				case INTEGER:
					outVal = (long long)(b ? 1 : 0);
					return true;
				case FLOAT:
					outVal = b ? 1.0 : 0.0;
					return true;
				case VEC4:
					outVal = glm::vec4(double(i));
					return true;
				case MAT4:
					outVal = glm::mat4(double(i));
					return true;
				default:
					break;
			}
			break;
		case INTEGER:
			switch (target) {
				case BOOL:
					outVal = (i != 0);
					return true;
				case FLOAT:
					outVal = double(i);
					return true;
				case VEC4:
					outVal = glm::vec4(double(i));
					return true;
				case MAT4:
					outVal = glm::mat4(double(i));
					return true;
				default:
					break;
			}
			break;
		case FLOAT:
			switch (target) {
				case BOOL:
					outVal = f != 0.0;
					return true;
				case INTEGER:
					outVal = (long long)(f);
					return true;
				case VEC4:
					outVal = glm::vec4(f);
					return true;
				case MAT4:
					outVal = glm::mat4(f);
					return true;
				default:
					break;
			}
			break;
		case VEC4:
		case MAT4:
		default:
			break;
	}
	return false;
}

Value Unary::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value Binary::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value Ternary::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value Literal::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value Member::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value Variable::evaluate(TreeVisitor& visitor)  {
	return visitor.process(*this);
}

Value VariableDef::evaluate(TreeVisitor& visitor) {
	return visitor.process(*this);
}

Value FunctionDef::evaluate(TreeVisitor& visitor) {
	return visitor.process(*this);
}

Value FunctionVar::evaluate(TreeVisitor& visitor) {
	return visitor.process(*this);
}

Value FunctionCall::evaluate(TreeVisitor& visitor) {
	return visitor.process(*this);
}

Status TreeVisitor::getStatus() const {
	return !_failed ? Status(true) : Status(0, _failedMessage);
}

Status Expression::evaluate(TreeVisitor& visitor, Value& outValue){
	outValue = evaluate(visitor);
	return visitor.getStatus();
}
