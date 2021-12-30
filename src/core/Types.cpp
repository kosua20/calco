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

Value Value::convert(const Type& target, bool& success) const {
	success = true;
	if(target == type){
		return *this;
	}

	// String conversion is always possible.
	if(target == Value::STRING){
		return toString();
	}

	switch (type) {
		case BOOL:
			switch (target) {
				case INTEGER:
					return Value((long long)(b ? 1 : 0));
				case FLOAT:
					return Value(b ? 1.0 : 0.0);
				default:
					break;
			}
			break;
		case INTEGER:
			switch (target) {
				case BOOL:
					return Value(i != 0);
				case FLOAT:
					return Value(double(i));
				case VEC4:
					return Value(glm::vec4(double(i)));
				case MAT4:
					return Value(glm::mat4(double(i)));
				default:
					break;
			}
			break;
		case FLOAT:
			switch (target) {
				case BOOL:
					return Value(f != 0.0);
				case INTEGER:
					return Value((long long)(f));
				case VEC4:
					return Value(glm::vec4(f));
				case MAT4:
					return Value(glm::mat4(f));
				default:
					break;
			}
			break;
		case VEC4:
		case MAT4:
		default:
			break;
	}
	success = false;
	return Value();
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
