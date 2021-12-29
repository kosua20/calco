#include "core/Types.hpp"


Value Value::convert(const Type& target, bool& success) const {
	success = true;
	if(target == type){
		return *this;
	}

	switch (type) {
		case BOOL:
			switch (target) {
				case INTEGER:
					return Value((long long)(b ? 1 : 0));
				case FLOAT:
					return Value(b ? 1.0 : 0.0);
				case STRING:
					return Value(b ? "true" : "false");
				default:
					// Error
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
				case STRING:
					return Value(std::to_string(i));
				default:
					// Error
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
				case STRING:
					return Value(std::to_string(f));
				default:
					// Error
					break;
			}
			break;
		case VEC4:
			switch (target) {
				case STRING:
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
					return Value(ms);
				}
				default:
					// Error
					break;
			}
			break;
		case MAT4:
			switch (target) {
				case STRING:
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
					return Value(ms);
				}
				default:
					// Error
					break;
			}
			break;

		default:
			break;
	}
	success = false;
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

Value FunctionCall::evaluate(TreeVisitor& visitor) {
	return visitor.process(*this);
}
